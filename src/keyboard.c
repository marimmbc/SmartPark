#include "keyboard.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios oldt, newt;
static int has_buf = 0;
static unsigned char buf_ch = 0;

void keyboard_init(void){
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int f = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, f | O_NONBLOCK);
    has_buf = 0;
}

void keyboard_shutdown(void){
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    int f = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, f & ~O_NONBLOCK);
    has_buf = 0;
}

int kbhit(void){
    if (has_buf) return 1;
    unsigned char c;
    int n = read(STDIN_FILENO, &c, 1);
    if (n == 1){ buf_ch = c; has_buf = 1; return 1; }
    return 0;
}

int getch_now(void){
    if (has_buf){ has_buf = 0; return (int)buf_ch; }
    unsigned char c;
    int n = read(STDIN_FILENO, &c, 1);
    return n == 1 ? (int)c : -1;
}
