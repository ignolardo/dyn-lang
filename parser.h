#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include <glib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define _MAX(n,...) max_variadic(n, __VA_ARGS__)

typedef enum ASTNodeType {
	SEQUENCE,
	BLOCK,
	STATEMENT,
	LET,
	SET,
	ADD,
	SUB,
	MULT,
	DIV,
	PRINT,
	ID,
	CONSTANT,
	NEG
} node_type_t;

typedef struct ASTNode {
	struct ASTNode **children;
	node_type_t type;
	void *data;
} node_t, *pnode_t;

typedef struct Parser {
	GList *tokens;
	pnode_t ast;
	pnode_t current;
} parser_t;

typedef enum KeywordHash {
	LET_H = 0x1a21b,
	SET_H = 0x1bc62,
	ADD_H = 0x178a1,
	PRINT_H = 0x65fb2ad,
} keyword_hash_t;

#define new_ast() new_node()

//extern GList *TOKENS;

void error(const char *msg);
void consumetok();
lexeme_type_t peektok();

bool accept(lexeme_type_t);
bool expect(lexeme_type_t);

pnode_t factor();
pnode_t term();
pnode_t expression();
pnode_t statement();
pnode_t block();
pnode_t program();


pnode_t new_node();
void free_node(pnode_t node);
parser_t new_parser(GList *tokens);
void free_parser(parser_t *parser);
uint16_t *get_children_header(pnode_t node);
void parse(parser_t *parser);
pnode_t backtrack(GList *ltoken, node_type_t type);
void print_ast(parser_t *parser);
char *parse_node_type(node_type_t type);


#endif
