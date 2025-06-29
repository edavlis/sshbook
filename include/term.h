#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

extern struct termios original_termios;

void disableRawMode(void);
void enableRawMode(void);
void clearScreen(void);