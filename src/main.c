#include "../include/menu.h"
#include "../include/paths.h"
#include "../include/term.h"

struct termios original_termios;

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
  char choice;
  int isRunning = 1;
  while (isRunning) {
    draw_menu(&menu, selected);

    if (read(STDIN_FILENO, &choice, 1) != 1) {
      continue;
    }

    char seq[2];
    switch (choice) {
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
      if (read(STDIN_FILENO, &choice, 1) != 1) {
        goto helpLoop;
      }
      break;
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
