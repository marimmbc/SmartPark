#ifndef TIMER_H
#define TIMER_H

// Inicia "cronômetro" interno
void timer_start(void);

// Ms desde o timer_start()
double timer_ms(void);

// Espera (bloqueia) por ms aproximados (para testes simples)
void sleep_ms(int ms);

#endif
