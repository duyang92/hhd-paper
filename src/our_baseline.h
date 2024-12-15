#ifndef OUR_BASELINE_H
#define OUR_BASELINE_H
#include <stdint.h>
#include<vector>
#include <utility>
#include<unordered_set>
#include<unordered_map>


#define CELL_SIZE 72
typedef struct cell
{
    uint32_t key;
    uint8_t level;
    uint32_t count;
} CELL;


#ifdef BIT
#define DEPTH 33
#define CELL_NUM 4
#else
#define DEPTH 5
#define CELL_NUM 2
#endif





extern uint32_t masks[DEPTH];

#ifndef SEED
#define SEED 1010
#endif

#ifndef MEMORY
#define MEMORY 256
#endif

#ifdef BIT
#if MEMORY==256
#define BUCKET_NUM 7084
#elif MEMORY==128
#define BUCKET_NUM 3542
#elif MEMORY==512
#define BUCKET_NUM 14168
#elif MEMORY==1024
#define BUCKET_NUM 28336
#elif MEMORY==64
#define BUCKET_NUM 1771
#endif
#else
#if MEMORY==256
#define BUCKET_NUM 14168
#elif MEMORY==128
#define BUCKET_NUM 7084
#elif MEMORY==512
#define BUCKET_NUM 28336
#elif MEMORY==1024
#define BUCKET_NUM 58254
#elif MEMORY==64
#define BUCKET_NUM 3542
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
;
void init(BUCKET *b, int bucket_num);
void insert(BUCKET *b, int bucket_num, uint32_t key,int threshold);
void insert_bit(BUCKET *b, int bucket_num, uint32_t key,int threshold);
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time);
std::vector<std::pair<uint64_t,uint32_t>> query(BUCKET *b,int bucket_num,uint32_t threshold);
std::vector<std::pair<uint64_t,uint32_t>> query_test(BUCKET *b,int bucket_num,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &est_size,std::unordered_map<uint64_t,uint32_t> &est_concnt);
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold);
std::vector<std::pair<uint64_t,uint32_t>> query_test(BUCKET *b,int bucket_num,int threshold);
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold);
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time);
#endif