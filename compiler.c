#include "compiler.h"
#include "bytecode.h"
#include "chunk.h"
#include "parser.h"
#include "chunk.h"
#include "value.h"
#include <stdint.h>
#include <stdio.h>

Chunk compile(pnode_t ast) {
    Chunk chunk;
    init_chunk(&chunk);

    check_node(&chunk, ast);
    write_chunk(&chunk, RETURN_OP);

    //disassembleChunk(&chunk, "main chunk");
    //freeChunk(&chunk);
    return chunk;
}

void let_handler(Chunk *chunk, pnode_t node) {
    uint16_t *header = get_children_header(node);
    for (register uint16_t i = 0; i < header[0]; i++) {
        int name = set_name(chunk, (char*)node->children[i]->data, NUMBER_VAL(0));
        write_chunk(chunk, LET_NAME);
        write_chunk(chunk, name);
    }
}

void set_handler(Chunk *chunk, pnode_t node) {
    pnode_t left = node->children[0];
    pnode_t right = node->children[1];
    int name = index_name(chunk, (char*)left->data);
    //printf("SIZE LEFT: %ld\n", strlen((char*)left->data));
    
    if (right->type==ID) {
        //printf("SIZE RIGHT: %ld\n", strlen((char*)right->data));
        int rname = index_name(chunk, (char*)right->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, rname);
    } else check_node(chunk, right);

    //printf("------\n");
    
    write_chunk(chunk, SET_NAME);
    write_chunk(chunk, name);
}

void const_handler(Chunk *chunk, pnode_t node) {
    int constant = add_constant(chunk, NUMBER_VAL(*(long*)(node->data)));
    write_chunk(chunk, LOAD_CONST);
    write_chunk(chunk, constant);
}

void add_handler(Chunk *chunk, pnode_t node) {
    pnode_t left = node->children[0];
    pnode_t right = node->children[1];
    
    if (left->type==ID) {
        int name = index_name(chunk, (char*)left->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, left);
    
    if (right->type==ID) {
        int name = index_name(chunk, (char*)right->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, right);
    
    write_chunk(chunk, BINARY_OP);
    write_chunk(chunk, 0);
}

void sub_handler(Chunk *chunk, pnode_t node) {
    pnode_t left = node->children[0];
    pnode_t right = node->children[1];
    
    if (left->type==ID) {
        int name = index_name(chunk, (char*)left->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, left);
    
    if (right->type==ID) {
        int name = index_name(chunk, (char*)right->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, right);
    
    write_chunk(chunk, BINARY_OP);
    write_chunk(chunk, 1);
}

void mult_handler(Chunk *chunk, pnode_t node) {
    pnode_t left = node->children[0];
    pnode_t right = node->children[1];
    
    if (left->type==ID) {
        int name = index_name(chunk, (char*)left->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, left);
    
    if (right->type==ID) {
        int name = index_name(chunk, (char*)right->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, right);
    
    write_chunk(chunk, BINARY_OP);
    write_chunk(chunk, 2);
}

void div_handler(Chunk *chunk, pnode_t node) {
    pnode_t left = node->children[0];
    pnode_t right = node->children[1];
    
    if (left->type==ID) {
        int name = index_name(chunk, (char*)left->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, left);
    
    if (right->type==ID) {
        int name = index_name(chunk, (char*)right->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, right);
    
    write_chunk(chunk, BINARY_OP);
    write_chunk(chunk, 3);
}

void neg_handler(Chunk *chunk, pnode_t node) {
    pnode_t child = node->children[0];
    
    if (child->type==ID) {
        int name = index_name(chunk, (char*)child->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, child);
    
    write_chunk(chunk, UNARY_OP);
    write_chunk(chunk, 0);
}

void print_handler(Chunk *chunk, pnode_t node) {
    pnode_t child = node->children[0];
    
    if (child->type==ID) {
        //printf("SIZE: %ld STR: %s\n-------\n", strlen((char*)child->data), (char*)child->data);
        int name = index_name(chunk, (char*)child->data);
        write_chunk(chunk, LOAD_NAME);
        write_chunk(chunk, name);
    } else check_node(chunk, child);
    
    write_chunk(chunk, PRINT_OP);
}

void check_node(Chunk *chunk, pnode_t node) {
    
    if (node->type==BLOCK && node->children) {
        uint16_t *header = get_children_header(node);
        for (register uint16_t i = 0; i < header[0]; i++) {
            check_node(chunk, node->children[i]);
        }
    }
    
    switch (node->type) {
        case BLOCK: break;
        case LET: let_handler(chunk, node); break;
        case SET: set_handler(chunk, node); break;
        case PRINT: print_handler(chunk, node); break;
        case ADD: add_handler(chunk, node); break;
        case SUB: sub_handler(chunk, node); break;
        case MULT: mult_handler(chunk, node); break;
        case DIV: div_handler(chunk, node); break;
        case NEG: neg_handler(chunk, node); break;
        case ID: break;
        case CONSTANT: const_handler(chunk, node); break;
        default: printf("-----\n"); break;
    }
}