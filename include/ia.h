#ifndef IA_H
#define IA_H
float ia_init(const char* model_dir, const char* input_op, const char* output_op);
void  ia_shutdown(void);
float ia_predict_paciencia(float preferido, float espera_s, float pac_atual);
#endif
