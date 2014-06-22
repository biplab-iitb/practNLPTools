#include "SENNA_PSG.h"
#include "SENNA_utils.h"
#include "SENNA_nn.h"
#include "SENNA_Scores2Treillis.h"

void SENNA_PSG_forward(SENNA_PSG *psg, const int *sentence_words, const int *sentence_caps, const int *sentence_posl, int sentence_size,
                       int **labels_, int *n_level_)
{
  int *sentence_psgl = SENNA_malloc(sizeof(int), sentence_size);
  int *sentence_segl = SENNA_malloc(sizeof(int), sentence_size);
  int *start_and_sentence_level_label = SENNA_malloc(sizeof(int), sentence_size+1);
  int t;
  int level;

  for(t = 0; t < sentence_size; t++)
  {
    sentence_psgl[t] = 0;
    sentence_segl[t] = 0;
  }

  psg->input_state = SENNA_realloc(psg->input_state, sizeof(float), sentence_size*psg->input_state_size);
  psg->l1_state = SENNA_realloc(psg->l1_state, sizeof(float), sentence_size*psg->l1_state_size);
  psg->l2_state = SENNA_realloc(psg->l2_state, sizeof(float), sentence_size*psg->l2_state_size);
  psg->l3_state = SENNA_realloc(psg->l3_state, sizeof(float), sentence_size*psg->l3_state_size);
  psg->l4_state = SENNA_realloc(psg->l4_state, sizeof(float), sentence_size*psg->l4_state_size);
  
  SENNA_nn_lookup(psg->input_state,                                                       psg->input_state_size, psg->ll_word_weight, psg->ll_word_size, psg->ll_word_max_idx, sentence_words, sentence_size, 0, 0);
  SENNA_nn_lookup(psg->input_state+psg->ll_word_size,                                     psg->input_state_size, psg->ll_caps_weight, psg->ll_caps_size, psg->ll_caps_max_idx, sentence_caps,  sentence_size, 0, 0);
  SENNA_nn_lookup(psg->input_state+psg->ll_word_size+psg->ll_caps_size,                   psg->input_state_size, psg->ll_posl_weight, psg->ll_posl_size, psg->ll_posl_max_idx, sentence_posl,  sentence_size, 0, 0);

  level = 0;
  while(1)
  {
    int all_tags_are_o;
    int all_in_one_segment;

    SENNA_nn_lookup(psg->input_state+psg->ll_word_size+psg->ll_caps_size+psg->ll_posl_size, psg->input_state_size, psg->ll_psgl_weight, psg->ll_psgl_size, psg->ll_psgl_max_idx, sentence_psgl,  sentence_size, 0, 0);
    
    SENNA_nn_temporal_convolution(psg->l1_state, psg->l1_state_size, psg->l1_weight, psg->l1_bias, psg->input_state, psg->input_state_size, sentence_size, 1);
    SENNA_nn_temporal_max_convolution(psg->l2_state, psg->l2_bias, psg->l1_state, psg->l1_state_size, sentence_size, psg->window_size);
    SENNA_nn_temporal_convolution(psg->l3_state, psg->l3_state_size, psg->l3_weight, psg->l3_bias, psg->l2_state, psg->l1_state_size, sentence_size, 1);
    SENNA_nn_hardtanh(psg->l3_state, psg->l3_state, psg->l3_state_size*sentence_size);
    SENNA_nn_temporal_convolution(psg->l4_state, psg->l4_state_size, psg->l4_weight, psg->l4_bias, psg->l3_state, psg->l3_state_size, sentence_size, 1);
    
    SENNA_Treillis_buildfromscorewithsegmentation(psg->treillis, psg->l4_state, psg->viterbi_score_init, psg->viterbi_score_trans, sentence_segl, psg->l4_state_size, sentence_size);    
    SENNA_Treillis_viterbi(psg->treillis, start_and_sentence_level_label);

    /* update history and segmentation */
    all_tags_are_o = 1;
    for(t = 0; t < sentence_size; t++)
    {
      if(start_and_sentence_level_label[t+1])
      {
        sentence_psgl[t] = start_and_sentence_level_label[t+1]; /* note we always keep if something was there */
        sentence_segl[t] = (start_and_sentence_level_label[t+1]-1)%4+1;
        
        all_tags_are_o = 0;
      }
    }

    /* check if only one big segment */
    if(sentence_size == 1)
      all_in_one_segment = (sentence_segl[0] == SEG_S);
    else
      all_in_one_segment = (sentence_segl[0] == SEG_B) && (sentence_segl[sentence_size-1] == SEG_E);

    for(t = 1; all_in_one_segment && (t < sentence_size-1); t++)
    {
      if(sentence_segl[t] != SEG_I)
        all_in_one_segment = 0;
    }

    level++;

    if(psg->max_labels_size < sentence_size*level)
    {
      psg->labels = SENNA_realloc(psg->labels, sizeof(float), sentence_size*level);
      psg->max_labels_size = sentence_size*level;
    }
    memcpy(psg->labels+(level-1)*sentence_size, start_and_sentence_level_label+1, sizeof(float)*sentence_size);

    if(all_in_one_segment || all_tags_are_o)
      break;
  }

  free(sentence_psgl);
  free(sentence_segl);
  free(start_and_sentence_level_label);

  *labels_ = psg->labels;
  *n_level_ = level;
}

