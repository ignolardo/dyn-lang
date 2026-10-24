#include "../chunk.h"
#include "../value.h"
#include "../bytecode.h"

int main() {
    Chunk chunk;
    init_chunk(&chunk);
    
    int constant = add_constant(&chunk, NUMBER_VAL(34.5));
    write_chunk(&chunk, LOAD_CONST);
    write_chunk(&chunk, constant);

    disassemble_chunk(&chunk, "test chunk");
    
    free_chunk(&chunk);
    return 0;
}