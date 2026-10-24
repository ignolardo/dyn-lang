#ifndef BYTECODE_H_
#define BYTECODE_H_

typedef enum {
    LOAD_CONST,
    LET_NAME,
    SET_NAME,
    PRINT_OP,
    LOAD_NAME,
    BINARY_OP,
    UNARY_OP,
    RETURN_OP,
} OpCode;

#endif