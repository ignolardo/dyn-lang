#ifndef COMPILER_H_
#define COMPILER_H_

#include "parser.h"
#include "chunk.h"

Chunk compile(pnode_t ast);
void check_node(Chunk *chunk, pnode_t node);

#endif
