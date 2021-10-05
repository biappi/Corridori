extern int outpsp;
extern void far * csip;

extern int load_program(char near *cosa);
extern void start_program();

void main(void) {
    printf("ecci`\n");

    if (load_program("c:\\tr\\manager.exe")) {
        printf("error\n");
        return;
    }

    printf("new psp: %4x\n", outpsp);
    printf("cs:ip:   %8lx\n", csip);

    start_program();
}