SENNA_PSG* SENNA_PSG_new(const char *path, const char *subpath)
{
  SENNA_PSG *psg = SENNA_malloc(sizeof(SENNA_PSG), 1);
  FILE *f;
  float dummy;

  memset(psg, 0, sizeof(SENNA_PSG));

  f = SENNA_fopen(path, subpath, "rb");

  SENNA_fread_tensor_2d(&psg->ll_word_weight, &psg->ll_word_size, &psg->ll_word_max_idx, f);
  SENNA_fread_tensor_2d(&psg->ll_caps_weight, &psg->ll_caps_size, &psg->ll_caps_max_idx, f);
  SENNA_fread_tensor_2d(&psg->ll_posl_weight, &psg->ll_posl_size, &psg->ll_posl_max_idx, f);
  SENNA_fread_tensor_2d(&psg->ll_psgl_weight, &psg->ll_psgl_size, &psg->ll_psgl_max_idx, f);
  SENNA_fread_tensor_2d(&psg->l1_weight, &psg->input_state_size, &psg->l1_state_size, f);
  SENNA_fread_tensor_1d(&psg->l1_bias, &psg->l1_state_size, f);
  SENNA_fread_tensor_2d(&psg->l2_bias, &psg->l1_state_size, &psg->window_size, f);
  SENNA_fread_tensor_2d(&psg->l3_weight, &psg->l2_state_size, &psg->l3_state_size, f);
  SENNA_fread_tensor_1d(&psg->l3_bias, &psg->l3_state_size, f);
  SENNA_fread_tensor_2d(&psg->l4_weight, &psg->l3_state_size, &psg->l4_state_size, f);
  SENNA_fread_tensor_1d(&psg->l4_bias, &psg->l4_state_size, f);

  SENNA_fread_tensor_1d(&psg->viterbi_score_init, &psg->l4_state_size, f);
  SENNA_fread_tensor_2d(&psg->viterbi_score_trans, &psg->l4_state_size, &psg->l4_state_size, f);

  SENNA_fread(&psg->ll_word_padding_idx, sizeof(int), 1, f);
  SENNA_fread(&psg->ll_caps_padding_idx, sizeof(int), 1, f);
  SENNA_fread(&psg->ll_posl_padding_idx, sizeof(int), 1, f);
  SENNA_fread(&psg->ll_psgl_padding_idx, sizeof(int), 1, f);

  SENNA_fread(&dummy, sizeof(float), 1, f);
  SENNA_fclose(f);

  if((int)dummy != 777)
    SENNA_error("psg: data corrupted (or not IEEE floating computer)");

  psg->input_state = NULL;
  psg->l1_state = NULL;
  psg->l2_state = NULL;
  psg->l3_state = NULL;
  psg->l4_state = NULL;
  psg->labels = NULL;
  psg->treillis = SENNA_Treillis_new();

  /* some info if you want verbose */
  SENNA_message("psg: max window size: %d", psg->window_size);
  SENNA_message("psg: vector size in word lookup table: %d", psg->ll_word_size);
  SENNA_message("psg: word lookup table size: %d", psg->ll_word_max_idx);
  SENNA_message("psg: vector size in caps lookup table: %d", psg->ll_caps_size);
  SENNA_message("psg: caps lookup table size: %d", psg->ll_caps_max_idx);
  SENNA_message("psg: vector size in pos lookup table: %d", psg->ll_posl_size);
  SENNA_message("psg: pos lookup table size: %d", psg->ll_posl_max_idx);
  SENNA_message("psg: vector size in psg lookup table: %d", psg->ll_psgl_size);
  SENNA_message("psg: psg lookup table size: %d", psg->ll_psgl_max_idx);
  SENNA_message("psg: number of hidden units: %d [1]", psg->l1_state_size);
  SENNA_message("psg: number of hidden units: %d [2]", psg->l3_state_size);
  SENNA_message("psg: number of classes: %d", psg->l4_state_size);

  return psg;
}

void SENNA_PSG_free(SENNA_PSG *psg)
{
  SENNA_free(psg->ll_word_weight);
  SENNA_free(psg->ll_caps_weight);
  SENNA_free(psg->ll_posl_weight);
  SENNA_free(psg->ll_psgl_weight);
  SENNA_free(psg->l1_weight);
  SENNA_free(psg->l1_bias);  
  SENNA_free(psg->l2_bias);
  SENNA_free(psg->l3_weight);
  SENNA_free(psg->l3_bias);  
  SENNA_free(psg->l4_weight);
  SENNA_free(psg->l4_bias);  
  SENNA_free(psg->viterbi_score_init);
  SENNA_free(psg->viterbi_score_trans);
  
  SENNA_free(psg->input_state);
  SENNA_free(psg->l1_state);
  SENNA_free(psg->l2_state);
  SENNA_free(psg->l3_state);
  SENNA_free(psg->l4_state);
  SENNA_free(psg->labels);
  SENNA_Treillis_free(psg->treillis);

  SENNA_free(psg);
}
