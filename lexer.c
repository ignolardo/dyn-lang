#include "lexer.h"
#include "utils.h"
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>

lexer_t new_lexer(char *cursor) {
	lexeme_t *lexeme = malloc(sizeof(lexeme_t));
	lexeme->buffer = g_string_new(NULL);
	lexeme->type = BREAK;
	return (lexer_t){
		.cursor=cursor,
		.list=NULL,
		.last=lexeme
	};
}

void free_lexeme(lexeme_t *lexeme) {
    g_string_free(lexeme->buffer, TRUE);
    free(lexeme);
}

void free_lexer(lexer_t *lexer) {
    free_lexeme(lexer->last);
    g_list_free_full(lexer->list, (GDestroyNotify)free_lexeme);
}

void tokenize(lexer_t *lexer) {
    char c;
    do {
		c = next(lexer);
	} while (c);
    // We need to push an END token at the end of the list, parsing doesn't work otherwise (something happen with last instruction and ID nodes)
    g_string_free(lexer->last->buffer, TRUE);
    lexer->last->buffer = g_string_new("\n");
    lexer->last->type = END;
    push_lexeme(lexer);
}

char next(lexer_t *lexer) {
	char c = *(lexer->cursor);
	analyze(lexer, c);
	if (c) {
		lexer->cursor++;
	}
	return c;
}

int is_keyword(char *str) {
	int r = 0;
	CMP_KEY(r,str,"let");
	CMP_KEY(r,str,"set");
	CMP_KEY(r,str,"add");
	CMP_KEY(r,str,"print");
	return r;
}

void analyze(lexer_t *lexer, char c) {
	lexeme_t *lexeme = lexer->last;
	if ((c >= 'A' && c<= 'Z') || (c >= 'a' && c<= 'z')) {
		switch (lexeme->type) {
			case LETK:
			case SETK:
			case ADDK:
			case PRINTK:
			case IDENT:
			case KEYWORD:
			case BREAK:
				lexeme->buffer = g_string_append_c(lexeme->buffer, c);
				if      (cmpstr(lexeme->buffer, "let"))     lexeme->type = LETK;
				else if (cmpstr(lexeme->buffer, "set"))     lexeme->type = SETK;
				else if (cmpstr(lexeme->buffer, "add"))     lexeme->type = ADDK;
				else if (cmpstr(lexeme->buffer, "print"))   lexeme->type = PRINTK;
				else    lexeme->type = IDENT;
				break;
			default:
				lexeme = push_lexeme(lexer);
				lexeme->buffer = g_string_append_c(lexeme->buffer, c);
				lexeme->type = IDENT;
				break;
		}
	} else if (c >= '0' && c <= '9') {
		switch (lexeme->type) {
			case LITERAL:
			case BREAK:
				lexeme->buffer = g_string_append_c(lexeme->buffer, c);
				lexeme->type = LITERAL;
				break;
			default:
				lexeme = push_lexeme(lexer);
				lexeme->buffer = g_string_append_c(lexeme->buffer, c);
				lexeme->type = LITERAL;
				break;
		}
	} else if (c == '=') {
		switch (lexeme->type) {
			case BREAK:
				lexeme->buffer = g_string_append_c(lexeme->buffer, c);
				lexeme->type = ASSIGN;
				break;
			default:
				lexeme = push_lexeme(lexer);
				lexeme->buffer = g_string_append_c(lexeme->buffer, c);
				lexeme->type = ASSIGN;
				break;
		}
	} else if (c == '+') {
        lexeme = push_lexeme(lexer);
        lexeme->buffer = g_string_append_c(lexeme->buffer, c);
        lexeme->type = PLUS;
	} else if (c == '-') {
        lexeme = push_lexeme(lexer);
        lexeme->buffer = g_string_append_c(lexeme->buffer, c);
        lexeme->type = MINUS;
	} else if (c == '*') {
        lexeme = push_lexeme(lexer);
        lexeme->buffer = g_string_append_c(lexeme->buffer, c);
        lexeme->type = TIMES;
	} else if (c == '/') {
        lexeme = push_lexeme(lexer);
        lexeme->buffer = g_string_append_c(lexeme->buffer, c);
        lexeme->type = SLASH;
	} else if (c == ',') {
		lexeme = push_lexeme(lexer);
		lexeme->buffer = g_string_append_c(lexeme->buffer, c);
		lexeme->type = SEPAR;
	} else if (c == '\n') {
	    lexeme = push_lexeme(lexer);
		lexeme->buffer = g_string_append_c(lexeme->buffer, c);
		lexeme->type = END;
	} else if (c == '(') {
        lexeme = push_lexeme(lexer);
        lexeme->buffer = g_string_append_c(lexeme->buffer, c);
        lexeme->type = OPAR;
	} else if (c == ')') {
        lexeme = push_lexeme(lexer);
        lexeme->buffer = g_string_append_c(lexeme->buffer, c);
        lexeme->type = CPAR;
	} else if (c == '.') {
	    lexeme = push_lexeme(lexer);
		lexeme->buffer = g_string_append_c(lexeme->buffer, c);
		lexeme->type = ENDPROG;
	} else if (c == 0 || c == EOF || c == ' ' || c == '\t') {
	    lexeme = push_lexeme(lexer);
	} else {
		printf("Uknown character '%c'\n", c);
		exit(1);
	}
}

lexeme_t *push_lexeme(lexer_t *lexer) {
	if (lexer->last->type == BREAK) return lexer->last;
	lexer->list = g_list_append(lexer->list, lexer->last);
	lexeme_t *lexeme = malloc(sizeof(lexeme_t));
	lexeme->buffer = g_string_new(NULL);
	lexeme->type = BREAK;
	lexer->last = lexeme;
	return lexeme;
}

char *parse_lexeme_type(lexeme_type_t type) {
	switch(type) {
		case IDENT: return strdup("IDENT");
		case KEYWORD: return strdup("KEYWORD");
		case LETK: return strdup("LETK");
		case SETK: return strdup("SETK");
		case ADDK: return strdup("ADDK");
		case PRINTK: return strdup("PRINTK");
		case ASSIGN: return strdup("ASSIGN");
		case BREAK: return strdup("BREAK");
		case PLUS: return strdup("PLUS");
		case MINUS: return strdup("MINUS");
		case TIMES: return strdup("TIMES");
		case SLASH: return strdup("SLASH");
		case SEPAR: return strdup("SEPAR");
		case OPAR: return strdup("OPAR");
		case CPAR: return strdup("CPAR");
		case END: return strdup("END");
		case ENDPROG: return strdup("ENDPROG");
		case LITERAL: return strdup("LITERAL");
		default: return strdup("UKNOWN");
	}
}

void print_lexeme(void* data, void* userData) {
	char *type = parse_lexeme_type(((lexeme_t*)data)->type);
	if (cmpstr(((lexeme_t*)data)->buffer, "\n"))
	    g_print("[\"\\n\",%s]  ", type);
	else
        g_print("[\"%s\",%s]  ", ((lexeme_t*)data)->buffer->str, type);
	free(type);
}

void print_list(lexer_t *lexer) {
	g_list_foreach(lexer->list, print_lexeme, NULL);
	g_print("\n");
}

void print_buffer(lexer_t *lexer) {
	g_print("%s", lexer->last->buffer->str);
}
