#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "SENNA_utils.h"
#include "SENNA_Hash.h"
#include "SENNA_Tokenizer.h"

#include "SENNA_POS.h"
#include "SENNA_CHK.h"
#include "SENNA_NER.h"
#include "SENNA_VBS.h"
#include "SENNA_PT0.h"
#include "SENNA_SRL.h"
#include "SENNA_PSG.h"

/* fgets max sizes */
#define MAX_SENTENCE_SIZE 1024
#define MAX_TARGET_VB_SIZE 256

static void help(const char *name)
{
  printf("\n");
  printf("SENNA Tagger (POS - CHK - NER - SRL)\n");
  printf("(c) Ronan Collobert 2009\n");
  printf("\n");
  printf("Usage: %s [options]\n", name);
  printf("\n");
  printf(" Takes sentence (one line per sentence) on stdin\n");
  printf(" Outputs tags on stdout\n");
  printf(" Typical usage: %s [options] < inputfile.txt > outputfile.txt\n", name);
  printf("\n");
  printf("Display options:\n");
  printf("  -h             Display this help\n");
  printf("  -verbose       Display model informations on stderr\n");
  printf("  -notokentags   Do not output tokens\n");
  printf("  -offsettags    Output start/end offset of each token\n");
  printf("  -iobtags       Output IOB tags instead of IOBES\n");
  printf("  -brackettags   Output 'bracket' tags instead of IOBES\n");
  printf("\n");
  printf("Data options:\n");
  printf("  -path <path>   Path to the SENNA data/ and hash/ directories [default: ./]\n");
  printf("\n");
  printf("Input options:\n");
  printf("  -usrtokens     Use user's tokens (space separated) instead of SENNA tokenizer\n");
  printf("\n");
  printf("SRL options:\n");
  printf("  -posvbs        Use POS verbs instead of SRL style verbs for SRL task\n");
  printf("  -usrvbs <file> Use user's verbs (given in <file>) instead of SENNA verbs for SRL task\n");
  printf("\n");
  printf("Tagging options:\n");
  printf("  -pos           Output POS\n");
  printf("  -chk           Output CHK\n");
  printf("  -ner           Output NER\n");
  printf("  -srl           Output SRL\n");
  printf("  -psg           Output PSG\n");
  printf("\n");

  exit(-1);
}

