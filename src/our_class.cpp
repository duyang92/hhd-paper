#include "our_class.h"
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
inline uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed  )
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
BUCKET* our_class::get_b()
{
    return b;

}
int our_class::find_cell(int bucket_idx, uint32_t key,uint32_t level,int threshold)
{
    int min_val_cell_idx = -1;
    int empty_cell_idx = -1;
    int has_empty_cell = 0;
    int is_inserted=0;
    uint32_t min_cell_val=UINT32_MAX;    
    uint32_t masked_key=key&masks[level];
    for (int i = 0; i < CELL_NUM; i++)
    {
        if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level&&((b + bucket_idx)->cells[i].key&masks[level])== masked_key)
        {
            is_inserted=1;
            (b + bucket_idx)->cells[i].count+=1;
            if(key&(1<<(level-1)))
            {
                (b + bucket_idx)->cells[i].right_child_count+=1;
            }
            else
            {
                (b + bucket_idx)->cells[i].left_child_count+=1;
            }
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
        (b + bucket_idx)->cells[empty_cell_idx].key = key;
        (b + bucket_idx)->cells[empty_cell_idx].level = level;
        (b + bucket_idx)->cells[empty_cell_idx].count = 1;
        (b + bucket_idx)->cells[empty_cell_idx].is_empty=0;
        (b + bucket_idx)->cells[empty_cell_idx].flag=1;
    }
    else if (is_inserted==0&&has_empty_cell == 0)
    {
        // if (min_cell_val*DEPTH/D<threshold&&rand()* pow(1.08,min_cell_val) < RAND_MAX)
        // {
        //     (b + bucket_idx)->cells[min_val_cell_idx].count-=1;
        //     uint64_t key1=(b + bucket_idx)->cells[min_val_cell_idx].key;
        //     uint32_t level1=(b + bucket_idx)->cells[min_val_cell_idx].level;
        //     key1=key1&masks[level1];
        //     key1=(key1<<32)+(level1);
        //     if(query_keys.find(key1)!=query_keys.end())
        //     {
        //         decayed_time[key1]++;
        //     }
        //     if(query_keys.find(x)!=query_keys.end())
        //     {
        //         decay_other[x]++;
        //     }
        //     if(min_cell_val<=1)
        //     {
        //         if(query_keys.find(key1)!=query_keys.end())
        //         {
        //             replaced_time[key1]++;
        //         }
        //         (b + bucket_idx)->cells[min_val_cell_idx].count=1;
        //         (b + bucket_idx)->cells[min_val_cell_idx].key=key;
        //         (b + bucket_idx)->cells[min_val_cell_idx].level=high_level-k;
        //         (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
        //         (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
        //     }
        // }
        
        if (min_cell_val*DEPTH/D<threshold&&gen() < UINT_FAST64_MAX/min_cell_val)
        {
            uint64_t key1=(b + bucket_idx)->cells[min_val_cell_idx].key;
//                 if(query_keys.find(key1)!=query_keys.end())
//                 {
//                     replaced_time[key1]++;
//                     std::cout<<x<<" "<<min_cell_val<<std::endl;
//                 }
            (b + bucket_idx)->cells[min_val_cell_idx].key=key;
            (b + bucket_idx)->cells[min_val_cell_idx].level=level;
            (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
            (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
            (b + bucket_idx)->cells[min_val_cell_idx].replace_time++;
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=0;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=0;
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=min_cell_val/2;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=min_cell_val/2;
        } 
    }
    return 0;
}
void our_class::init()
{
    for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            b[i].cells[j].is_empty=1;
            b[i].cells[j].flag=0;
        }
    }

}
our_class::our_class(int bucket_num, int threshold)
{
    b = (BUCKET *)malloc(bucket_num * sizeof(BUCKET));
    this->bucket_num=bucket_num;
    this->threshold=threshold;
    init();
}

void our_class::insert(int bucket_num, uint32_t key,int threshold)
{
    int hash_level = 0;
    hash_level=gen()%(DEPTH);
    int high_level=hash_level-hash_level%2+1;
    if(high_level>=DEPTH)
    {
        high_level=DEPTH-1;
    }
    int round=D/2;
    for(int i=0;i<round;i++)
    {
        uint64_t x = key & masks[high_level];
        x=(x<<32)+high_level;
        uint32_t hash_value = 0;
        MurmurHash3_x86_32(&x, 8, SEED, &hash_value);
        uint32_t bucket_idx=hash_value%bucket_num;
    //     if(query_keys.find(x)!=query_keys.end())
    //     {
    //         insert_time[x]++;
    //     }
        find_cell(bucket_idx,key,high_level,threshold);
        if(high_level==DEPTH-1)
        {
            high_level--;
        }
        else
        {
            high_level-=2;
            if(high_level<0)
            {
                high_level=DEPTH-1;
            }
        } 
    }
}
std::vector<std::pair<uint64_t,uint32_t>> our_class::query(int bucket_num,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &est_size,std::unordered_map<uint64_t,uint32_t> &est_concnt)
{
    std::unordered_map<u_int64_t,u_int32_t> flow_size;
    std::vector<std::pair<uint64_t,uint32_t>> HHH;
    std::unordered_set<u_int64_t> nearest;
    std::vector<std::vector<uint32_t>> level_cell_idx(DEPTH);
    for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            if((b+i)->cells[j].is_empty||(b+i)->cells[j].count*DEPTH/D<threshold)
            {
                continue;
            }
            int level=(b+i)->cells[j].level;
            level_cell_idx[level].push_back((i*CELL_NUM+j)*3);
            if((b+i)->cells[j].left_child_count*DEPTH/D>=threshold)
            {
                level_cell_idx[level-1].push_back((i*CELL_NUM+j)*3+1);
            }
            if((b+i)->cells[j].right_child_count*DEPTH/D>=threshold)
            {
                level_cell_idx[level-1].push_back((i*CELL_NUM+j)*3+2);
            } 
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
            if(i%2==0&&cell_idx%3==1)
            {
                key=key&(~(1<<i));
                size=(b+bucket_idx)->cells[in_bucket_cell_idx].left_child_count;
            }
            else if(i%2==0&&cell_idx%3==2)
            {
                key=key|(1<<i); 
                size=(b+bucket_idx)->cells[in_bucket_cell_idx].right_child_count;
            }
            key=(key<<32)+i;
            size=size*DEPTH/D;
            flow_size[key]=std::max(flow_size[key],size);
            if(query_keys.find(key)!=query_keys.end())
            {
                est_size[key]=flow_size[key];
            }
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
            if(query_keys.find(key)!=query_keys.end())
            {
                est_concnt[key]=conditional_estimation;
            }
            if(conditional_estimation>threshold)
            {
                HHH.push_back(std::make_pair(key,flow_size[key]));
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