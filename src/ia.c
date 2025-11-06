#include "ia.h"
#include <math.h>

static int g_tf_ok = 0;

#ifdef __APPLE__
#include <dlfcn.h>
#include <tensorflow/c/c_api.h>
#endif

static float clamp01(float v){
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

static float penalidade_por_espera(float espera_s){
    float penalidade = 0.0f;

    float faixa1 = espera_s;
    if (faixa1 > 10.0f) faixa1 = 10.0f;
    penalidade -= 0.003f * faixa1;

    if (espera_s > 10.0f){
        float faixa2 = espera_s - 10.0f;
        if (faixa2 > 15.0f) faixa2 = 15.0f;
        penalidade -= 0.0015f * faixa2;
    }

    if (espera_s > 25.0f){
        float faixa3 = espera_s - 25.0f;
        penalidade -= 0.00075f * faixa3;
    }

    return penalidade;
}

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
    float delta = penalidade_por_espera(espera_s);
    if (preferido) delta += 0.10f;
    else delta -= 0.06f;

    delta += reforco_extra;

    float pred = clamp01(p + delta);
    float suavizado = clamp01((0.7f * p) + (0.3f * pred));
    return suavizado;
}