int main(int argc, char *argv[])
{
  int i, j;

  /* options */
  char *opt_path = NULL;
  int opt_verbose = 0;
  int opt_notokentags = 0;
  int opt_offsettags = 0;
  int opt_iobtags = 0;
  int opt_brackettags = 0;
  int opt_posvbs = 0;
  int opt_usrtokens = 0;
  int opt_pos = 0;
  int opt_chk = 0;
  int opt_ner = 0;
  int opt_srl = 0;
  int opt_psg = 0;
  FILE *opt_usrvbs = NULL;

  for(i = 1; i < argc; i++)
  {
    if(!strcmp(argv[i], "-verbose"))
      opt_verbose = 1;
    else if(!strcmp(argv[i], "-notokentags"))
      opt_notokentags = 1;
    else if(!strcmp(argv[i], "-offsettags"))
      opt_offsettags = 1;
    else if(!strcmp(argv[i], "-iobtags"))
      opt_iobtags = 1;
    else if(!strcmp(argv[i], "-brackettags"))
      opt_brackettags = 1;
    else if(!strcmp(argv[i], "-path"))
    {
      if(i+1 >= argc)
        SENNA_error("please provide a path for the -path option");
      opt_path = argv[i+1];
      i++;
    }
    else if(!strcmp(argv[i], "-posvbs"))
      opt_posvbs = 1;
    else if(!strcmp(argv[i], "-usrtokens"))
      opt_usrtokens = 1;
    else if(!strcmp(argv[i], "-usrvbs"))
    {
      if(i+1 >= argc)
        SENNA_error("please provide a filename for the -usrvbs option");
      opt_usrvbs = SENNA_fopen(NULL, argv[i+1], "rb");
      i++;
    }
    else if(!strcmp(argv[i], "-pos"))
      opt_pos = 1;
    else if(!strcmp(argv[i], "-chk"))
      opt_chk = 1;
    else if(!strcmp(argv[i], "-ner"))
      opt_ner = 1;
    else if(!strcmp(argv[i], "-srl"))
      opt_srl = 1;
    else if(!strcmp(argv[i], "-psg"))
      opt_psg = 1;
    else
    {
      printf("invalid argument: %s\n", argv[i]);
      help(argv[0]);
    }
  }

  SENNA_set_verbose_mode(opt_verbose);

  if(!opt_pos && !opt_chk && !opt_ner && !opt_srl && !opt_psg) /* the user does not know what he wants... */
    opt_pos = opt_chk = opt_ner = opt_srl = opt_psg = 1;     /* so give him everything (aren't we insane?) */


  /* the real thing */
  {
    char sentence[MAX_SENTENCE_SIZE];
    char target_vb[MAX_TARGET_VB_SIZE];
    int *chk_labels = NULL;
    int *pt0_labels = NULL;
    int *pos_labels = NULL;
    int *ner_labels = NULL;
    int *vbs_labels = NULL;
    int **srl_labels = NULL;
    int *psg_labels = NULL;
    int n_psg_level = 0;
    int is_psg_one_segment = 0;
    int vbs_hash_novb_idx = 22;
    int n_verbs = 0;
    
    /* inputs */
    SENNA_Hash *word_hash = SENNA_Hash_new(opt_path, "hash/words.lst");
    SENNA_Hash *caps_hash = SENNA_Hash_new(opt_path, "hash/caps.lst");
    SENNA_Hash *suff_hash = SENNA_Hash_new(opt_path, "hash/suffix.lst");
    SENNA_Hash *gazt_hash = SENNA_Hash_new(opt_path, "hash/gazetteer.lst");

    SENNA_Hash *gazl_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.loc.lst", "data/ner.loc.dat");
    SENNA_Hash *gazm_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.msc.lst", "data/ner.msc.dat");
    SENNA_Hash *gazo_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.org.lst", "data/ner.org.dat");
    SENNA_Hash *gazp_hash = SENNA_Hash_new_with_admissible_keys(opt_path, "hash/ner.per.lst", "data/ner.per.dat");

    /* labels */
    SENNA_Hash *pos_hash = SENNA_Hash_new(opt_path, "hash/pos.lst");
    SENNA_Hash *chk_hash = SENNA_Hash_new(opt_path, "hash/chk.lst");
    SENNA_Hash *pt0_hash = SENNA_Hash_new(opt_path, "hash/pt0.lst");
    SENNA_Hash *ner_hash = SENNA_Hash_new(opt_path, "hash/ner.lst");
    SENNA_Hash *vbs_hash = SENNA_Hash_new(opt_path, "hash/vbs.lst");
    SENNA_Hash *srl_hash = SENNA_Hash_new(opt_path, "hash/srl.lst");
    SENNA_Hash *psg_left_hash = SENNA_Hash_new(opt_path, "hash/psg-left.lst");
    SENNA_Hash *psg_right_hash = SENNA_Hash_new(opt_path, "hash/psg-right.lst");

    SENNA_POS *pos = SENNA_POS_new(opt_path, "data/pos.dat");
    SENNA_CHK *chk = SENNA_CHK_new(opt_path, "data/chk.dat");
    SENNA_PT0 *pt0 = SENNA_PT0_new(opt_path, "data/pt0.dat");
    SENNA_NER *ner = SENNA_NER_new(opt_path, "data/ner.dat");
    SENNA_VBS *vbs = SENNA_VBS_new(opt_path, "data/vbs.dat");
    SENNA_SRL *srl = SENNA_SRL_new(opt_path, "data/srl.dat");
    SENNA_PSG *psg = SENNA_PSG_new(opt_path, "data/psg.dat");

    SENNA_Tokenizer *tokenizer = SENNA_Tokenizer_new(word_hash, caps_hash, suff_hash, gazt_hash, gazl_hash, gazm_hash, gazo_hash, gazp_hash, opt_usrtokens);

    if(opt_iobtags)
    {
      SENNA_Hash_convert_IOBES_to_IOB(chk_hash);
      SENNA_Hash_convert_IOBES_to_IOB(ner_hash);
      SENNA_Hash_convert_IOBES_to_IOB(srl_hash);
    }
    else if(opt_brackettags)
    {
      SENNA_Hash_convert_IOBES_to_brackets(chk_hash);
      SENNA_Hash_convert_IOBES_to_brackets(ner_hash);
      SENNA_Hash_convert_IOBES_to_brackets(srl_hash);
    }

    SENNA_message("ready");

    while(fgets(sentence, MAX_SENTENCE_SIZE, stdin))
    {
      SENNA_Tokens* tokens = SENNA_Tokenizer_tokenize(tokenizer, sentence);
    
      if(tokens->n == 0)
        continue;

      pos_labels = SENNA_POS_forward(pos, tokens->word_idx, tokens->caps_idx, tokens->suff_idx, tokens->n);
      if(opt_chk)
        chk_labels = SENNA_CHK_forward(chk, tokens->word_idx, tokens->caps_idx, pos_labels, tokens->n);
      if(opt_srl)
        pt0_labels = SENNA_PT0_forward(pt0, tokens->word_idx, tokens->caps_idx, pos_labels, tokens->n);
      if(opt_ner)
        ner_labels = SENNA_NER_forward(ner, tokens->word_idx, tokens->caps_idx, tokens->gazl_idx, tokens->gazm_idx, tokens->gazo_idx, tokens->gazp_idx, tokens->n);
      if(opt_srl)
      {
        if(opt_usrvbs)
        {
          vbs_labels = SENNA_realloc(vbs_labels, sizeof(int), tokens->n);
          n_verbs = 0;
          for(i = 0; i < tokens->n; i++)
          {
            if(!SENNA_fgetline(target_vb, MAX_TARGET_VB_SIZE, opt_usrvbs))
              SENNA_error("invalid user verbs file\n");
            vbs_labels[i] = !( (target_vb[0] == '-') && ( (target_vb[1] == '\0') || isspace(target_vb[1])) );
            n_verbs += vbs_labels[i];          
          }
          if(!SENNA_fgetline(target_vb, MAX_TARGET_VB_SIZE, opt_usrvbs))
            SENNA_error("invalid user verbs file\n");
          if(strlen(target_vb) > 0)
            SENNA_error("sentence size does not match in user verbs file");
        }
        else if(opt_posvbs)
        {
          vbs_labels = SENNA_realloc(vbs_labels, sizeof(int), tokens->n);
          n_verbs = 0;
          for(i = 0; i < tokens->n; i++)
          {
            vbs_labels[i] = (SENNA_Hash_key(pos_hash, pos_labels[i])[0] == 'V');
            n_verbs += vbs_labels[i];
          }
        }
        else
        {
          vbs_labels = SENNA_VBS_forward(vbs, tokens->word_idx, tokens->caps_idx, pos_labels, tokens->n);
          n_verbs = 0;
          for(i = 0; i < tokens->n; i++)
          {
            vbs_labels[i] = (vbs_labels[i] != vbs_hash_novb_idx);
            n_verbs += vbs_labels[i];
          }
        }
      }

      if(opt_srl)
        srl_labels = SENNA_SRL_forward(srl, tokens->word_idx, tokens->caps_idx, pt0_labels, vbs_labels, tokens->n);

      if(opt_psg)
      {
        SENNA_PSG_forward(psg, tokens->word_idx, tokens->caps_idx, pos_labels, tokens->n, &psg_labels, &n_psg_level);

        /* check if top level takes the full sentence */
        {
          int *psg_top_labels = psg_labels + (n_psg_level-1)*tokens->n;

          if(tokens->n == 1)
            is_psg_one_segment = ((psg_top_labels[0]-1) % 4 == 3); /* S- ? */
          else
            is_psg_one_segment = ((psg_top_labels[0]-1) % 4 == 0) && ((psg_top_labels[tokens->n-1]-1) % 4 == 2); /* B- or E- ? */

          for(i = 1; is_psg_one_segment && (i < tokens->n-1); i++)
          {
            if((psg_top_labels[i]-1) % 4 != 1) /* I- ? */
              is_psg_one_segment = 0;
          }
        }
      }

      for(i = 0; i < tokens->n; i++)
      {
        if(!opt_notokentags)
          printf("%15s", tokens->words[i]);
        if(opt_offsettags)
          printf("\t%d %d", tokens->start_offset[i], tokens->end_offset[i]);
        if(opt_pos)
          printf("\t%10s", SENNA_Hash_key(pos_hash, pos_labels[i]));
        if(opt_chk)
          printf("\t%10s", SENNA_Hash_key(chk_hash, chk_labels[i]));
        if(opt_ner)
          printf("\t%10s", SENNA_Hash_key(ner_hash, ner_labels[i]));
        if(opt_srl)
        {
          printf("\t%15s", (vbs_labels[i] ? tokens->words[i] : "-"));
          for(j = 0; j < n_verbs; j++)
            printf("\t%10s", SENNA_Hash_key(srl_hash, srl_labels[j][i]));
        }
        if(opt_psg) /* last, can be long */
        {
          printf("\t");
          if(i == 0)
          {
            printf("(S1");
            if(!is_psg_one_segment)
              printf("(S");
          }
          for(j = n_psg_level-1; j >= 0; j--)
            printf("%s", SENNA_Hash_key(psg_left_hash, psg_labels[j*tokens->n+i]));
          printf("*");
          for(j = 0; j < n_psg_level; j++)
            printf("%s", SENNA_Hash_key(psg_right_hash, psg_labels[j*tokens->n+i]));
          if(i == tokens->n-1)
          {
            if(!is_psg_one_segment)
              printf(")");
            printf(")");
          }
        }
        printf("\n");
      }
      printf("\n"); /* end of sentence */
    }

    if(opt_posvbs)
      SENNA_free(vbs_labels);

    if(opt_usrvbs)
    {
      SENNA_free(vbs_labels);
      SENNA_fclose(opt_usrvbs);
    }

    SENNA_Tokenizer_free(tokenizer);

    SENNA_POS_free(pos);
    SENNA_CHK_free(chk);
    SENNA_PT0_free(pt0);
    SENNA_NER_free(ner);
    SENNA_VBS_free(vbs);
    SENNA_SRL_free(srl);
    SENNA_PSG_free(psg);

    SENNA_Hash_free(word_hash);
    SENNA_Hash_free(caps_hash);
    SENNA_Hash_free(suff_hash);
    SENNA_Hash_free(gazt_hash);

    SENNA_Hash_free(gazl_hash);
    SENNA_Hash_free(gazm_hash);
    SENNA_Hash_free(gazo_hash);
    SENNA_Hash_free(gazp_hash);

    SENNA_Hash_free(pos_hash);
    SENNA_Hash_free(chk_hash);
    SENNA_Hash_free(pt0_hash);
    SENNA_Hash_free(ner_hash);
    SENNA_Hash_free(vbs_hash);
    SENNA_Hash_free(srl_hash);
    SENNA_Hash_free(psg_left_hash);
    SENNA_Hash_free(psg_right_hash);
  }

  return 0;
}
