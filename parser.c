#include <glib.h>
#include "parser.h"
#include "lexer.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>

GList *TOKENS;
bool eot; // END OF TOKENS
pnode_t node;

uint16_t *get_children_header(pnode_t node) {
    pnode_t *children = node->children;
    return (uint16_t*)children - 2;
}

pnode_t create_node(node_type_t type, int children) {
    pnode_t node = new_node();
    node->type = type;
    if (children) {
        uint16_t *header;
        header = malloc(2 * sizeof(uint16_t) + children * sizeof(pnode_t)); // 4 extra bytes for the header (count, capacity)
        header[0] = 0;
        header[1] = children;
        node->children = (pnode_t*)(header + 2);
    }
    return node;
}

void add_child(pnode_t node, pnode_t child) {
    uint16_t *header = get_children_header(node);
    uint16_t count = header[0];
    uint16_t capacity = header[1];
    if (count==capacity) {
        capacity *= 2;
        header = realloc(header, 2 * sizeof(uint16_t) + capacity * sizeof(pnode_t)); // reallocate
        header[0] = count;
        header[1] = capacity;
        node->children = (pnode_t*)(header + 2);
    }
    node->children[header[0]++] = child;
}

pnode_t new_node() {
	pnode_t node = malloc(sizeof(node_t));
	node->children = NULL;
	node->type = SEQUENCE;
	return node;
}



void free_node(pnode_t node) {
    if (node==NULL) return;
    if (node->children) {
        uint16_t *header = get_children_header(node);
        for (register uint16_t i = 0; i < header[0]; i++)
            free_node(node->children[i]);
        free(header);
    }
    free(node);
}

parser_t new_parser(GList *tokens) {
    TOKENS = tokens;
	eot = false;
	return (parser_t){
		.tokens=tokens,
		.ast=NULL,
		.current=NULL,
	};
}

void free_parser(parser_t *parser) {
    free_node(parser->ast);
    free_node(parser->current);
    g_list_free(parser->tokens);
}

pnode_t id_node(const char *id) {
    //printf("Creating id node: %s\n", id);
    pnode_t node = create_node(ID, 0);
    node->data = (void*)strdup(id);
    return node;
}

pnode_t const_node(const char *value) {
    pnode_t node = create_node(CONSTANT, 0);
    
    char *endptr;
    errno = 0;
    long num = strtol(value, &endptr, 10);

    if (errno == ERANGE)        error("number: overflow at conversion");
    else if (endptr == value)   error("number: couldn't convert number");

    node->data = malloc(sizeof(long));
    *(long*)(node->data) = num;
    
    return node;
}

/*
    op:
    0 MULTIPLICATION,
    1 DIVISION
*/
pnode_t mult_div_node(uint8_t op, pnode_t left, pnode_t right) {
    pnode_t node;
    if (op)     node = create_node(DIV, 2);
    else        node = create_node(MULT, 2);
    add_child(node, left);
    add_child(node, right);
    return node;
}

pnode_t neg_node(pnode_t cnode) {
    pnode_t node = create_node(NEG, 1);
    add_child(node , cnode);
    return node;
}
/*
    op:
    0 ADDITION,
    1 SUBSTRACTION
*/
pnode_t add_sub_node(uint8_t op, pnode_t left, pnode_t right) {
    pnode_t node;
    if (op)     node = create_node(SUB, 2);
    else        node = create_node(ADD, 2);
    add_child(node, left);
    add_child(node, right);
    return node;
}

void error(const char *msg) {
    printf("ERROR: %s\n", msg);
    exit(1);
}

void consumetok() {
    if (TOKENS->next) TOKENS = TOKENS->next;
    else eot = true;
}

lexeme_type_t peektok() {
    if (eot) return -1;
    return ((lexeme_t*)(TOKENS->data))->type;
}

bool accept(lexeme_type_t type) {
    if (peektok() == type) {
        consumetok();
        return true;
    }
    return false;
}

bool expect(lexeme_type_t type) {
    if (accept(type)) {
        return true;
    }
    error("expect: unexpected token");
    return false;
}


pnode_t factor() {
    pnode_t node;
    if (accept(IDENT)) {
        node = id_node(TOK(TOKENS->prev)->buffer->str);
        return node;
    }
    if (accept(LITERAL)) {
        node = const_node(TOK(TOKENS->prev)->buffer->str);
        return node;
    }
    if (accept(OPAR)) {
        node = expression();
        expect(CPAR);
    } else {
        error("factor: syntax error");
        consumetok();
    }
    return node;
}

