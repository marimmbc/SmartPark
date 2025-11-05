#ifndef IA_H
#define IA_H
#include <stdint.h>

int  ia_init(void);
void ia_shutdown(void);
int  ia_enabled(void);
float ia_predict_paciencia(float paciencia_atual, float espera_s, float reforco_extra);

#endif
