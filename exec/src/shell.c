#include <stdio.h>
#include <string.h>
#include <colors.h>
#include <types.h>
#include <dev/ps2/keyboard.h>

char buf[512];
int buf_idx = 0;
char c = 0;

char* cmd;
char* arg;

typedef void(*fn_ptr)(void);
typedef struct {
    char const *name;
    fn_ptr exec;
} __attribute__((packed)) cmd_info;

void cmd_echo();

const cmd_info cmds[] = {
    {"echo", cmd_echo},
    {"",0}
};


void shell_prompt() {
    printf(COL_WHITE "[" COL_YELLOW "root " COL_GREEN "hd0:/" COL_WHITE "]\n" COL_GREEN "# " COL_WHITE);
}

void shell_init() {
    shell_prompt();
}

void shell_handle() {
    int i = 0;
    while (cmds[i].exec > 0) {
        if (!strcmp(cmds[i].name, cmd)) {
            (*cmds[i].exec)();
            return;
        }
        i++;
    }
    printf(COL_RED "Bad command!" COL_WHITE "\n");
}

void shell_update() {
    shell_init();
    while (1) {
        c = keyboard_get();
        if (c != 0) {
            switch (c) {
                case '\n':
                    printf("\n");
                    cmd = strtok(buf, " ");
                    arg = strtok(NULL, " ");
                    shell_handle();
                    shell_prompt();
                    buf_idx = 0;
                    memset(buf, 0, 512);
                    break;
                case '\b':
                    if (buf_idx > 0) {
                        printf("\b \b");
                        buf_idx--;
                        buf[buf_idx] = 0;
                    }
                    break;
                default:
                    printf("%c", c);
                    buf[buf_idx] = c;
                    buf_idx++;
                    break;
            }
        }
    }
}

void cmd_echo() {
    printf("%s\n", arg);
}

int main() {
    shell_update();
    return 0;
}