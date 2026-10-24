#ifndef LEXER_H_
#define LEXER_H_

#include <glib.h>

#define TOK(token) ((lexeme_t*)(token->data))

typedef enum LexemeType {
	IDENT,
	KEYWORD,
	LETK,
	SETK,
	ADDK,
	PRINTK,
	ASSIGN,
	SEPAR,
	END,
	ENDPROG,
	LITERAL,
	TIMES,
	SLASH,
	PLUS,
	MINUS,
	OPAR,
	CPAR,
	BREAK
} lexeme_type_t;

typedef struct Lexeme {
	GString *buffer;
	lexeme_type_t type;
} lexeme_t;

typedef struct Lexer {
	char *cursor;
	GList *list;
	lexeme_t *last;
} lexer_t;


lexer_t new_lexer(char *cursor);
void free_lexer(lexer_t *lexer);
void tokenize(lexer_t *lexer);
char next(lexer_t *lexer);
void analyze(lexer_t *lexer, char c);
lexeme_t *push_lexeme(lexer_t *lexer);
void print_list(lexer_t *lexer);
void print_buffer(lexer_t *lexer);

#endif
