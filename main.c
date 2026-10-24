#include "chunk.h"
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc<2) return 1;
   
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }
    int fd = fileno(fp);

    struct stat sb;
    if (fstat(fd, &sb) == -1) return 1;

    size_t out_size = sb.st_size;
    void *mem = mmap(NULL, out_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mem == MAP_FAILED) return 1;

    fclose(fp);

	lexer_t lex = new_lexer(mem);
	tokenize(&lex);
   
	parser_t parser = new_parser(lex.list);
	parse(&parser);
   
	Chunk chunk = compile(parser.ast);
	//disassemble_chunk(&chunk, "main chunk");
   
	init_VM();
	InterpretResult result = interpret(&chunk);
	//printf("\nExecution result: %d\n", result);
	
	free_chunk(&chunk);
	free_lexer(&lex);
	free_parser(&parser);
	
	return 0;
}
