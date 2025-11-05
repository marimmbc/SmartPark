#include "ia.h"
#ifdef __APPLE__
#include <dlfcn.h>
#endif
#include <math.h>

static int g_tf_ok = 0;

int ia_init(void){
    g_tf_ok = 0;
    return 1;
}

void ia_shutdown(void){
    g_tf_ok = 0;
}

int ia_enabled(void){
    return g_tf_ok;
}

float ia_predict_paciencia(float p, int preferido, float espera_s, float reforco_extra){
    float delta = 0.0f;
    if (preferido) delta += 0.10f; else delta -= 0.06f;
    delta += (-0.01f * espera_s);
    delta += reforco_extra;
    float r = p + delta;
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    return r;
}
