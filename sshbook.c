#include <stdio.h>
#include <unistd.h>
#include <termio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define CLEARSCREEN write(STDOUT_FILENO, "\x1b[2J\x1b[H", 7)

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);  // Turn off echo, canonical mode, Ctrl-C/Z

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main(int argc, char* argv[]) {
			
	//	system("mkdir -p \"$HOME/.config/sshbook\"");
	//							system("touch $HOME/.config/sshbook/list"); // do this after checking if file is empty
		enable_raw_mode();
		
		int x = system("awk \'NF {exit 1}\' $HOME/.config/sshbook/list && echo 0 || echo 1"); // 0 is empty 1 is not.
		int y = system("echo 1");
		printf("%d", y);
		// 1 1 0
}
