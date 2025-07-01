#include "../include/menu.h"
#include "../include/term.h"

void menuMake(Menu *menu) {
  menu->count = 0;
  menu->capacity = CAPACITY;
  menu->items = malloc(menu->capacity * sizeof *menu->items);
}

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

void menuAdd(Menu *menu, char *line) {
  if (menu->count >= menu->capacity) {
    menu->items =
        realloc(menu->items, (menu->capacity *= 2) * sizeof *menu->items);
  }
  menu->items[menu->count++] = line;
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
    if (buffer[0] == '\r' || buffer[0] == '\n') {
      enableRawMode();
      return;
    }
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
  if (*selected >= menu->count && menu->count > 0) {
    *selected = menu->count - 1;
  }
  if (*selected < 0) {
    *selected = 0;
  }
  saveMenu(menu, path);
}