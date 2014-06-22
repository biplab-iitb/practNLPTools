#include "SENNA_CHK.h"
#include "SENNA_utils.h"
#include "SENNA_nn.h"

int* SENNA_CHK_forward(SENNA_CHK *chk, const int *sentence_words, const int *sentence_caps, const int *sentence_posl, int sentence_size)
{
  int idx;

  chk->input_state = SENNA_realloc(chk->input_state, sizeof(float), (sentence_size+chk->window_size-1)*(chk->ll_word_size+chk->ll_caps_size+chk->ll_posl_size));
  chk->output_state = SENNA_realloc(chk->output_state, sizeof(float), sentence_size*chk->output_state_size);
  
  SENNA_nn_lookup(chk->input_state,                                     chk->ll_word_size+chk->ll_caps_size+chk->ll_posl_size, chk->ll_word_weight, chk->ll_word_size, chk->ll_word_max_idx, sentence_words, sentence_size, chk->ll_word_padding_idx, (chk->window_size-1)/2);
  SENNA_nn_lookup(chk->input_state+chk->ll_word_size,                   chk->ll_word_size+chk->ll_caps_size+chk->ll_posl_size, chk->ll_caps_weight, chk->ll_caps_size, chk->ll_caps_max_idx, sentence_caps,  sentence_size, chk->ll_caps_padding_idx, (chk->window_size-1)/2);
  SENNA_nn_lookup(chk->input_state+chk->ll_word_size+chk->ll_caps_size, chk->ll_word_size+chk->ll_caps_size+chk->ll_posl_size, chk->ll_posl_weight, chk->ll_posl_size, chk->ll_posl_max_idx, sentence_posl,  sentence_size, chk->ll_posl_padding_idx, (chk->window_size-1)/2);

  for(idx = 0; idx < sentence_size; idx++)
  {
    SENNA_nn_linear(chk->hidden_state, chk->hidden_state_size, chk->l1_weight, chk->l1_bias, chk->input_state+idx*(chk->ll_word_size+chk->ll_caps_size+chk->ll_posl_size), chk->window_size*(chk->ll_word_size+chk->ll_caps_size+chk->ll_posl_size));
    SENNA_nn_hardtanh(chk->hidden_state, chk->hidden_state, chk->hidden_state_size);
    SENNA_nn_linear(chk->output_state+idx*chk->output_state_size, chk->output_state_size, chk->l2_weight, chk->l2_bias, chk->hidden_state, chk->hidden_state_size);
  }

  chk->labels = SENNA_realloc(chk->labels, sizeof(int), sentence_size);
  SENNA_nn_viterbi(chk->labels, chk->viterbi_score_init, chk->viterbi_score_trans, chk->output_state, chk->output_state_size, sentence_size);

  return chk->labels;
}

SENNA_CHK* SENNA_CHK_new(const char *path, const char *subpath)
{
  SENNA_CHK *chk = SENNA_malloc(sizeof(SENNA_CHK), 1);
  FILE *f;
  float dummy;

  memset(chk, 0, sizeof(SENNA_CHK));

  f = SENNA_fopen(path, subpath, "rb");

  SENNA_fread(&chk->window_size, sizeof(int), 1, f);
  SENNA_fread_tensor_2d(&chk->ll_word_weight, &chk->ll_word_size, &chk->ll_word_max_idx, f);
  SENNA_fread_tensor_2d(&chk->ll_caps_weight, &chk->ll_caps_size, &chk->ll_caps_max_idx, f);
  SENNA_fread_tensor_2d(&chk->ll_posl_weight, &chk->ll_posl_size, &chk->ll_posl_max_idx, f);
  SENNA_fread_tensor_2d(&chk->l1_weight, &chk->input_state_size, &chk->hidden_state_size, f);
  SENNA_fread_tensor_1d(&chk->l1_bias, &chk->hidden_state_size, f);
  SENNA_fread_tensor_2d(&chk->l2_weight, &chk->hidden_state_size, &chk->output_state_size, f);
  SENNA_fread_tensor_1d(&chk->l2_bias, &chk->output_state_size, f);
  SENNA_fread_tensor_1d(&chk->viterbi_score_init, &chk->output_state_size, f);
  SENNA_fread_tensor_2d(&chk->viterbi_score_trans, &chk->output_state_size, &chk->output_state_size, f);

  SENNA_fread(&chk->ll_word_padding_idx, sizeof(int), 1, f);
  SENNA_fread(&chk->ll_caps_padding_idx, sizeof(int), 1, f);
  SENNA_fread(&chk->ll_posl_padding_idx, sizeof(int), 1, f);

  SENNA_fread(&dummy, sizeof(float), 1, f);
  SENNA_fclose(f);

  if((int)dummy != 777)
    SENNA_error("chk: data corrupted (or not IEEE floating computer)");

  chk->input_state = NULL;
  chk->hidden_state = SENNA_malloc(sizeof(float), chk->hidden_state_size);
  chk->output_state = NULL;
  chk->labels = NULL;

  /* some info if you want verbose */
  SENNA_message("chk: window size: %d", chk->window_size);
  SENNA_message("chk: vector size in word lookup table: %d", chk->ll_word_size);
  SENNA_message("chk: word lookup table size: %d", chk->ll_word_max_idx);
  SENNA_message("chk: vector size in caps lookup table: %d", chk->ll_caps_size);
  SENNA_message("chk: caps lookup table size: %d", chk->ll_caps_max_idx);
  SENNA_message("chk: vector size in pos lookup table: %d", chk->ll_posl_size);
  SENNA_message("chk: pos lookup table size: %d", chk->ll_posl_max_idx);
  SENNA_message("chk: number of hidden units: %d", chk->hidden_state_size);
  SENNA_message("chk: number of classes: %d", chk->output_state_size);

  return chk;
}

void SENNA_CHK_free(SENNA_CHK *chk)
{
  SENNA_free(chk->ll_word_weight);
  SENNA_free(chk->ll_caps_weight);
  SENNA_free(chk->ll_posl_weight);
  SENNA_free(chk->l1_weight);
  SENNA_free(chk->l1_bias);  
  SENNA_free(chk->l2_weight);
  SENNA_free(chk->l2_bias);
  SENNA_free(chk->viterbi_score_init);
  SENNA_free(chk->viterbi_score_trans);
  
  SENNA_free(chk->input_state);
  SENNA_free(chk->hidden_state);
  SENNA_free(chk->output_state);
  SENNA_free(chk->labels);

  SENNA_free(chk);
}
