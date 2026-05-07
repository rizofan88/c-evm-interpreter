#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "disasm.h"
#include "opcode.h"
#include "evm.h"
#include "types.h"


#define DEMO_BYTECODE "600a600506"

typedef enum {
    MODE_DISASM,
    MODE_RUN
} program_mode_t;


const char *evm_status_helper(evm_status_t status) {

    switch (status) {
    case EVM_OK:
        return "EVM OK.";
    case EVM_STACK_OVERFLOW:
        return "EVM Stack Overflow.";
    case EVM_STACK_UNDERFLOW:
        return "EVM Stack Underflow.";
    case EVM_INVALID_OPCODE:
        return "EVM Invalid Opcode.";
    case MALLOC_ERROR:
        return "Malloc Error.";
    case EVM_INTERNAL_ERROR:
        return "EVM Internal Error.";
    default:
        return "Unknown EVM status.";
    }

}

static void print_usage(const char *program_name) {
    
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s disasm <hex bytecode>\n", program_name);
    fprintf(stderr, "  %s run <hex bytecode>\n", program_name);
    fprintf(stderr, "  %s demo\n", program_name);

}

int main(int argc, char **argv) {
    
    const char *input_hex = NULL;
    size_t bytecode_len = 0;
    uint8_t *bytecode = NULL;
    program_mode_t mode;

    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "demo") == 0) {
        input_hex = DEMO_BYTECODE;
        mode = MODE_RUN;
        
    } else if (strcmp(argv[1], "disasm") == 0 && argc == 3) {
        input_hex = argv[2];
        mode = MODE_DISASM;
    
    } else if (strcmp(argv[1], "run") == 0 && argc == 3) {
        input_hex = argv[2];
        mode = MODE_RUN;

    } else {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    bytecode = hex_to_bytes(input_hex, &bytecode_len);

    if (!bytecode) {
        fprintf(stderr, "Error: invalid hex string.\n");
        return EXIT_FAILURE;
    
    }

    if (mode == MODE_DISASM) {
        disasm(bytecode, bytecode_len);

    } else if (mode == MODE_RUN) {
        evm_status_t status = evm_state(bytecode, bytecode_len);

        if (status != EVM_OK) {
            fprintf(stderr, "EVM execution failed: %s\n", evm_status_helper(status));
            free(bytecode);
            return EXIT_FAILURE;
    
        }
    }

    free(bytecode);

    return EXIT_SUCCESS;

}

