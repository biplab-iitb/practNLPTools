#include "SENNA_Treillis.h"
#include "SENNA_utils.h"
#include "SENNA_Scores2Treillis.h"

void SENNA_Treillis_buildfromscore(SENNA_Treillis *treillis, float *score, float *start, float *trans, int N, int T)
{
  SENNA_TreillisNode *node_i, *node_j;
  long nLabel, i, j, t;
  long i_l, j_l, i_bies, j_bies;

  if((N-1) % 4)
    SENNA_error("Treillis: Number of tags is not divisible by 4...");

  SENNA_Treillis_resize(treillis, T+1);
  node_i = SENNA_TreillisNode_new(treillis, 0, 0);
  SENNA_Treillis_addNode(treillis, 0, node_i);

  nLabel = (N-1)/4+1; /* +1 for O */

  for(t = 0; t < T; t++)
  {
    for(j_l = 0; j_l < nLabel; j_l++)
    {
      if(t == 0)
      {
        if(j_l == 0) /* O */
        {
          j = 0;
          node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
          SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, start[j]);
          SENNA_Treillis_addNode(treillis, t+1, node_j);
        }
        else /* B & S */
        {
          if(T > 1) /* not for 1 word sentences */
          {
            j = (j_l-1)*4+1; /* B */
            node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
            SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, start[j]);
            SENNA_Treillis_addNode(treillis, t+1, node_j);
          }

          j = (j_l-1)*4+3+1; /* S */
          node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
          SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, start[j]);
          SENNA_Treillis_addNode(treillis, t+1, node_j);
        }
      }
      else
      {
        /* O first (not duplicated with BIES) */
        if(j_l == 0)
        {
          j = 0;
          node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
          node_i = treillis->node[t];
          while(node_i)
          {
            i = node_i->label;
            i_bies = (i-1)%4;
            if((i == 0) || (i_bies == 2) || (i_bies == 3)) /* previous: O or E- or S- */
            {
              SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, trans[j*N+i]);
            }
            node_i = node_i->next;
          }
          if(node_j->edge)
            SENNA_Treillis_addNode(treillis, t+1, node_j);
        }
        else
        {
          for(j_bies = 0; j_bies < 4; j_bies++)
          {
            if((t == T-1) && (j_bies == 0 || j_bies == 1))
              continue; /* no B or I at end of sentence */

            j = (j_l-1)*4+j_bies+1;
            node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
            node_i = treillis->node[t];
            while(node_i)
            {
              i = node_i->label;
              i_bies = (i-1)%4;
              i_l = (i == 0 ? 0 : (i-1)/4+1);

              if( ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
              ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) /* B-X || I-X --> I-X || E-X */
                )
              {
                SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, trans[j*N+i]);
              }
              node_i = node_i->next;
            }
            if(node_j->edge)
              SENNA_Treillis_addNode(treillis, t+1, node_j);
          }
        } /* j_l == 0 ? */
      } /* t == 0 ? */
    } /* on all labels */
  } /* on all t */
}

#define ADD_NODE(STATUS, CONDITION) \
  {                                                                     \
    j = (j_l-1)*4+j_bies+1;                                             \
    node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);         \
    node_j->status = STATUS;                                            \
    node_i = treillis->node[t];                                         \
    while(node_i)                                                       \
    {                                                                   \
      i = node_i->label;                                                \
      i_bies = (i-1)%4;                                                 \
      i_l = (i == 0 ? 0 : (i-1)/4+1);                                   \
                                                                        \
      if( CONDITION )                                                   \
        SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, trans[j*N+i]); \
      node_i = node_i->next;                                            \
    }                                                                   \
    if(node_j->edge)                                                    \
      SENNA_Treillis_addNode(treillis, t+1, node_j);                    \
  }                                                                     \

