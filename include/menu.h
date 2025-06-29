
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#define CAPACITY 16 // no particular reason for this number. just a high number.

typedef struct {
  char **items;
  int count, capacity;
} Menu;

void deleteItem(Menu *menu, const char *path, int *selected);
void addItem(Menu *menu, const char *path);
void saveMenu(Menu *menu, const char *path);
void loadMenu(Menu *menu, const char *path);
void draw_menu(Menu *menu, int selected);
void menuMake(Menu *menu);