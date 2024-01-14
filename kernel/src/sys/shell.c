#include <sys/shell.h>

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

void shell_ls();
void shell_cd();
void shell_cat();
void shell_touch();

const cmd_info cmds[] = {
    {"ls", shell_ls},
    {"cd", shell_cd},
    {"cat", shell_cat},
    {"touch", shell_touch},
    {"",0}
};


void shell_prompt() {
    printf(COL_WHITE "[" COL_YELLOW "root " COL_GREEN "%s" COL_WHITE "]\n" COL_GREEN "# " COL_WHITE, vfs->path);
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

void shell_ls() {
    vfs_list_print();
}

void shell_cd() {
    vfs_cd(arg);
}

void shell_cat() {
    u8* buf = vfs_read(arg);
    if (buf == NULL) {
        printf(COL_RED "Error: Couldn't find file '%s'." COL_WHITE "\n", arg);
        return;
    }
    printf("%s", buf);
    kfree(buf);
}

void shell_touch() {
    u8* null_buf = kmalloc(1);
    vfs_write(arg, null_buf, 1);
    kfree(null_buf);
}