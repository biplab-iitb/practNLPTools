#include "SENNA_Treillis.h"
#include "SENNA_utils.h"

#include <float.h>

/* treillis */

SENNA_Treillis* SENNA_Treillis_new(void)
{
  SENNA_Treillis *treillis;

  treillis = SENNA_malloc(sizeof(SENNA_Treillis), 1);
  treillis->node = NULL;
  treillis->last_node = NULL;
  treillis->size = 0;
  treillis->max_size = 0;

  treillis->buffer_node = NULL;
  treillis->n_buffer_node = 0;
  treillis->max_buffer_node = 0;

  treillis->buffer_edge = NULL;
  treillis->n_buffer_edge = 0;
  treillis->max_buffer_edge = 0;

  return treillis;
}

void SENNA_Treillis_resize(SENNA_Treillis *treillis, int size)
{
  int t;

  if(size > treillis->max_size)
  {
    treillis->node = SENNA_realloc(treillis->node, sizeof(SENNA_TreillisNode*), size);
    treillis->last_node = SENNA_realloc(treillis->last_node, sizeof(SENNA_TreillisNode*), size);
    treillis->max_size = size;
  }

  treillis->size = size;
  for(t = 0; t < size; t++)
  {
    treillis->node[t] = NULL;
    treillis->last_node[t] = NULL;
  }

  treillis->n_buffer_node = 0;
  treillis->n_buffer_edge = 0;
}

void SENNA_Treillis_addNode(SENNA_Treillis *treillis, int t, SENNA_TreillisNode *new_node)
{
  if( (t < 0) || (t > treillis->size) )
    SENNA_error("Treillis: t out of range");

  if(new_node->next)
    SENNA_error("Treillis: node already in use");

  if(treillis->node[t])
  {
    treillis->last_node[t]->next = new_node;
    treillis->last_node[t] = new_node;
  }
  else
  {
    treillis->node[t] = new_node;
    treillis->last_node[t] = new_node;
  }
}

void SENNA_Treillis_free(SENNA_Treillis *treillis)
{
  int i;

  for(i = 0; i < treillis->max_buffer_node; i++)
    SENNA_free(treillis->buffer_node[i]);

  for(i = 0; i < treillis->max_buffer_edge; i++)
    SENNA_free(treillis->buffer_edge[i]);

  SENNA_free(treillis->node);
  SENNA_free(treillis->last_node);
  SENNA_free(treillis->buffer_node);
  SENNA_free(treillis->buffer_edge);
  SENNA_free(treillis);
}

/* nodes */

SENNA_TreillisNode* SENNA_TreillisNode_new(SENNA_Treillis *treillis, float score, int label)
{
  SENNA_TreillisNode *node;

  if(treillis->n_buffer_node >= treillis->max_buffer_node)
  {
    int max_buffer_node = 2*(treillis->max_buffer_node+1);
    int i;

    treillis->buffer_node = SENNA_realloc(treillis->buffer_node, sizeof(SENNA_TreillisNode*), max_buffer_node);
    for(i = treillis->max_buffer_node; i < max_buffer_node; i++)
      treillis->buffer_node[i] = SENNA_malloc(sizeof(SENNA_TreillisNode), 1);
    treillis->max_buffer_node = max_buffer_node;
  }

  node = treillis->buffer_node[treillis->n_buffer_node++];
  node->next = NULL;
  node->edge = NULL;
  node->last_edge = NULL;
  node->max_edge = NULL;
  node->score = score;
  node->accScore = 0;
  node->label = label;
  node->status = -1;
  return node;
}

void SENNA_TreillisNode_addEdgeFromNode(SENNA_Treillis *treillis,
                                        SENNA_TreillisNode *dst_node, SENNA_TreillisNode *src_node,
                                        float score)
{
  SENNA_TreillisEdge *new_edge;

  if(treillis->n_buffer_edge >= treillis->max_buffer_edge)
  {
    int max_buffer_edge = 2*(treillis->max_buffer_edge+1);
    int i;

    treillis->buffer_edge = SENNA_realloc(treillis->buffer_edge, sizeof(SENNA_TreillisEdge*), max_buffer_edge);
    for(i = treillis->max_buffer_edge; i < max_buffer_edge; i++)
      treillis->buffer_edge[i] = SENNA_malloc(sizeof(SENNA_TreillisEdge), 1);
    treillis->max_buffer_edge = max_buffer_edge;
  }

  new_edge = treillis->buffer_edge[treillis->n_buffer_edge++];
  new_edge->next = NULL;
  new_edge->src_node = src_node;
  new_edge->score = score;

  if(dst_node->edge)
  {
    dst_node->last_edge->next = new_edge;
    dst_node->last_edge = new_edge;
  }
  else
  {
    dst_node->edge = new_edge;
    dst_node->last_edge = new_edge;
  }
}

void SENNA_Treillis_viterbi(SENNA_Treillis *treillis, int *path)
{
  SENNA_TreillisNode *node, *max_node;
  SENNA_TreillisEdge *edge, *max_edge;
  int t, T;
  float max_score;

  T = treillis->size;
  
  node = treillis->node[0];
  while(node)
  {
    node->accScore = node->score;
    node = node->next;
  }
  
  for(t = 1; t < T; t++)
  {
    node = treillis->node[t];
    
    while(node)
    {
      max_score = -FLT_MAX;
      max_edge = NULL;
      edge = node->edge;
      while(edge)
      {
        float z = edge->src_node->accScore + edge->score;
        if(z > max_score)
        {
          max_score = z;
          max_edge = edge;
        }
        edge = edge->next;
      }

      node->accScore = max_score + node->score;
      node->max_edge = max_edge;
      node = node->next;
    }
  }

  {
    float treillisScore = -FLT_MAX;
    max_node = NULL;
    node = treillis->node[treillis->size-1];
    while(node)
    {
      if(node->accScore > treillisScore)
      {
        treillisScore = node->accScore;
        max_node = node;
      }
      node = node->next;
    }
  }

  path[T-1] = max_node->label;
  for(t = T-1; t > 0; t--)
  {
    max_node = max_node->max_edge->src_node;
    path[t-1] = max_node->label;
  }
}
