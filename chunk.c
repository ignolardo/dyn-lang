/* 
    Code based on Robert Nystrom's Crafting Interpreters: Chunks of Bytecode
    https://craftinginterpreters.com/chunks-of-bytecode.html
*/

#include "chunk.h"
#include "memory.h"
#include "name.h"
#include "value.h"
#include "bytecode.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void init_chunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    init_value_array(&chunk->constants);
    init_name_array(&chunk->names);
}

void write_chunk(Chunk *chunk, uint8_t byte) {
    if (chunk->count >= chunk->capacity) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count++] = byte;
}

int add_constant(Chunk *chunk, Value value) {
    return write_value_array(&chunk->constants, value);
    //return chunk->constants.count - 1; // Index of where the new constant is located;
}

int set_name(Chunk *chunk, char *name, Value value) {
    return write_name_array(&chunk->names, (Name){.value=value, .str=strdup(name)});
}

int set_name_by_index(Chunk *chunk, int index, Value value) {
    return write_name_array_index(&chunk->names, index, value);
}

Value get_name(Chunk *chunk, char *name) {
    return read_name_array(&chunk->names, name);
}

Value get_name_by_index(Chunk *chunk, int index) {
    return read_name_array_index(&chunk->names, index);
}

int index_name(Chunk *chunk, char *name) {
    return index_name_array(&chunk->names, name);
}

void free_chunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    free_value_array(&chunk->constants);
    free_name_array(&chunk->names);
    init_chunk(chunk);
}


void disassemble_chunk(Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassemble_instruction(chunk, offset);
    }
}

int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

int loadConstantInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d (", name, constant);
    print_value(chunk->constants.values[constant]);
    printf(")\n");
    return offset + 2;
}

int letNameInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    const char *nameStr = chunk->names.names[constant].str;
    printf("%-16s %4d (%s)\n", name, constant, nameStr);
    return offset + 2;
}

int setNameInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    const char *nameStr = chunk->names.names[constant].str;
    printf("%-16s %4d (%s)\n", name, constant, nameStr);
    return offset + 2;
}

int loadNameInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    const char *nameStr = chunk->names.names[constant].str;
    printf("%-16s %4d (%s)\n", name, constant, nameStr);
    return offset + 2;
}

int binaryOpInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t op_b = chunk->code[offset + 1];
    char op_c;
    switch (op_b) {
        case 0: op_c='+'; break;
        case 1: op_c='-'; break;
        case 2: op_c='*'; break;
        case 3: op_c='/'; break;
    }
    printf("%-16s %4d (%c)\n", name, op_b, op_c);
    return offset + 2;
}

int unaryOpInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t op_b = chunk->code[offset + 1];
    char op_c;
    switch (op_b) {
        case 0: op_c='-'; break;
    }
    printf("%-16s %4d (%c)\n", name, op_b, op_c);
    return offset + 2;
}

int printInstruction(const char *name, int offset) {
    printf("%-16s\n", name);
    return offset + 1;
}

int disassemble_instruction(Chunk *chunk, int offset) {
    printf("%04d ", offset);

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case LOAD_CONST: return loadConstantInstruction("LOAD_CONSTANT", chunk, offset);
        case LET_NAME: return letNameInstruction("LET_NAME", chunk, offset);
        case SET_NAME: return setNameInstruction("SET_NAME", chunk, offset);
        case PRINT_OP: return printInstruction("PRINT", offset);
        case LOAD_NAME: return loadNameInstruction("LOAD_NAME", chunk, offset);
        case BINARY_OP: return binaryOpInstruction("BINARY_OP", chunk, offset);
        case UNARY_OP: return unaryOpInstruction("UNARY_OP", chunk, offset);
        case RETURN_OP: 
            printf("%-16s\n", "RETURN_OP");
            return offset + 1;
        default:
            printf("Uknown opcode %d\n", instruction);
            return offset + 1;
    }
}