void SENNA_Treillis_buildfromscorewithsegmentation(SENNA_Treillis *treillis, float *score, float *start, float *trans, int *segmentation, int N, int T)
{
  SENNA_TreillisNode *node_i, *node_j;
  long nLabel, i, j, t;
  long i_l, j_l, i_bies, j_bies;

  if((N-1) % 4)
    SENNA_error("Treillis: Number of tags is not divisible by 4...");

  SENNA_Treillis_resize(treillis, T+1);
  node_i = SENNA_TreillisNode_new(treillis, 0, 0);
  SENNA_Treillis_addNode(treillis, 0, node_i);

  nLabel = (N-1)/4+1; /* +1 for O */

  for(t = 0; t < T; t++)
  {
    int segment = segmentation[t];

    for(j_l = 0; j_l < nLabel; j_l++)
    {
      if(t == 0)
      {
        if(j_l == 0) /* O */
        {
          j = 0;
          node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
          node_j->status = (segment == SEG_O ? STATUS_SEG_NIL : STATUS_SEG_O);
          SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, start[j]);
          SENNA_Treillis_addNode(treillis, t+1, node_j);
        }
        else /* B & S */
        {
          if(T > 1) /* not for 1 word sentences */
          {
            j = (j_l-1)*4+1; /* B */
            node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
            node_j->status = (segment == SEG_O ? STATUS_SEG_NIL : STATUS_SEG_B);
            SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, start[j]);
            SENNA_Treillis_addNode(treillis, t+1, node_j);
          }

          if(segment == SEG_O)
          {
            j = (j_l-1)*4+3+1; /* S */
            node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
            node_j->status = STATUS_SEG_NIL;
            SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, start[j]);
            SENNA_Treillis_addNode(treillis, t+1, node_j);
          }
        }
      }
      else
      {
        /* O first (not duplicated with BIES) */
        if(j_l == 0)
        {
          j = 0;
          node_j = SENNA_TreillisNode_new(treillis, score[t*N+j], j);
          node_j->status = (segment == SEG_O ? STATUS_SEG_NIL : STATUS_SEG_O);
          node_i = treillis->node[t];
          while(node_i)
          {
            int condition = 0;
            i = node_i->label;
            i_bies = (i-1)%4;

            if(segment == SEG_O || segment == SEG_B || segment == SEG_S)
              condition = ((i == 0) || (i_bies == 2) || (i_bies == 3)); /* previous: O or E- or S- */
            else /* (segment == SEG_I || segment == SEG_E) */
              condition = ((i == 0) && (node_i->status == STATUS_SEG_O)); /* only O if in segment */

            if(condition) 
              SENNA_TreillisNode_addEdgeFromNode(treillis, node_j, node_i, trans[j*N+i]);

            node_i = node_i->next;
          }
          if(node_j->edge)
            SENNA_Treillis_addNode(treillis, t+1, node_j);
        }
        else
        {
          for(j_bies = 0; j_bies < 4; j_bies++)
          {
            if((t == T-1) && (j_bies == 0 || j_bies == 1))
              continue; /* no B or I at end of sentence */

            if(segment == SEG_I && j_bies != 1)
              continue; /* only I allowed inside segments */

            if(segment != SEG_O && j_bies == 3)
              continue; /* no S allowed in segments */

            if(segment == SEG_B && j_bies == 2)
              continue; /* no E at beginning of segments */
            
            if(segment == SEG_E && j_bies == 0)
              continue; /* no B at ending of segments */

            if(segment == SEG_O)
            {
              ADD_NODE(STATUS_SEG_NIL,
              ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
              ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) ); /* B-X || I-X --> I-X || E-X */
            }
            else if(segment == SEG_B)
            {
              if(j_bies == 0) /* B */
              {
                ADD_NODE(STATUS_SEG_B,
                ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
                ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) ); /* B-X || I-X --> I-X || E-X */
              }
              else /* I (S not allowed in segments) */
              {
                ADD_NODE(STATUS_SEG_I,
                ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
                ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) ); /* B-X || I-X --> I-X || E-X */

                ADD_NODE(STATUS_SEG_E,
                ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
                ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) ); /* B-X || I-X --> I-X || E-X */
              }
            }
            else if(segment == SEG_I)
            { /* only I allowed here */
              ADD_NODE(STATUS_SEG_B, ( (i_l == j_l) && (node_i->status == STATUS_SEG_B) ));
              ADD_NODE(STATUS_SEG_I, ( (i_l == j_l) && (node_i->status == STATUS_SEG_I) ));
              ADD_NODE(STATUS_SEG_E, ( (i_l == j_l) && (node_i->status == STATUS_SEG_E) ));
            }
            else if(segment == SEG_E)
            {
              if(j_bies == 1) /* I */
              {
                ADD_NODE(STATUS_SEG_B, ( (i_l == j_l) && (node_i->status == STATUS_SEG_B) ));
                ADD_NODE(STATUS_SEG_I, ( (i_l == j_l) && (node_i->status == STATUS_SEG_I) ));
              }
              else /* E , no B allowed here */
              {
                ADD_NODE(STATUS_SEG_E, ( (i_l == j_l) && (node_i->status == STATUS_SEG_E) ));
              }
            }
            else if(segment == SEG_S)
            {
              if(j_bies == 0) /* B */
              {
                ADD_NODE(STATUS_SEG_B,
                ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
                ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) ); /* B-X || I-X --> I-X || E-X */
              }
              else if(j_bies == 1) /* I */
              {
                ADD_NODE(STATUS_SEG_I,
                ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
                ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) ); /* B-X || I-X --> I-X || E-X */
              }
              else /* E , no S possible in segments */
              {
                ADD_NODE(STATUS_SEG_E,
                ( ((i_l == 0) || (i_bies == 3) || (i_bies == 2)) && ((j_bies == 0) || (j_bies == 3)) ) /* O || S-? || E-? --> B-? || S-? */
                ||  ( (i_l == j_l) && ((i_bies == 0) || (i_bies == 1)) && ((j_bies == 1) || (j_bies == 2))) ); /* B-X || I-X --> I-X || E-X */
              }
            }
            else
              SENNA_error("Treillis: unknown segmentation tag");
          }
        } /* j_l == 0 ? */
      } /* t == 0 ? */
    } /* on all labels */
  } /* on all t */
}
