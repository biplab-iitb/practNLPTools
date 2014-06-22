#ifndef SENNA_SCORES2TREILLIS_H
#define SENNA_SCORES2TREILLIS_H

#include "SENNA_Treillis.h"

#define SEG_O 0
#define SEG_B 1
#define SEG_I 2
#define SEG_E 3
#define SEG_S 4

#define STATUS_SEG_NIL -1
#define STATUS_SEG_O 0
#define STATUS_SEG_B 1
#define STATUS_SEG_I 2
#define STATUS_SEG_E 3

void SENNA_Treillis_buildfromscore(SENNA_Treillis *treillis, float *score, float *start, float *trans, int N, int T);
void SENNA_Treillis_buildfromscorewithsegmentation(SENNA_Treillis *treillis, float *score, float *start, float *trans, int *segmentation, int N, int T);

#endif
