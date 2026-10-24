OUTPUT = lang

BIN = bin

CFLAGS += \
	-I/usr/include/glib-2.0 \
	-I/usr/lib/x86_64-linux-gnu/glib-2.0/include

LDFLAGS += \
	-lglib-2.0

SRC = main.c vm.c compiler.c chunk.c value.c name.c parser.c lexer.c utils.c memory.c

SRCFILES := $(patsubst ./%,%,$(shell find -maxdepth 1 -type f 2>/dev/null | LC_ALL=C sort))
CFILES := $(filter %.c,$(SRCFILES))
OBJ := $(addprefix obj/,$(CFILES:.c=.c.o))
HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d))

.PHONY=all
all: $(OBJ)
	gcc $(OBJ) $(LDFLAGS) -g -o $(OUTPUT)

clean:
	rm -r $(OBJ)

obj/%.c.o: %.c
	gcc $(CFLAGS) $< -c -g -o $@

test_lexer: tests/test_lexer.c lexer.c lexer.h utils.c
	gcc $(CFLAGS) tests/test_lexer.c lexer.c utils.c $(LDFLAGS) -g -o $(BIN)/test_lexer

test_parser: tests/test_parser.c parser.c parser.h lexer.c lexer.h utils.c
	gcc $(CFLAGS) tests/test_parser.c parser.c lexer.c utils.c $(LDFLAGS) -g -o $(BIN)/test_parser

test_compiler: tests/test_compiler.c compiler.c compiler.h chunk.c chunk.h value.c value.h name.c name.h parser.c parser.h lexer.c lexer.h utils.c memory.c memory.h
	gcc $(CFLAGS) tests/test_compiler.c compiler.c chunk.c value.c name.c parser.c lexer.c utils.c memory.c $(LDFLAGS) -g -o $(BIN)/test_compiler

test_chunk: tests/test_chunk.c chunk.c chunk.h value.c value.h name.c name.h bytecode.h memory.c memory.h
	gcc $(CFLAGS) tests/test_chunk.c chunk.c value.c name.c memory.c $(LDFLAGS) -g -o $(BIN)/test_chunk

test_vm: tests/test_vm.c vm.c vm.h compiler.c compiler.h chunk.c chunk.h value.c value.h name.c name.h parser.c parser.h lexer.c lexer.h utils.c memory.c memory.h
	gcc $(CFLAGS) tests/test_vm.c vm.c compiler.c chunk.c value.c name.c parser.c lexer.c utils.c memory.c $(LDFLAGS) -g -o $(BIN)/test_vm