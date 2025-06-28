// comments r so that i can keep track. lot of things could be shortened, but i
// keep them like that for readablilty
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#define CAPACITY                                                               \
  16 // no particular reason for this number. just a high number.

struct termios original_termios;
typedef struct {
  char **items;
  int count, capacity;
} Menu;

void deleteItem(Menu *menu, const char *path, int *selected);
void addItem(Menu *menu, const char *path);
void saveMenu(Menu *menu, const char *path);
void loadMenu(Menu *menu, const char *path);
void disableRawMode();
void enableRawMode();
void clearScreen();
void draw_menu(Menu *menu, int selected);
void menuMake(Menu *menu);
char *getHomePath();

int main() {
  char *addresses = getHomePath();
  Menu menu;
  menuMake(&menu);
  loadMenu(&menu, addresses);
  const char *helpMessage =
      "press any key to exit\n"
      "\n"
      "\033[4mUSAGE\033[0m\n"
      "\t\n"
      "\tPress the up & down arrow keys or j & k to move up and down the list\n"
      "\t\n"
      "\tPress \033[1ma\033[0m to add an entry to the list. Examples:\n"
      "\t\t192.168.1.29\n"
      "\t\t2606:4700:abcd:1234:5678:90ab:cdef:1a2b\n"
      "\t\n"
      "\tAdd a description to the entry by adding a \"/\", and then your "
      "description. Examples:\n"
      "\t\t192.168.1.29 / homeserver\n"
      "\t\t2606:4700:abcd:1234:5678:90ab:cdef:1a2b / Personal VPS\n"
      "\t\n"
      "\tPress d to delete the selected entry from the list\n"
      "\t\n"
      "\tPress \033[1mh\033[0m to print this help message\n"
      "\t\n"
      "\tPress \033[1mq\033[0m to quit\n"
      "\t\n"
      "\tPress \033[1menter\033[0m to connect\n";

  enableRawMode();
  int selected = 0;
  char c;
  int isRunning = 1;
  while (isRunning) {
    draw_menu(&menu, selected);

    if (read(STDIN_FILENO, &c, 1) != 1) {
      continue;
    }

    char seq[2];
    switch (c) {
    case '\033':
      read(STDIN_FILENO, &seq[0], 1);
      read(STDIN_FILENO, &seq[1], 1);
      if (seq[0] == '[') {
        if (seq[1] == 'A') {
          selected = (selected - 1 + menu.count) % menu.count;
        } else if (seq[1] == 'B') {
          selected = (selected + 1) % menu.count;
        }
      }
      break;
    case 'j':
      if (selected + 1 == menu.count)
        break;
      ++selected;
      break;
    case 'k':
      if (selected == 0)
        break;
      --selected;
      break;
    case 'a':
      addItem(&menu, addresses);
      break;
    case 'd':
      deleteItem(&menu, addresses, &selected);
      break;
    case 'q':
      disableRawMode();
      free(addresses);
      for (int i = 0; i < menu.count; ++i) {
        free(menu.items[i]);
      }
      free(menu.items);
      isRunning = 0;
      break;
    case 'h':
      clearScreen();
      printf("%s", helpMessage);
    helpLoop:
      if (read(STDIN_FILENO, &c, 1) != 1) {
        goto helpLoop;
      }
    case '\n':
    case '\r':
      disableRawMode();
      char connected[512] = "ssh ";
      strcat(connected, menu.items[selected]);
      char *sshAddress = strstr(connected, "/");
      if (sshAddress)
        *sshAddress = '\0';
      system(connected);
      free(addresses);
      for (int i = 0; i < menu.count; ++i) {
        free(menu.items[i]);
      }
      free(menu.items);
      isRunning = 0;
      break;
    }
  }
  return EXIT_SUCCESS;
}

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
  system("reset");
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &original_termios);
  atexit(disableRawMode);
  struct termios raw = original_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  printf("\033[?25l");
}

void clearScreen() { write(STDOUT_FILENO, "\033[2J\033[H", 7); }

void draw_menu(Menu *menu, int selected) {
  clearScreen();

  printf("\n[a]dd entry\t [d]elete entry\t\t [Enter] Connect\t [q]uit\n\n");
  for (int i = 0; i < menu->count; i++) {
    if (i == selected) {
      printf("\033[1m-> %s\n\033[0m", menu->items[i]);
    } else {
      printf("%s\n", menu->items[i]);
    }
  }
  fflush(stdout);
}

// menu bs
void menuMake(Menu *menu) {
  menu->count = 0;
  menu->capacity = CAPACITY;
  menu->items = malloc(menu->capacity * sizeof *menu->items);
}

void menuAdd(Menu *menu, char *line) {
  if (menu->count >= menu->capacity) {
    menu->items =
        realloc(menu->items, (menu->capacity *= 2) * sizeof *menu->items);
  }
  menu->items[menu->count++] = line;
}

// expand home path, addresses file, etc.
char *getHomePath() {
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
  }

  char *path = malloc(strlen(home) + 32);
  sprintf(path, "%s/.config/sshbook/book", home);

  char dir[2048];
  strcpy(dir, path);
  *strrchr(dir, '/') = 0;
  mkdir(dir, 0755);

  char *sub = strrchr(dir, '/');
  if (sub) {
    mkdir(dir, 0755);
  }
  return path;
}

char *getDescPath() {
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    home = pw->pw_dir;
  }

  char *path = malloc(strlen(home) + 32);
  sprintf(path, "%s/.config/sshbook/desc", home);

  char dir[2048];
  strcpy(dir, path);
  *strrchr(dir, '/') = 0;
  mkdir(dir, 0755);

  char *sub = strrchr(dir, '/');
  if (sub) {
    mkdir(dir, 0755);
  }
  return path;
}

void loadMenu(Menu *menu, const char *path) {
  FILE *f = fopen(path, "a+");
  if (!f) {
    perror("fopen");
    exit(1);
  }
  fseek(f, 0, SEEK_SET);
  char *line = NULL;
  size_t len = 0;
  while (getline(&line, &len, f) != -1) {
    line[strcspn(line, "\r\n")] = 0;
    menuAdd(menu, strdup(line));
  }
  free(line);
  fclose(f);
}

void saveMenu(Menu *menu, const char *path) {
  FILE *f = fopen(path, "w");
  if (!f) {
    perror("fopen");
    exit(1);
  }
  for (int i = 0; i < menu->count; i++) {
    fprintf(f, "%s\n", menu->items[i]);
  }
  fclose(f);
}

void addItem(Menu *menu, const char *path) {
  disableRawMode();
  char buffer[512];
  printf("New entry: ");
  if (fgets(buffer, sizeof buffer, stdin)) {
    buffer[strcspn(buffer, "\r\n")] = 0;
    menuAdd(menu, strdup(buffer));
    saveMenu(menu, path);
  }
  enableRawMode();
}

void deleteItem(Menu *menu, const char *path, int *selected) {
  if (menu->count == 0) {
    return;
  }
  free(menu->items[*selected]);
  for (int i = *selected; i + 1 < menu->count; i++) {
    menu->items[i] = menu->items[i + 1];
  }
  menu->count--;
  if (*selected >= menu->count) {
    *selected = menu->count - 1;
  }
  saveMenu(menu, path);
}
