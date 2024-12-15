#ifndef OUR_CLASS_H
#define OUR_CLASS_H
#include <stdint.h>
#include<vector>
#include <utility>
#include<unordered_set>
#include<unordered_map>
#include<random>
typedef struct cell
{
    uint32_t key;
    uint8_t level;
    uint32_t count;
    uint8_t is_empty;
    uint8_t flag;
    uint32_t heavy_child_size;
    uint32_t delta;
    uint32_t replace_time;
    uint32_t left_child_count;
    uint32_t right_child_count;
} CELL;


#ifdef BIT
#define DEPTH 33
#define CELL_NUM 3
#else
#define DEPTH 5
#define CELL_NUM 2
#endif

#ifdef BIT
#define D 4
#else
#define D 1
#endif



extern uint32_t masks[DEPTH];

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
class our_class
{
private:
    std::mt19937_64 gen;
    BUCKET* b;
    int bucket_num;
    int threshold;
public:
    our_class(int bucket_num, int threshold);
    ~our_class();
    void init();
    void insert(int bucket_num, uint32_t key,int threshold);
    std::vector<std::pair<uint64_t,uint32_t>> query(int bucket_num,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &est_size,std::unordered_map<uint64_t,uint32_t> &est_concnt);
    int find_cell(int bucket_num, uint32_t key,uint32_t level,int threshold);
    BUCKET* get_b();
};
#endif


