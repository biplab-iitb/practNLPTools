#ifndef SENNA_PSG_H
#define SENNA_PSG_H

#include "SENNA_Treillis.h"

typedef struct SENNA_PSG_
{
    /* sizes */
    int window_size;
    int ll_word_size;
    int ll_word_max_idx;
    int ll_caps_size;
    int ll_caps_max_idx;
    int ll_posl_size;
    int ll_posl_max_idx;
    int ll_psgl_size;
    int ll_psgl_max_idx;

    int input_state_size;
    int l1_state_size;
    int l2_state_size;
    int l3_state_size;
    int l4_state_size;

    /* weights */
    float *ll_word_weight;
    float *ll_caps_weight;
    float *ll_posl_weight;
    float *ll_psgl_weight;
    float *l1_weight;
    float *l1_bias;
    float *l2_bias;
    float *l3_weight;
    float *l3_bias;
    float *l4_weight;
    float *l4_bias;
    float *viterbi_score_init;
    float *viterbi_score_trans;

    /* states */
    float *input_state;
    float *l1_state;
    float *l2_state;
    float *l3_state;
    float *l4_state;
    int *labels;
    int max_labels_size;

    /* treillis */
    SENNA_Treillis *treillis;

    /* padding indices */
    int ll_word_padding_idx;
    int ll_caps_padding_idx;
    int ll_posl_padding_idx;
    int ll_psgl_padding_idx;

} SENNA_PSG;

SENNA_PSG* SENNA_PSG_new();
void SENNA_PSG_forward(SENNA_PSG *psg, const int *sentence_words, const int *sentence_caps, const int *sentence_posl, int sentence_size, int **labels_, int *n_level_);
void SENNA_PSG_free(SENNA_PSG *psg);

#endif
