#include "ia.h"
#include <tensorflow/c/c_api.h>
#include <string.h>
#include <stdlib.h>

static TF_Graph* g = NULL;
static TF_Session* sess = NULL;
static TF_Status* st = NULL;
static TF_Operation* in_op = NULL;
static TF_Operation* out_op = NULL;

static TF_Tensor* make_tensor_1x3(float a, float b, float c){
    int64_t dims[2] = {1,3};
    size_t bytes = sizeof(float)*3;
    TF_Tensor* t = TF_AllocateTensor(TF_FLOAT, dims, 2, bytes);
    float* data = (float*)TF_TensorData(t);
    data[0]=a; data[1]=b; data[2]=c;
    return t;
}

float ia_init(const char* model_dir, const char* input_op_name, const char* output_op_name){
    st = TF_NewStatus();
    g  = TF_NewGraph();
    TF_SessionOptions* opts = TF_NewSessionOptions();
    TF_Buffer* run_opts = NULL;

    TF_Buffer* metagraph = TF_NewBuffer();
    const char* tags[] = {"serve"};
    sess = TF_LoadSessionFromSavedModel(opts, run_opts, model_dir, tags, 1, g, metagraph, st);
    TF_DeleteBuffer(metagraph);
    TF_DeleteSessionOptions(opts);
    if (TF_GetCode(st) != TF_OK) return 0.0f;

    in_op  = TF_GraphOperationByName(g, input_op_name);
    out_op = TF_GraphOperationByName(g, output_op_name);
    if (!in_op || !out_op) return 0.0f;

    return 1.0f;
}

void ia_shutdown(void){
    if (sess){ TF_CloseSession(sess, st); TF_DeleteSession(sess, st); sess=NULL; }
    if (g){ TF_DeleteGraph(g); g=NULL; }
    if (st){ TF_DeleteStatus(st); st=NULL; }
    in_op=NULL; out_op=NULL;
}

float ia_predict_paciencia(float preferido, float espera_s, float pac_atual){
    if (!sess || !in_op || !out_op){
        float a = preferido > 0.5f ? 0.10f : -0.06f;
        float b = -0.01f * espera_s;
        float p = pac_atual + a + b;
        if (p<0.0f) p=0.0f; if (p>1.0f) p=1.0f;
        return p;
    }

    TF_Tensor* input = make_tensor_1x3(preferido, espera_s, pac_atual);
    TF_Output inputs[1] = { { in_op, 0 } };
    TF_Tensor* input_vals[1] = { input };

    TF_Output outputs[1] = { { out_op, 0 } };
    TF_Tensor* output_vals[1] = { NULL };

    TF_SessionRun(sess,
                  NULL,
                  inputs, input_vals, 1,
                  outputs, output_vals, 1,
                  NULL, 0, NULL, st);

    TF_DeleteTensor(input);

    if (TF_GetCode(st) != TF_OK){
        if (output_vals[0]) TF_DeleteTensor(output_vals[0]);
        float a = preferido > 0.5f ? 0.10f : -0.06f;
        float b = -0.01f * espera_s;
        float p = pac_atual + a + b;
        if (p<0.0f) p=0.0f; if (p>1.0f) p=1.0f;
        return p;
    }

    float* outdata = (float*)TF_TensorData(output_vals[0]);
    float pred = outdata ? outdata[0] : pac_atual;
    TF_DeleteTensor(output_vals[0]);

    if (pred<0.0f) pred=0.0f; if (pred>1.0f) pred=1.0f;
    return pred;
}
