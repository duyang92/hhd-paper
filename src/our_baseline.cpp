#include "our_baseline.h"
#include "MurmurHash3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include<unordered_map>
#include<unordered_set>
#include<math.h>
#include<iostream>
#include<vector>
#include <utility>
#include<random>

uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed  )
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    uint32_t h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len)
    {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

void init(BUCKET *b, int bucket_num)
{
    for (int i = 0; i < bucket_num; i++)
    {
        (b + i)->empty_cell = CELL_NUM;
        for (int j = 0; j < CELL_NUM; j++)
        {
            (b+i)->cells[j].count=0;
            (b + i)->cells[j].key = 0;
            (b + i)->cells[j].level = 0;
        }
    }
}
uint32_t find_cell(BUCKET *b, int bucket_idx, uint32_t key,uint32_t level,int threshold)
{
    int min_val_cell_idx = -1;
    int empty_cell_idx = -1;
    int is_inserted=0;
    uint32_t min_cell_val=UINT32_MAX;
    uint64_t masked_key=key&masks[level];
    uint64_t x=(masked_key<<32)+level;
    if(b[bucket_idx].empty_cell>0)
    {
        b[bucket_idx].empty_cell-=1;
        is_inserted=1;
        empty_cell_idx=b[bucket_idx].empty_cell;
        b[bucket_idx].cells[empty_cell_idx].key = key;
        b[bucket_idx].cells[empty_cell_idx].level = level;
        b[bucket_idx].cells[empty_cell_idx].count = 1;
        return 0;
    }
    for (int i = 0; i < CELL_NUM; i++)
    {
        if (b[bucket_idx].cells[i].level==level&&(b[bucket_idx].cells[i].key&masks[level])== masked_key)
        {
            is_inserted=1;
            b[bucket_idx].cells[i].count+=1;
            return 0; 
        }
        else
        {
            uint32_t cell_val=b[bucket_idx].cells[i].count;     
           if(min_val_cell_idx==-1||min_cell_val>cell_val)
           {
                min_cell_val=cell_val;
                min_val_cell_idx=i;
           }
        }
    }
    if(is_inserted==0&&min_val_cell_idx!=-1)
    {
        if (min_cell_val==0||(rand() < RAND_MAX/min_cell_val))
        {
            
            b[bucket_idx].cells[min_val_cell_idx].key=key;
            b[bucket_idx].cells[min_val_cell_idx].level=level;
        }
    }
    return 0;
}
// void insert(BUCKET *b, int bucket_num, uint32_t key,int threshold)
// {
//     int hash_level = 0;
//     hash_level=rand()%(DEPTH);
//     for(int k=0;k<D;k++)
//     { 
//         uint64_t x = key & masks[hash_level];
//         x=(x<<32)+hash_level;
//         uint32_t hash_value = 0;
//         uint32_t bucket_idx=MurmurHash2(&x, 8, SEED);
//         bucket_idx=hash_value%bucket_num; 
//         int min_val_cell_idx = -1;
//         int empty_cell_idx = -1;
//         int has_empty_cell = 0;
//         int is_inserted=0;
//         uint32_t min_cell_val=UINT32_MAX;
//         if(b[bucket_idx].empty_cell>0)
//         {
//             b[bucket_idx].empty_cell-=1;
//             is_inserted=1;
//             empty_cell_idx=b[bucket_idx].empty_cell;
//             b[bucket_idx].cells[empty_cell_idx].key = key;
//             b[bucket_idx].cells[empty_cell_idx].level = level;
//             b[bucket_idx].cells[empty_cell_idx].count = 1;
//             return 0;
//         }
//         for (int i = 0; i < CELL_NUM; i++)
//         {
//             if ((b + bucket_idx)->cells[i].level==hash_level&&((b + bucket_idx)->cells[i].key&masks[hash_level])== (x>>32))
//             {
//                 is_inserted=1;
//                 (b + bucket_idx)->cells[i].count+=1；
//                 break; 
//             }
//             else
//             {
//                 uint32_t cell_val=0;
//                 if(min_val_cell_idx==-1||cell_val<min_cell_val)
//                 {
//                     min_val_cell_idx=i;
//                     min_cell_val=cell_val;
//                 }
//             }
//         }
//         if(is_inserted==0&&has_empty_cell==1)
//         {
//             (b + bucket_idx)->cells[empty_cell_idx].key = key;
//             (b + bucket_idx)->cells[empty_cell_idx].level = hash_level;
//             (b + bucket_idx)->cells[empty_cell_idx].count = 1;
//             (b + bucket_idx)->cells[empty_cell_idx].is_empty=0;
//             (b + bucket_idx)->cells[empty_cell_idx].flag=1;
//         }
//         else if (is_inserted==0&&has_empty_cell == 0)
//         {
// //             if (rand()* pow(1.08,min_cell_val) < RAND_MAX)
// //             {
// //                 (b + bucket_idx)->cells[min_val_cell_idx].count-=1;
// //                 if(min_cell_val<=1)
// //                 {
// //                     (b + bucket_idx)->cells[min_val_cell_idx].count=1;
// //                     (b + bucket_idx)->cells[min_val_cell_idx].key=key;
// //                     (b + bucket_idx)->cells[min_val_cell_idx].level=hash_level;
// //                     (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
// //                     (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
// //                 }
// //             }
//             if (min_cell_val==0||(rand() < RAND_MAX/min_cell_val))
//             {
//                 if((b + bucket_idx)->cells[min_val_cell_idx].level==1)
//                 {
//                     if(min_cell_val>=threshold/4)
//                     {
//                         return 0;
//                     }
//                 }
//                 b[bucket_idx].cells[min_val_cell_idx].key=key;
//                 b[bucket_idx].cells[min_val_cell_idx].level=level;
//             }
//         }
//         hash_level=(hash_level+1)%DEPTH;
//     }
// }
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH);
    uint64_t x=0;
    x=key&masks[hash_level];
    x=(x<<32)+hash_level;
    uint32_t bucket_idx=MurmurHash2(&x, 8, SEED);
    bucket_idx=bucket_idx%BUCKET_NUM;
    find_cell(b,bucket_idx,key,hash_level,threshold);     
}
std::vector<std::pair<uint64_t,uint32_t>> query_test(BUCKET *b,int bucket_num,int threshold)
{

    std::unordered_map<u_int64_t,u_int32_t> flow_size;
    std::vector<std::pair<uint64_t,uint32_t>> HHH;
    std::unordered_set<u_int64_t> nearest;
    std::vector<std::vector<uint32_t>> level_cell_idx(DEPTH);
    for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            if((b+i)->cells[j].count<threshold)
            {
                continue;
            }
            int level=(b+i)->cells[j].level;
            level_cell_idx[level].push_back((i*CELL_NUM+j)*3);
        }
    }
    for(int i=0;i<DEPTH;i++)
    {
        size_t j=0;
        for(j=0;j<level_cell_idx[i].size();j++)
        {
            int cell_idx=level_cell_idx[i][j];
            int bucket_idx=cell_idx/3/CELL_NUM;
            int in_bucket_cell_idx=(cell_idx/3)%CELL_NUM;
            uint64_t key=(b+bucket_idx)->cells[in_bucket_cell_idx].key&masks[i];
            uint32_t size=(b+bucket_idx)->cells[in_bucket_cell_idx].count;
            key=(key<<32)+i;
            flow_size[key]=std::max(flow_size[key],size);
            uint32_t conditional_estimation=flow_size[key];
            std::unordered_set<uint64_t> toerase;
            for(auto y:nearest)
            {
                uint32_t tmp=y>>32;
                if((tmp&masks[i])==(key>>32))
                {
                    if(conditional_estimation>flow_size[y]) conditional_estimation-=flow_size[y];
                    else conditional_estimation=0;
                    toerase.insert(y);
                }
            }
            if(conditional_estimation>threshold)
            {
                HHH.push_back(std::make_pair(key,flow_size[key]*DEPTH));
                for(auto y:toerase)
                {
                    nearest.erase(y);
                }
                nearest.insert(key);    
            }
            
        }
    }
    return HHH;
}