pnode_t term() {
    pnode_t left, right;
    left = factor();
    while (peektok()==TIMES || peektok()==SLASH) {
        if (peektok()==TIMES) {
            consumetok();
            right = factor();
            left = mult_div_node(0, left, right); // Multiply
        } else {
            consumetok();
            right = factor();
            left = mult_div_node(1, left, right); // Divide
        }
    }
    return left;
}

pnode_t expression() {
    bool neg = false;
    pnode_t left, right;
    if (peektok()==PLUS || peektok()==MINUS) {
        if (peektok()==MINUS) neg = true;
        consumetok();
    }
    left = term();
    while (peektok()==PLUS || peektok()==MINUS) {
        if (peektok()==PLUS) {
            consumetok();
            right = term();
            left = add_sub_node(0, left, right); // Add
        } else {
            consumetok();
            right = term();
            left = add_sub_node(1, left, right); // Substract
        }
    }
    if (neg) left = neg_node(left);
    return left;
}

pnode_t statement() {
    pnode_t node;
    if (accept(LETK)) {
        node = create_node(LET, 1);
        expect(IDENT);
        pnode_t id = id_node(((lexeme_t*)(TOKENS->prev->data))->buffer->str);
        add_child(node, id);
        while (peektok()!=END) {
            expect(IDENT);
            id = id_node(((lexeme_t*)(TOKENS->prev->data))->buffer->str);
            add_child(node, id);
        }
        //printf("LET STATEMENT\n");
    } else if (accept(SETK)) {
        node = create_node(SET, 2);
        expect(IDENT);
        pnode_t id = id_node(((lexeme_t*)(TOKENS->prev->data))->buffer->str);
        add_child(node, id);
        pnode_t exp = expression();
        add_child(node, exp);
        //printf("SET STATEMENT\n");
    } else if (accept(ADDK)) {
        node = create_node(SET, 3);
        expect(IDENT);
        pnode_t id = id_node(((lexeme_t*)(TOKENS->prev->data))->buffer->str);
        add_child(node, id);
        pnode_t exp1 = expression();
        add_child(node, exp1);
        pnode_t exp2 = expression();
        add_child(node, exp2);
        //printf("ADD STATEMENT\n");
    } else if (accept(PRINTK)) {
        node = create_node(PRINT, 1);
        pnode_t exp = expression();
        add_child(node, exp);
        //printf("PRINT STATEMENT\n");
    } else {
        error("statement: syntax error");
        consumetok();
    }
    return node;
}

pnode_t block() {
    pnode_t node = create_node(BLOCK, 1);
    while (peektok()!=-1) {
        if (peektok()==END) {
            consumetok();
            continue;
        }
        add_child(node, statement());
        //printf("Statement\n");
    }
    //consumetok();
    return node;
}

pnode_t program() {
    pnode_t node;
    node = block();
    //printf("Program\n");
    return node;
}

void parse(parser_t *parser) {
	GList *ltoken = parser->tokens;

	parser->ast = program();
}

static void print_ast_rec(pnode_t node, int level) {
	uint16_t count=0;
	uint16_t capacity=0;
	pnode_t *children = node->children;
	if (children) {
    	uint16_t *header = get_children_header(node);
    	count = header[0];
    	capacity = header[1];
	}
	
	for (register int i=level; i>0; i--) printf("\t");
	char *type_str = parse_node_type(node->type);
	if (node->type==ID)
	    printf("(%s=%s)c%d/%d\n", type_str, (char*)node->data, count, capacity);
	else if (node->type==CONSTANT)
	    printf("(%s=%ld)c%d/%d\n", type_str, *(long*)node->data, count, capacity);
	else
	    printf("(%s)c%d/%d\n", type_str, count, capacity);
	free(type_str);
	
	if (children==NULL) return;
	
   	for (register uint16_t i = 0; i < count; i++)
   	    print_ast_rec(children[i], level+1);
}

void print_ast(parser_t *parser) {
	print_ast_rec(parser->ast, 0);
}

char *parse_node_type(node_type_t type) {
    switch(type) {
		case BLOCK: return strdup("BLOCK");
		case LET: return strdup("LET");
		case SET: return strdup("SET");
		case PRINT: return strdup("PRINT");
		case ADD: return strdup("ADD");
		case SUB: return strdup("SUB");
		case MULT: return strdup("MULT");
		case DIV: return strdup("DIV");
		case ID: return strdup("ID");
		case CONSTANT: return strdup("CONST");
		case NEG: return strdup("NEG");
		default: return strdup("UKNOWN");
	}
}