#ifndef RANKING_H
#define RANKING_H
#include "types.h"
int  ranking_load(Score* arr, int max);
int  ranking_save_append(const Score* s);
void quicksort_scores(Score* arr, int left, int right);
void ranking_print_top(const Score* arr, int n);
#endif
