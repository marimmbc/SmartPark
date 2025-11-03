#ifndef KEYBOARD_H
#define KEYBOARD_H
int kbhit(void);     // retorna 1 se tem tecla pendente, 0 se não
int getch_now(void); // lê 1 char sem bloquear; -1 se não tem
void keyboard_init(void);
void keyboard_shutdown(void);
#endif
