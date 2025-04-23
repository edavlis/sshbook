// ill refactor all of this later. this is just the framework for how it's gonna work.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <pwd.h>
#include <sys/stat.h>

#define INITIAL_CAPACITY 16

struct termios orig_termios;
typedef struct {
    char **items;
    int  count, cap;
} Menu;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\033[?25h");
}
void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    printf("\033[?25l");
}

void clearScreen() {
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
}

void draw_menu(Menu *m, int sel) {
    clearScreen();
    for (int i = 0; i < m->count; i++) {
        if (i == sel)   printf("> %s\n", m->items[i]);
        else            printf("  %s\n", m->items[i]);
    }
    printf("\n[a]dd entry [d]elete entry [Enter] connect\n");
    fflush(stdout);
}

//  menu shit
void menu_init(Menu *m) {
    m->count = 0; m->cap = INITIAL_CAPACITY;
    m->items = malloc(m->cap * sizeof *m->items);
}
void menu_add(Menu *m, char *line) {
    if (m->count >= m->cap)
        m->items = realloc(m->items, (m->cap*=2) * sizeof *m->items);
    m->items[m->count++] = line;
}

// config path, load & save,
char *get_config_path() {
    const char *home = getenv("HOME");
    if (!home) {
        struct passwd *pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    char *path = malloc(strlen(home) + 32);
    sprintf(path, "%s/.config/sshbook/book", home);

    char dir[2000];
    strcpy(dir, path);
    *strrchr(dir, '/') = 0;
    mkdir(dir, 0755);
    char *sub = strrchr(dir, '/');
    if (sub) { mkdir(dir, 0755); }

    return path;
}
void load_menu(Menu *m, const char *path) {
    FILE *f = fopen(path, "a+");
    if (!f) { perror("fopen"); exit(1); }
    fseek(f, 0, SEEK_SET);
    char *line = NULL; size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        menu_add(m, strdup(line));
    }
    free(line);
    fclose(f);
}
void save_menu(Menu *m, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) { perror("fopen"); exit(1); }
    for (int i = 0; i < m->count; i++)
        fprintf(f, "%s\n", m->items[i]);
    fclose(f);
}

// add & delete items
void add_item(Menu *m, const char *path) {
    disable_raw_mode();
    char buf[512];
    printf(" New line: ");
    if (fgets(buf, sizeof buf, stdin)) {
        buf[strcspn(buf, "\r\n")] = 0;
        menu_add(m, strdup(buf));
        save_menu(m, path);
    }
    enable_raw_mode();
}
void delete_item(Menu *m, const char *path, int *sel) {
    if (m->count == 0) return;
    free(m->items[*sel]);
    for (int i = *sel; i + 1 < m->count; i++)
        m->items[i] = m->items[i+1];
    m->count--;
    if (*sel >= m->count) *sel = m->count - 1;
    save_menu(m, path);
}

int main() {
    char *config = get_config_path();
    Menu menu;
    menu_init(&menu);
    load_menu(&menu, config);

    enable_raw_mode();

    int selected = 0;
    char c;
    while (1) {
        draw_menu(&menu, selected);

        if (read(STDIN_FILENO, &c, 1) != 1) continue;
        if (c == '\033') {
            char seq[2];
            read(STDIN_FILENO, &seq[0],1);
            read(STDIN_FILENO, &seq[1],1);
            if (seq[0]=='[') {
                if (seq[1]=='A')
                    selected = (selected - 1 + menu.count) % menu.count;
                else if (seq[1]=='B')
                    selected = (selected + 1) % menu.count;
            }
        }
        else if (c == 'a') {
            add_item(&menu, config);
            if (menu.count == 1) selected = 0;
        }
        else if (c == 'd') {
            delete_item(&menu, config, &selected);
        }
        else if (c == '\n' || c == '\r') {
            disable_raw_mode();
            printf("\nYou selected: %s\n", menu.items[selected]);

	    //system("ssh ",);
            break;
        }
    }

    // cleanup
    for (int i = 0; i < menu.count; i++)
        free(menu.items[i]);
    free(menu.items);
    free(config);
    return 0;
}

