#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../lexer.h"

int main(int argc, char **argv) {
	printf("Lexer testing...\n");

	//char buf[] = "VAR==,cwuqcioe\n";

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
	char c;

	printf("Cursor: \n");
	do {
		c = next(&lex);
	//	printf("%c", c);
	} while (c);

	printf("Buffer: ");
	print_buffer(&lex);

	printf("\nList: ");
	print_list(&lex);

	free_lexer(&lex);

	return 0;
}
