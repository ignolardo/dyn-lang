#ifndef VM_H_
#define VM_H_

#include "chunk.h"

#define STACK_MAX 256

typedef struct {
    Chunk *chunk;
    uint8_t *pc;
    Value stack[STACK_MAX];
    Value *sp;
} VM;

typedef enum {
    INTERPET_OK,
    INTERPET_COMPILE_ERROR,
    INTERPET_RUNTIME_ERROR,
} InterpretResult;

void init_VM();
void free_VM();
InterpretResult interpret(Chunk *chunk);
void push(Value value);
Value pop();
InterpretResult run();

#endif