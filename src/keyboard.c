#include "keyboard.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios oldt, newt;

void keyboard_init(void){
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void keyboard_shutdown(void){
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}

int kbhit(void){
    unsigned char c;
    int n = read(STDIN_FILENO, &c, 1);
    if (n == 1){ ungetc(c, stdin); return 1; }
    return 0;
}

int getch_now(void){
    unsigned char c;
    int n = read(STDIN_FILENO, &c, 1);
    return n == 1 ? (int)c : -1;
}
