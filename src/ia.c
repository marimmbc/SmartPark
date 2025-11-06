#include "ia.h"
#include <math.h>

static int g_tf_ok = 0;

#ifdef __APPLE__
#include <dlfcn.h>
#include <tensorflow/c/c_api.h>
#endif

int ia_init(void){
    g_tf_ok = 0;
#ifdef __APPLE__
    void* handle = dlopen("/opt/homebrew/lib/libtensorflow.dylib", RTLD_LAZY);
    if (handle) { g_tf_ok = 1; dlclose(handle); }
#endif
    return g_tf_ok;
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

    float pred = p + delta;
    if (pred < 0.0f) pred = 0.0f;
    if (pred > 1.0f) pred = 1.0f;

    float suavizado = (0.7f * p) + (0.3f * pred);
    if (suavizado < 0.0f) suavizado = 0.0f;
    if (suavizado > 1.0f) suavizado = 1.0f;

    return suavizado;
}
