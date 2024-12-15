#ifndef OUR_H
#define OUR_H
#include <stdint.h>
#include<vector>
#include <utility>
#include<stdlib.h>
#include<stdio.h>
typedef struct cell
{
    uint64_t key;
    uint64_t level;
    uint32_t count;
    uint8_t is_empty;
    uint8_t flag;
    uint32_t heavy_child_size;
    uint32_t delta;
} CELL;


#ifdef BIT
#define DEPTH 65
#define MAX_LEVEL 32
#define CELL_NUM 7
#else
#define DEPTH 9
#define MAX_LEVEL 4
#define CELL_NUM 2
#endif

#ifdef BIT
#define D 3
#else
#define D 1
#endif



extern uint32_t masks[MAX_LEVEL+1];

#ifndef SEED
#define SEED 1010
#endif

#ifndef SEED1
#define SEED1 1231
#endif
typedef struct bucket
{
    CELL cells[CELL_NUM];
} BUCKET;
;
void init(BUCKET *b, int bucket_num);
void insert(BUCKET *b, int bucket_num, uint64_t key,int threshold);
void insert_bit(BUCKET *b, int bucket_num, uint64_t key,int threshold);
std::vector<std::pair<__uint128_t,uint32_t>> query(BUCKET *b,int bucket_num,uint32_t threshold);

#endif