/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
     if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        die("tcsetattr");
     }
}

void enableRawMode() {
     // orig terminal attributes stored in orig_temrios, a global var
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode); // Upon exit, disable terminal raw mode

    struct termios raw = orig_termios;  // copy of terminal is made so that we can make changes to it
    raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_oflag |= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);  // c_lflag => miscellaneous flags. Turning off the passed flags.
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    // The important flags we're turning off are as follows
    // Echo: Shows all user typing in command line
    // ICANNON, Canonical mode: Read line by line instead of byte by byte
    // ISIG: Uses Ctrl-C and Ctrl-Z to terminate/suspend the program, respectively
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/

int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)) {  // If it is a control character
            printf("%d\r\n", c);  // Print regularly
        } else {
            printf("%d ('%c')\r\n", c, c); // Print char's ASCII value (formated as decimal), then the char itself
        }
        if (c == 'q') break;
    }
    return 0;
}
