#include "ranking.h"
#include <stdio.h>
#include <string.h>

#define RANKING_FILE "scores.txt"

static int cmp_desc(const Score* a, const Score* b){
    if (a->score > b->score) return -1;
    if (a->score < b->score) return 1;
    return 0;
}

void quicksort_scores(Score* arr, int left, int right){
    int i=left, j=right;
    Score p = arr[(left+right)/2];
    while(i<=j){
        while(cmp_desc(&arr[i], &p)<0) i++;
        while(cmp_desc(&arr[j], &p)>0) j--;
        if(i<=j){ Score t=arr[i]; arr[i]=arr[j]; arr[j]=t; i++; j--; }
    }
    if(left<j) quicksort_scores(arr,left,j);
    if(i<right) quicksort_scores(arr,i,right);
}

int ranking_load(Score* arr, int max){
    FILE* f = fopen(RANKING_FILE, "r");
    if(!f) return 0;
    int n=0;
    char name[32];
    int sc, ps, ph;
    while(n<max && fscanf(f,"%31[^;];%d;%d;%d\n", name, &sc, &ps, &ph)==4){
        strncpy(arr[n].name,name,sizeof(arr[n].name));
        arr[n].name[sizeof(arr[n].name)-1]='\0';
        arr[n].score=sc; arr[n].penalties_soft=ps; arr[n].penalties_hard=ph;
        n++;
    }
    fclose(f);
    return n;
}

int ranking_save_append(const Score* s){
    FILE* f = fopen(RANKING_FILE, "a");
    if(!f) return 0;
    fprintf(f,"%s;%d;%d;%d\n", s->name, s->score, s->penalties_soft, s->penalties_hard);
    fclose(f);
    return 1;
}

void ranking_print_top(const Score* arr, int n){
    int m = n<10 ? n : 10;
    for(int i=0;i<m;i++){
        printf("%2d) %-12s  %5d  (%d/%d)\n", i+1, arr[i].name, arr[i].score, arr[i].penalties_soft, arr[i].penalties_hard);
    }
}
