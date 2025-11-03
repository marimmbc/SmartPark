#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "timer.h"

static clock_t g_start = 0;

void timer_start(void) {
    g_start = clock();
}

double timer_ms(void) {
    if (g_start == 0) return 0.0;
    clock_t now = clock();
    double s = (double)(now - g_start) / (double)CLOCKS_PER_SEC;
    return s * 1000.0;
}

void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}
