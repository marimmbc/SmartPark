#include "ia.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int g_tf_ok = 0;

#ifdef __APPLE__
#include <dlfcn.h>
#include <tensorflow/c/c_api.h>

static void*       g_tf_handle = NULL;
static TF_Graph*   g_graph = NULL;
static TF_Session* g_session = NULL;
static TF_Status*  g_status = NULL;
static TF_Output   g_input_op;
static TF_Output   g_output_op;
static int         g_model_ready = 0;

static const char* resolve_env_or_default(const char* key, const char* fallback){
    const char* value = getenv(key);
    if (value && value[0] != '\0') return value;
    return fallback;
}

static void log_tf_error(const char* stage){
    if (!g_status) return;
    fprintf(stderr, "[IA][TensorFlow] %s: %s\n", stage, TF_Message(g_status));
}

static void tf_tensor_deallocator(void* data, size_t len, void* arg){
    (void)len;
    (void)arg;
    free(data);
}

static TF_Tensor* tensor_from_features(const float* features, int count){
    int64_t dims[2] = {1, count};
    size_t bytes = sizeof(float) * (size_t)count;
    float* buffer = malloc(bytes);
    if (!buffer) return NULL;
    memcpy(buffer, features, bytes);
    return TF_NewTensor(TF_FLOAT, dims, 2, buffer, bytes, tf_tensor_deallocator, NULL);
}

static int tf_predict(float p, int preferido, float espera_s, float reforco_extra, float* out_value){
    if (!g_model_ready || !g_session) return 0;

    float features[4];
    features[0] = p;
    features[1] = preferido ? 1.0f : 0.0f;
    features[2] = espera_s;
    features[3] = reforco_extra;

    TF_Tensor* input_tensor = tensor_from_features(features, 4);
    if (!input_tensor) return 0;

    TF_Tensor* output_tensor = NULL;
    TF_SessionRun(g_session,
                  NULL,
                  &g_input_op, &input_tensor, 1,
                  &g_output_op, &output_tensor, 1,
                  NULL, 0,
                  NULL,
                  g_status);

    TF_DeleteTensor(input_tensor);

    if (TF_GetCode(g_status) != TF_OK || !output_tensor){
        log_tf_error("TF_SessionRun");
        if (output_tensor) TF_DeleteTensor(output_tensor);
        return 0;
    }

    float* data = (float*)TF_TensorData(output_tensor);
    if (!data){
        TF_DeleteTensor(output_tensor);
        return 0;
    }

    *out_value = data[0];
    TF_DeleteTensor(output_tensor);
    return 1;
}
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
    const char* lib_path = resolve_env_or_default("SMARTPARK_TF_LIB", "/opt/homebrew/lib/libtensorflow.dylib");
    g_tf_handle = dlopen(lib_path, RTLD_NOW | RTLD_LOCAL);
    if (!g_tf_handle){
        fprintf(stderr, "[IA][TensorFlow] Nao foi possivel carregar %s: %s\n", lib_path, dlerror());
        return 0;
    }

    g_graph = TF_NewGraph();
    g_status = TF_NewStatus();
    TF_SessionOptions* options = TF_NewSessionOptions();

    const char* model_dir = resolve_env_or_default("SMARTPARK_TF_MODEL_DIR", "/Users/marianacorreia/parquinho_inteligente/model");
    const char* tags[] = {"serve"};

    g_session = TF_LoadSessionFromSavedModel(options, NULL, model_dir, tags, 1, g_graph, NULL, g_status);
    TF_DeleteSessionOptions(options);

    if (TF_GetCode(g_status) != TF_OK){
        log_tf_error("TF_LoadSessionFromSavedModel");
        ia_shutdown();
        return 0;
    }

    const char* input_name = resolve_env_or_default("SMARTPARK_TF_INPUT_OP", "serving_default_input");
    const char* output_name = resolve_env_or_default("SMARTPARK_TF_OUTPUT_OP", "StatefulPartitionedCall");

    TF_Operation* input_op = TF_GraphOperationByName(g_graph, input_name);
    TF_Operation* output_op = TF_GraphOperationByName(g_graph, output_name);

    if (!input_op || !output_op){
        fprintf(stderr, "[IA][TensorFlow] Operacoes nao encontradas (input: %s, output: %s)\n", input_name, output_name);
        ia_shutdown();
        return 0;
    }

    g_input_op.oper = input_op;
    g_input_op.index = 0;
    g_output_op.oper = output_op;
    g_output_op.index = 0;

    g_model_ready = 1;
    g_tf_ok = 1;
#endif
    return g_tf_ok;
}

void ia_shutdown(void){
#ifdef __APPLE__
    if (g_session && g_status){
        TF_CloseSession(g_session, g_status);
        if (TF_GetCode(g_status) != TF_OK) log_tf_error("TF_CloseSession");
        TF_DeleteSession(g_session, g_status);
        if (TF_GetCode(g_status) != TF_OK) log_tf_error("TF_DeleteSession");
    }
    g_session = NULL;

    if (g_graph){
        TF_DeleteGraph(g_graph);
        g_graph = NULL;
    }

    if (g_status){
        TF_DeleteStatus(g_status);
        g_status = NULL;
    }

    if (g_tf_handle){
        dlclose(g_tf_handle);
        g_tf_handle = NULL;
    }

    g_model_ready = 0;
#endif
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

#ifdef __APPLE__
    float tf_value = 0.0f;
    if (tf_predict(p, preferido, espera_s, reforco_extra, &tf_value)){
        if (isfinite(tf_value)){
            pred = clamp01(tf_value);
        }
    }
#endif

    float suavizado = clamp01((0.7f * p) + (0.3f * pred));
    return suavizado;
}
