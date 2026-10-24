#ifndef CHUNK_H_
#define CHUNK_H_

#include "name.h"
#include "value.h"
#include <stdint.h>

typedef struct {
    int count;
    int capacity;
    uint8_t *code;
    ValueArray constants;
    NameArray names;
} Chunk;

void init_chunk(Chunk *chunk);
void write_chunk(Chunk *chunk, uint8_t byte);
int add_constant(Chunk *chunk, Value value);
int set_name(Chunk *chunk, char *name, Value value);
int set_name_by_index(Chunk *chunk, int index, Value value);
Value get_name(Chunk *chunk, char *name);
Value get_name_by_index(Chunk *chunk, int index);
int index_name(Chunk *chunk, char *name);
void free_chunk(Chunk *chunk);

void disassemble_chunk(Chunk *chunk, const char *name);
int disassemble_instruction(Chunk *chunk, int offset);

#endif