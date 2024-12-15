#include "our_2D.h"
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
void init(BUCKET *b, int bucket_num)
{
    for (int i = 0; i < bucket_num; i++)
    {
        for (int j = 0; j < CELL_NUM; j++)
        {
            (b + i)->cells[j].count = 0;
            (b + i)->cells[j].key = 0;
            (b + i)->cells[j].level = 0;
            (b+i)->cells[j].is_empty=1;
            (b + i)->cells[j].flag = 1; 
            (b + i)->cells[j].heavy_child_size = 0; 
        }
    }
}
void insert(BUCKET *b, int bucket_num, uint64_t key,int threshold)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH);
    __uint128_t src=key>>32;
    __uint128_t dst=key&0xffffffff;
    for(int i=0;i<=hash_level;i++)
    {
        uint64_t src_level=i;
        uint64_t dst_level=hash_level-i;
        if(src_level>MAX_LEVEL||dst_level>MAX_LEVEL)
        {
            continue;
        }
        src=src&masks[src_level];
        dst=dst&masks[dst_level];
        uint64_t level=(src_level<<32)+dst_level;
        __uint128_t x=(((src<<32)+dst)<<64)+level;
        uint64_t hash_value = 0;
        uint32_t bucket_idx=0;
        MurmurHash3_x64_128(&x, 16, SEED, &hash_value);
        bucket_idx=hash_value%bucket_num; 
        int min_val_cell_idx = -1;
        int empty_cell_idx = -1;
        int has_empty_cell = 0;
        int is_inserted=0;
        uint32_t min_cell_val=UINT32_MAX;
        for (int i = 0; i < CELL_NUM; i++)
        {
            if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level&&((b + bucket_idx)->cells[i].key)== (x>>64))
            {
                is_inserted=1;
                (b + bucket_idx)->cells[i].count+=1;
                break; 
            }
            else if ((b + bucket_idx)->cells[i].is_empty)
            {
                has_empty_cell = 1;
                empty_cell_idx=i;
            }
            else
            {
                uint32_t cell_val=0;
                if((b + bucket_idx)->cells[i].count>(b + bucket_idx)->cells[i].heavy_child_size)
                {
                    cell_val=(b + bucket_idx)->cells[i].count-(b + bucket_idx)->cells[i].heavy_child_size;
                }
                if(min_val_cell_idx==-1||cell_val<min_cell_val)
                {
                    min_val_cell_idx=i;
                    min_cell_val=cell_val;
                }
            }
        }
        if(is_inserted==0&&has_empty_cell==1)
        {
            (b + bucket_idx)->cells[empty_cell_idx].key = x>>64;
            (b + bucket_idx)->cells[empty_cell_idx].level = level;
            (b + bucket_idx)->cells[empty_cell_idx].count = 1;
            (b + bucket_idx)->cells[empty_cell_idx].is_empty=0;
            (b + bucket_idx)->cells[empty_cell_idx].flag=1;
        }
        else if (is_inserted==0&&has_empty_cell == 0)
        {
            if (rand()* pow(1.08,min_cell_val) < RAND_MAX)
            {
                (b + bucket_idx)->cells[min_val_cell_idx].count-=1;
                if(min_cell_val<=1)
                {
                    (b + bucket_idx)->cells[min_val_cell_idx].count=1;
                    (b + bucket_idx)->cells[min_val_cell_idx].key=x>>64;
                    (b + bucket_idx)->cells[min_val_cell_idx].level=level;
                    (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
                    (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
                }
            }
        }
    }
}
void insert_bit(BUCKET *b, int bucket_num, uint64_t key,int threshold)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH);
    uint32_t high_level=hash_level-hash_level%D+(D-1);
    if(high_level>=DEPTH)
    {
        high_level=DEPTH-1;
    }
    uint32_t hash_value = 0;
    uint64_t x=key&masks[high_level];
    x=(x<<32)+high_level;
    MurmurHash3_x86_32(&x, 8, SEED, &hash_value);
    uint32_t bucket_idx = hash_value % bucket_num;
    int is_inserted=0;
    if((b+bucket_idx)->cells[0].is_empty)
    {
        (b+bucket_idx)->cells[0].key=key&masks[high_level];
        (b+bucket_idx)->cells[0].level=high_level;
        (b+bucket_idx)->cells[0].count=1;
        (b+bucket_idx)->cells[0].is_empty=0;
        is_inserted=1;
    }
    else
    {
        uint64_t bucket_key=(b+bucket_idx)->cells[0].key;
        bucket_key=(bucket_key<<32)+(b+bucket_idx)->cells[0].level;
        if(bucket_key==x)
        {
            (b+bucket_idx)->cells[0].count++;
            is_inserted=1;
        }
        else
        {
            uint32_t count=(b+bucket_idx)->cells[0].count;
            if(rand()*pow(1.08,count)<RAND_MAX)
            {
                (b+bucket_idx)->cells[0].count--;
                if((b+bucket_idx)->cells[0].count==0)
                {
                    (b+bucket_idx)->cells[0].key=key&masks[high_level];
                    (b+bucket_idx)->cells[0].level=high_level;
                    (b+bucket_idx)->cells[0].count=1;
                    (b+bucket_idx)->cells[0].is_empty=0;
                    for(int i=1;i<CELL_NUM;i++)
                    {
                        (b+bucket_idx)->cells[i].key=0;
                        (b+bucket_idx)->cells[i].level=0;
                        (b+bucket_idx)->cells[i].count=0;
                        (b+bucket_idx)->cells[i].is_empty=0;
                    }
                    is_inserted=1; 
                }
            }
        }
    }
    if(is_inserted&&high_level<DEPTH-1)
    {
        int cell_idx=0;
        for(int i=1;i<D;i++)
        {   
            uint32_t tmp_key=key&masks[high_level-i];
            if(tmp_key&(1<<(high_level-i)))
            {
                cell_idx=cell_idx*2+2;
            }
            else
            {
                cell_idx=cell_idx*2+1;
            }
            (b+bucket_idx)->cells[cell_idx].key=tmp_key;
            (b+bucket_idx)->cells[cell_idx].level=high_level-i;
            (b+bucket_idx)->cells[cell_idx].count++;
            (b+bucket_idx)->cells[cell_idx].is_empty=0; 
        }
    }
}
std::vector<std::pair<__uint128_t,uint32_t>> query(BUCKET* b,int bucket_num,uint32_t threshold)
{

    std::unordered_map<__uint128_t,uint32_t> flow_size;
    std::vector<std::pair<__uint128_t,uint32_t>> HHH;
    std::unordered_set<__uint128_t> nearest;
    std::vector<std::vector<uint64_t>> level_cell_idx(DEPTH);
    for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            if((b+i)->cells[j].is_empty)
            {
                continue;
            }
            uint64_t level=(b+i)->cells[j].level;
            uint64_t src_level=level>>32;
            uint64_t dst_level=level&0xffffffff;
            level_cell_idx[src_level+dst_level].push_back(i*CELL_NUM+j);
        }
    }
    for(int i=0;i<DEPTH;i++)
    {
        for(int j=0;j<level_cell_idx[i].size();j++)
        {
            int cell_idx=level_cell_idx[i][j];
            int bucket_idx=cell_idx/CELL_NUM;
            int in_bucket_cell_idx=cell_idx%CELL_NUM;
            uint64_t key=(b+bucket_idx)->cells[in_bucket_cell_idx].key;
            uint64_t level=(b+bucket_idx)->cells[in_bucket_cell_idx].level;
            uint64_t src_level=level>>32;
            uint64_t dst_level=level&0xffffffff;
            __uint128_t flow_id=key;
            flow_id=(flow_id<<64)+level;
            flow_size[flow_id]=(b+bucket_idx)->cells[in_bucket_cell_idx].count*DEPTH/D;
            uint32_t conditional_estimation=flow_size[flow_id];
            std::unordered_set<__uint128_t> toerase;
            for(auto y:nearest)
            {
                uint64_t tmp=y>>64;
                uint64_t src=tmp>>32;
                uint64_t dst=tmp&0xffffffff;
                uint64_t src_level1=(y&0xffffffffffffffff)>>32;
                uint64_t dst_level1=(y&0xffffffffffffffff)&0xffffffff;
                src=src&masks[src_level1];
                dst=dst&masks[dst_level1];
                if(src_level1<=src_level&&dst_level1<=dst_level&&tmp==key)
                {
                    if(conditional_estimation>flow_size[y]) conditional_estimation-=flow_size[y];
                    else conditional_estimation=0;
                    toerase.insert(y);
                }
            }
            if(conditional_estimation>threshold)
            {
                HHH.push_back(std::make_pair(flow_id,flow_size[flow_id]));
                for(auto y:toerase)
                {
                    nearest.erase(y);
                }
                nearest.insert(flow_id);    
            }
            
        }
    }
    return HHH;
}