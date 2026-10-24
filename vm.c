#include "vm.h"
#include "bytecode.h"
#include "chunk.h"
#include "value.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

//#define DEBUG_TRACE_EXECUTION

VM vm;

void reset_stack() {
    vm.sp = vm.stack;
}

void init_VM() {
    reset_stack();
}

void push(Value value) {
    *vm.sp++ = value;
}

Value pop() {
    return *--vm.sp;
}

static Value peek(int dist) {
    return vm.sp[-1 - dist];
}

static void runtimeError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.pc - 1 - vm.chunk->code;
    fprintf(stderr, "[offset %ld] in bytecode", instruction);
    reset_stack();
}

InterpretResult interpret(Chunk *chunk) {
    vm.chunk = chunk;
    vm.pc = chunk->code;
    return run();
}

InterpretResult run() {
#define READ_BYTE() (*vm.pc++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(type, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers"); \
            return INTERPET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(type(a op b)); \
    } while (false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("STACK:  ");
        for (Value *slot = vm.stack; slot < vm.sp; slot++) {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");
        disassemble_instruction(vm.chunk, vm.pc - vm.chunk->code);
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case LET_NAME: {
                uint8_t i_name = READ_BYTE();
                break;
            }
            case SET_NAME: {
                uint8_t i_name = READ_BYTE();
                Value value = pop();
                set_name_by_index(vm.chunk, i_name, value);
                break;
            }
            case PRINT_OP: {
                Value value = pop();
                print_value(value);
                printf("\n");
                break;
            }
            case LOAD_NAME: {
                uint8_t i_name = READ_BYTE();
                Value value = get_name_by_index(vm.chunk, i_name);
                push(value);
                break;
            }
            case LOAD_CONST: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case BINARY_OP: {
                uint8_t op = READ_BYTE();
                switch (op) {
                    case 0: BINARY_OP(NUMBER_VAL, +); break;
                    case 1: BINARY_OP(NUMBER_VAL, -); break;
                    case 2: BINARY_OP(NUMBER_VAL, *); break;
                    case 3: BINARY_OP(NUMBER_VAL, /); break;
                }
                break;
            }
            case UNARY_OP: {
                uint8_t op = READ_BYTE();
                Value value = pop();
                switch (op) {
                    case 0:
                    default:
                        if (!IS_NUMBER(peek(0))) {
                            runtimeError("Operand must be a number");
                            return INTERPET_RUNTIME_ERROR;
                        }
                        double num = AS_NUMBER(value);
                        push(NUMBER_VAL(-num));
                        break;
                }
                break;
            }
            case RETURN_OP: return INTERPET_OK;
            default: {
                vm.pc += 2;
                printf("UKN\n");
                break;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}