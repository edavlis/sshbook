#include "../include/term.h"

void clearScreen() { write(STDOUT_FILENO, "\033[2J\033[H", 7); }

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
