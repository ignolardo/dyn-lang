#include "../vm.h"
#include "../compiler.h"
#include "../parser.h"
#include "../lexer.h"
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	printf("VM testing...\n");

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

	printf("\nTokens: ");
	print_list(&lex);

	parser_t parser = new_parser(lex.list);
	parse(&parser);

	printf("\nAST:\n");
	print_ast(&parser);

	printf("\nBytecode:\n");
	Chunk chunk = compile(parser.ast);
	disassemble_chunk(&chunk, "main chunk");

	printf("\nVirtual Machine:\n");
	init_VM();
	InterpretResult result = interpret(&chunk);
	printf("\nExecution result: %d\n", result);
	
	free_chunk(&chunk);
	printf("Chunk freed\n");
	free_lexer(&lex);
	printf("Lexer freed\n");
	free_parser(&parser);
	printf("Parser freed\n");
	
	return 0;
}
