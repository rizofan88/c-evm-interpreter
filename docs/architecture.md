# Architecture

The project is split into small modules:

- `main.c`: command-line interface
- `evm.c`: execution loop and opcode dispatch
- `opcode.c`: opcode metadata and lookup
- `stack.c`: EVM stack operations
- `operations.c`: opcode semantics that interact with the stack
- `math/`: raw `uint256_t` arithmetic
- `utils.c`: byte and integer conversion helpers
- `disasm.c`: bytecode disassembly

## Execution Model

The interpreter reads EVM bytecode as a byte array. The execution loop in `evm.c` uses the program counter (`pc`) to fetch the current opcode, looks up opcode metadata with `evm_get_opcode()`, then dispatches to the matching operation.

`PUSH1` through `PUSH32` are handled as stack-push instructions. Their immediate bytes are copied into a 32-byte stack slot, right-aligned so that small values behave like normal 256-bit EVM words.

For example, this bytecode:

```text
6003600101
```

represents:

```text
PUSH1 0x03
PUSH1 0x01
ADD
```

The execution loop fetches each opcode, performs the stack operation, advances the program counter, and continues until the bytecode ends or execution is stopped.

## Stack Representation

The stack stores 256-bit EVM words as 32-byte arrays:

```c
uint8_t stack[EVM_STACK_LIMIT][UINT256_BYTES];
```

`stack_pointer` store the index of the next free slot:

```c
stack[stack_pointer]
```

When a value is pushed, it is written into the next free slot and `stack_pointer` is incremented. When values are popped, `stack_pointer` is decremented.

Small pushed values are right-aligned inside the 32-byte stack slot. For example, pushing `0x03` produces:

```text
0x0000000000000000000000000000000000000000000000000000000000000003
```

This matches the idea that EVM stack values are 256-bit words.

## Arithmetic Operations

Arithmetic opcodes such as `ADD`, `SUB`, `MUL`, `DIV`, and `MOD` are implemented in two layers:

- `operations.c` handles EVM-level behavior: reading operands from the stack, calling the math helper, popping operands, and pushing the result.
- `math/` contains raw `uint256_t` arithmetic helpers that do not know about the EVM stack.

The arithmetic flow is:

```text
32-byte stack words
        ↓
bytes_to_uint256()
        ↓
uint256_t arithmetic
        ↓
uint256_to_bytes()
        ↓
32-byte stack result
```

This keeps stack-machine behavior separate from integer arithmetic.

For example, the `ADD` operation:

1. Reads the top two stack values.
2. Converts them from 32-byte arrays into `uint256_t`.
3. Calls the `addition()` helper.
4. Pops the two operands.
5. Pushes the result back onto the stack.

The math helpers implement unsigned 256-bit arithmetic using four 64-bit limbs:

```c
typedef struct {
    uint64_t w[UINT256_LIMBS];
} uint256_t;
```

The least significant limb is stored in `w[0]`, and the most significant limb is stored in `w[3]`.

## Opcode Metadata

Opcode metadata is stored in `opcode.c`. Each opcode entry contains:

- the opcode byte
- the mnemonic
- the number of immediate bytes, if any

For example:

```c
{ 0x60, "PUSH1", 1 }
```

The execution loop and disassembler use `evm_get_opcode()` to look up information about each byte.

`PUSH1` through `PUSH32` are detected with a helper function because they all share the same execution pattern.

## Disassembler

The disassembler in `disasm.c` walks bytecode linearly, decodes each opcode, and prints a human-readable instruction table with:

- program counter
- opcode byte
- mnemonic
- immediate bytes for `PUSHn` instructions

Example output:

```text
PC       OPCODE     MNEMONIC        IMMEDIATE
------------------------------------------------------------
0        0x60       PUSH1           0A
2        0x60       PUSH1           05
4        0x06       MOD
```

The disassembler also bounds-checks truncated `PUSHn` immediates so it does not read past the bytecode buffer.

## Utilities

`utils.c` contains conversion helpers used by multiple modules.

Important helpers include:

- `hex_to_bytes()`: converts a hex string into a byte array
- `bytes_to_uint256()`: converts a 32-byte stack word into a `uint256_t`
- `uint256_to_bytes()`: converts a `uint256_t` back into a 32-byte array


## Current Limitations

This is not a complete EVM implementation. The following areas are not fully implemented yet:

- memory expansion and memory opcodes
- gas accounting
- signed arithmetic
- control flow
- jumps and jump destination validation
- contract calls
- storage and transient storage
- full EVM semantics
- complete opcode support

## Design Goal

The goal of this project is not to provide a production-ready EVM. The goal is to build a small, understandable EVM-style interpreter in C while practicing:

- byte-level parsing
- stack-machine execution
- modular C project organization
- unsigned 256-bit arithmetic
- low-level debugging
- clean build structure
