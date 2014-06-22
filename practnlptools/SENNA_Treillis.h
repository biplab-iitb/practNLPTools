#ifndef SENNA_TREILLIS_H
#define SENNA_TREILLIS_H

struct SENNA_TreillisEdge_;
struct SENNA_TreillisNode_;

typedef struct SENNA_TreillisEdge_
{
    struct SENNA_TreillisEdge_ *next;
    struct SENNA_TreillisNode_ *src_node;

    float score;

} SENNA_TreillisEdge;

typedef struct SENNA_TreillisNode_
{
    struct SENNA_TreillisNode_ *next;
    struct SENNA_TreillisEdge_ *edge;
    struct SENNA_TreillisEdge_ *last_edge;
    struct SENNA_TreillisEdge_ *max_edge;

    int label;
    int status;
    float score;
    float accScore;

} SENNA_TreillisNode;

typedef struct SENNA_Treillis_
{
    struct SENNA_TreillisNode_ **node;
    struct SENNA_TreillisNode_ **last_node;
    int size;
    int max_size;

    struct SENNA_TreillisNode_ **buffer_node;
    struct SENNA_TreillisEdge_ **buffer_edge;
    int max_buffer_node;
    int max_buffer_edge;
    int n_buffer_node;
    int n_buffer_edge;

} SENNA_Treillis;

SENNA_Treillis* SENNA_Treillis_new(void);
void SENNA_Treillis_resize(SENNA_Treillis *treillis, int size);
void SENNA_Treillis_addNode(SENNA_Treillis *treillis, int t, SENNA_TreillisNode *new_node);
void SENNA_Treillis_free(SENNA_Treillis *treillis);


SENNA_TreillisNode* SENNA_TreillisNode_new(SENNA_Treillis *treillis, float score, int label);
void SENNA_TreillisNode_addEdgeFromNode(SENNA_Treillis *treillis, SENNA_TreillisNode *dst_node, SENNA_TreillisNode *src_node, float score);
void SENNA_Treillis_viterbi(SENNA_Treillis *treillis, int *path);

#endif
