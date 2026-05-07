# C EVM Interpreter

A small Ethereum Virtual Machine project written in C.

This is an experimental project focused on low-level C, bytecode decoding, stack-machine execution, and 256-bit arithmetic.

## Features

The project currently includes:

- EVM bytecode disassembly
- A basic execution loop
- Stack operations
- Execution halting with `STOP`
- `uint256_t` arithmetic helpers
- Initial support for arithmetic opcodes such as `ADD`, `SUB`, `MUL`, `DIV`, and `MOD`

## Build

```bash
make
```

This creates the executable:

```text
build/evm
```

## Usage

Run the built-in demo:

```bash
make demo
```

Disassemble bytecode:

```bash
make disasm HEX=6001600201
```

Run bytecode through the experimental EVM executor:

```bash
make exec HEX=6001600201
```

Run whole test suite:

```bash
make test
```

Run unit tests only:

```bash
make test-unit
```

Run integration tests only:

```bash
make test-integration
```

Equivalent direct commands:

```bash
./build/evm demo
./build/evm disasm 6001600201
./build/evm run 6001600201
```

## Example

```bash
make disasm HEX=600a600506
```

Expected output:

```text
PC       OPCODE     MNEMONIC        IMMEDIATE
------------------------------------------------------------
0        0x60       PUSH1           0A
2        0x60       PUSH1           05
4        0x06       MOD
```


```bash
make exec HEX=600160100100
```

`600160100100` corresponds to:

```text
0x60 0x01 -> PUSH1 0x01
0x60 0x10 -> PUSH1 0x10
0x01      -> ADD
0x00      -> STOP
```

Push `0x01` on the stack.

Push `0x10` on the stack.

Perform the `ADD` operation, popping two elements from the stack and pushing the result.

In this case, `0x01 + 0x10 = 0x11`.

The `STOP` opcode explicitly terminates EVM execution.

Final stack state should contain one element: `0x11`.

Expected output:

```text
./build/evm run 600160100100

Execution terminated successfully.

STACK (next empty slot = [1])
----------------------------------------------------------------------
[0] 0x0000000000000000000000000000000000000000000000000000000000000011
```

## Current Opcode Support

Implemented or partially implemented:

- `STOP`
- `ADD`
- `MUL`
- `SUB`
- `DIV`
- `MOD`
- `POP`
- `PUSH1` through `PUSH32`

More opcodes will be added incrementally.

## Project Structure

```text
include/     public headers
src/         source files
src/math/    uint256 arithmetic implementation
test/        unit and integration tests
docs/        architecture notes
build/       generated build output
```

## Portability Note

The `uint256_t` multiplication implementation currently uses `__uint128_t`. This is supported by GCC and Clang, including Clang on macOS, but it is not part of standard ISO C.

## Status

Experimental.

This is not a complete EVM implementation yet. The current focus is bytecode parsing, stack execution, arithmetic operations, and clean C project organization.

## Future

- Add remaining arithmetic opcodes
- Add memory region
- Add memory-related opcodes
- Add storage region
- Add storage-related opcodes
- Add transient storage region
- Add transient storage-related opcodes
- Add program-counter-altering opcodes
- Add stack-modifying opcodes
- Add gas tracking
- Add gas cost for each opcode
- Investigate support for contract creation
