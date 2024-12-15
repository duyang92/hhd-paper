#ifndef OUR_C4_H
#define OUR_C4_H
#include <stdint.h>
#include<vector>
#include <utility>
#include<unordered_set>
#include<unordered_map>


#define CELL_SIZE 296
typedef struct cell
{
    uint32_t key;
    uint8_t level;
    uint32_t count;
    uint32_t right_child_count;
    uint32_t subcounts[7];
} CELL;


#ifdef BIT
#define DEPTH 33
#define CELL_NUM 4
#else
#define DEPTH 5
#define CELL_NUM 2
#endif

#ifdef BIT
#define D 1
#else
#define D 1
#endif

#ifndef L
#define L 4
#endif

extern uint32_t masks[DEPTH];

#ifndef SEED
#define SEED 1010
#endif

#ifndef MEMORY
#define MEMORY 256
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 4
#endif

#if MEMORY==256
#define BUCKET_NUM 1760
#elif MEMORY==128
#define BUCKET_NUM 880
#elif MEMORY==512
#define BUCKET_NUM 3520
#elif MEMORY==1024
#define BUCKET_NUM 7040
#endif

#ifndef BLOCK_NUM
#define BLOCK_NUM 1024
#endif

#ifdef USEBLOCK
#ifndef BLOCK_LEVEL
#define BLOCK_LEVEL 4
#endif
#endif

#ifndef SEED1
#define SEED1 1231
#endif
typedef struct bucket
{
    uint8_t empty_cell;
    CELL cells[CELL_NUM];
//     std::unordered_set<uint64_t> keys;
} BUCKET;
void init(BUCKET *b, int bucket_num);
void insert(BUCKET *b, int bucket_num, uint32_t key,int threshold);
void insert_bit(BUCKET *b, int bucket_num, uint32_t key,int threshold);
std::vector<std::pair<uint64_t,uint32_t>> query(BUCKET *b,int bucket_num,int threshold);

#endif