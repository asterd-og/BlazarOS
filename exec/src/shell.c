int main() {
    int vec = 1;
    char* str = "Hello World from ELF!\n";
    asm volatile (
        "int $0x80\n\t"
        : "+a" (vec), "+b" (str) // I/O operands
        : 
        : "memory" // probably
    );
    return 0;
}