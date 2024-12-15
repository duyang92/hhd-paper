#include "our.h"
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
        for (int j = 0; j < CELL_NUM; j++)
        {
            (b + i)->cells[j].count = 0;
            (b + i)->cells[j].key = 0;
            (b + i)->cells[j].level = 0;
            (b+i)->cells[j].is_empty=1;
            (b + i)->cells[j].flag = 0; 
            (b + i)->cells[j].heavy_child_size = 0;
            (b+i)->cells[j].replace_time=0;
        }
    }
}
uint32_t find_cell(BUCKET *b, int bucket_idx, uint32_t key,uint32_t level,int threshold,int i,uint32_t passed_val,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &decay_other,std::unordered_map<uint64_t,uint32_t> &decayed_time,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time)
{
    int min_val_cell_idx = -1;
    int empty_cell_idx = -1;
    int has_empty_cell = 0;
    int is_inserted=0;
    uint32_t tmp=0;
    uint32_t min_cell_val=UINT32_MAX;    
    uint32_t masked_key=key&masks[level];
    for (int i = 0; i < CELL_NUM; i++)
    {
        if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level&&((b + bucket_idx)->cells[i].key&masks[level])== masked_key)
        {
            is_inserted=1;
            (b + bucket_idx)->cells[i].count+=1;
            if(level==DEPTH-1)
            {
                break;
            }
           (b + bucket_idx)->cells[i].heavy_child_size=passed_val; 
            break; 
        }
        else if ((b + bucket_idx)->cells[i].is_empty)
        {
            has_empty_cell = 1;
            empty_cell_idx=i;
        }
        else
        {
            uint32_t to_subtract=b[bucket_idx].cells[i].heavy_child_size;
            uint32_t cell_val=b[bucket_idx].cells[i].count;
            int flag=0;
            if(cell_val<to_subtract)
            {
                flag=1;
            }
            if((cell_val-to_subtract)*(DEPTH)/D>=threshold)
            {
                flag=1;
            }
            
            if(!flag)
            {
                cell_val-=to_subtract;
           }
           if(min_val_cell_idx==-1||min_cell_val>cell_val)
           {
                min_cell_val=cell_val;
                min_val_cell_idx=i;
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

        if (min_cell_val==0||(min_cell_val*(DEPTH)/D<threshold&&rand() < RAND_MAX/min_cell_val))
        {
        
            (b + bucket_idx)->cells[min_val_cell_idx].key=key;
            (b + bucket_idx)->cells[min_val_cell_idx].level=level;
            (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
            // (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
            // (b + bucket_idx)->cells[min_val_cell_idx].replace_time++;
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=0;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=0;
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=min_cell_val/2;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=min_cell_val/2;
        } 
    }
    return tmp;
}
uint32_t find_cell(BUCKET *b, int bucket_idx, uint32_t key,uint32_t level,int threshold,std::unordered_set<uint64_t> &false_HHH,int passed_val,uint32_t &judge_time)
{
    int min_val_cell_idx = -1;
    int empty_cell_idx = -1;
    int has_empty_cell = 0;
    int is_inserted=0;
    uint32_t tmp=0;
    uint32_t min_cell_val=UINT32_MAX;    
    uint32_t masked_key=key&masks[level];
    for (int i = 0; i < CELL_NUM; i++)
    {
        if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level&&((b + bucket_idx)->cells[i].key&masks[level])== masked_key)
        {
            is_inserted=1;
            (b + bucket_idx)->cells[i].count+=1;
            if(level==DEPTH-1)
            {
                break;
            }
           (b + bucket_idx)->cells[i].heavy_child_size+=passed_val;
           if((b + bucket_idx)->cells[i].count>(b + bucket_idx)->cells[i].heavy_child_size&&((b + bucket_idx)->cells[i].count-(b + bucket_idx)->cells[i].heavy_child_size)*(DEPTH)/D>=threshold)
           {
                if(((b + bucket_idx)->cells[i].count-passed_val)*(DEPTH)/D>=threshold)
                {
                    if((b + bucket_idx)->cells[i].flag)
                    {
                        tmp=1;
                    }
                    else
                    {
                        tmp=(b + bucket_idx)->cells[i].count-(b + bucket_idx)->cells[i].heavy_child_size;
                        (b + bucket_idx)->cells[i].flag=1; 
                    }
                }
           } 
           else
           {
                if((b + bucket_idx)->cells[i].flag)
                {
                    tmp=(b + bucket_idx)->cells[i].heavy_child_size-(b + bucket_idx)->cells[i].count;
                    (b + bucket_idx)->cells[i].flag=0; 
                }
                else
                {
                    tmp=1; 
                }
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
            uint32_t to_subtract=b[bucket_idx].cells[i].heavy_child_size;
            uint32_t cell_val=b[bucket_idx].cells[i].count;
            int flag=0;
            if(cell_val<to_subtract)
            {
                flag=1;
            }
            if((cell_val-to_subtract)*(DEPTH)/D>=threshold)
            {
                flag=1;
            }
            
            if(!flag)
            {
                cell_val-=to_subtract;
           }
           if(min_val_cell_idx==-1||min_cell_val>cell_val)
           {
                min_cell_val=cell_val;
                min_val_cell_idx=i;
           }
        }
    }
    if(is_inserted==0&&has_empty_cell==1)
    {
        (b + bucket_idx)->cells[empty_cell_idx].key = key;
        (b + bucket_idx)->cells[empty_cell_idx].level = level;
        (b + bucket_idx)->cells[empty_cell_idx].count = 1;
        (b + bucket_idx)->cells[empty_cell_idx].is_empty=0;
        (b + bucket_idx)->cells[empty_cell_idx].flag=0;
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

        if (min_cell_val==0||(min_cell_val*(DEPTH)/D<threshold&&rand() < RAND_MAX/min_cell_val))
        {
        
            (b + bucket_idx)->cells[min_val_cell_idx].key=key;
            (b + bucket_idx)->cells[min_val_cell_idx].level=level;
            (b + bucket_idx)->cells[min_val_cell_idx].flag=0;
            (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
            // (b + bucket_idx)->cells[min_val_cell_idx].replace_time++;
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=0;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=0;
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=min_cell_val/2;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=min_cell_val/2;
        } 
    }
    return tmp;
}

void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &decay_other,std::unordered_map<uint64_t,uint32_t> &decayed_time,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH+1);
    int tmp=0;
    int high_level=hash_level-hash_level%2+1;
    if(high_level>=DEPTH)
    {
        high_level=DEPTH-1;
    }
    int round=D/2;
    uint32_t passed_val=0;
    for(int i=0;i<round;i++)
    {
        uint64_t x = key & masks[high_level];
        x=(x<<32)+high_level;
        uint32_t hash_value = 0;
        hash_value=MurmurHash2(&x, 8,SEED1);
        uint32_t bucket_idx=hash_value%bucket_num;
        if(query_keys.find(x)!=query_keys.end())
        {
            insert_time[x]++;
        }
        if(high_level<DEPTH-1)
        {
            uint64_t x1=key&masks[high_level-1];
            x1=(x1<<32)+(high_level-1);
            if(query_keys.find(x1)!=query_keys.end())
            {
                insert_time[x1]++;
            }
        }
        passed_val=find_cell(b,bucket_idx,key,high_level,threshold,i,passed_val,query_keys,decay_other,decayed_time,replaced_time,insert_time);
        if(high_level==DEPTH-1)
        {
            high_level=1;
            passed_val=0;
        }
        else
        {
            high_level+=2;
            if(high_level>=DEPTH)
            {
                high_level=DEPTH-1;
            }
        }
        // if(high_level==DEPTH-1)
        // {
        //     high_level--;
        // }
        // else
        // {
        //     high_level-=2;
        //     if(high_level<0)
        //     {
        //         high_level=DEPTH-1;
        //     }
        // }  
    }
}
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH);
    int passed_val=0;
    uint32_t block_idx=0;
    int block_num=bucket_num/BLOCK_SIZE;
    int last_block_size=0;
    if(block_num*BLOCK_SIZE<bucket_num)
    {
        block_num++;
        last_block_size=bucket_num-block_num*BLOCK_SIZE;
    }
    int block_idx_level=hash_level-hash_level%BLOCK_LEVEL+BLOCK_LEVEL-1;
    if(block_idx_level>=DEPTH)
    {
        block_idx_level=DEPTH-1;
    }
    uint64_t block_key=key&masks[block_idx_level];
    block_key=(block_key<<32)+block_idx_level;
    MurmurHash3_x86_32(&block_key, 8, SEED, &block_idx);
    block_idx=block_idx%block_num;
    for(int i=0;i<D;i++)
    {
        uint64_t x=0;
        x=key&masks[hash_level];
        x=(x<<32)+hash_level;
        // int new_block_idx_level=hash_level-hash_level%BLOCK_LEVEL+BLOCK_LEVEL-1;
        // if(new_block_idx_level>=DEPTH)
        // {
        //     new_block_idx_level=DEPTH-1;
        // }
        // if(new_block_idx_level!=block_idx_level)
        // {
        //     block_idx_level=new_block_idx_level;
        //     block_key=key&masks[block_idx_level];
        //     block_key=(block_key<<32)+block_idx_level;
        //     MurmurHash3_x86_32(&block_key, 8, SEED, &block_idx);
        // //     if(x==2225341165874446353)
        // // {
        // //     std::cout<<" "<<block_key<<" "<<block_idx<<std::endl;
        // //     continue;

        // // }
        // block_idx=block_idx%block_num;
        // }
        
        uint32_t in_block_idx=0;
        MurmurHash3_x86_32(&x, 8, SEED, &in_block_idx);
        // uint32_t bucket_idx=block_idx*BLOCK_SIZE+in_block_idx%BLOCK_SIZE;
        uint32_t bucket_idx=0;
        MurmurHash3_x86_32(&x, 8, SEED, &bucket_idx);
        bucket_idx=bucket_idx%bucket_num;   
        passed_val=find_cell(b,bucket_idx,key,hash_level,threshold,false_HHH,passed_val,judge_time);
        if(hash_level==DEPTH-1)
        {
            hash_level=0;
            passed_val=0;
        }
        else
        {
            hash_level+=1;
        }
        // if(high_level==DEPTH-1)
        // {
        //     high_level--;
        // }
        // else
        // {
        //     high_level-=2;
        //     if(high_level<0)
        //     {
        //         high_level=DEPTH-1;
        //     }
        // }  
    } 
}
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH+1);
    int tmp=0;
    int high_level=hash_level-hash_level%2+1;
    if(high_level>=DEPTH)
    {
        high_level=DEPTH-1;
    }
    int round=D/2;
    uint32_t passed_val=0;
    for(int i=0;i<round;i++)
    {
        uint64_t x = key & masks[high_level];
        x=(x<<32)+high_level;
        uint32_t hash_value = 0;
        hash_value=MurmurHash2(&x, 8,SEED1);
        uint32_t bucket_idx=hash_value%bucket_num;
        // if(query_keys.find(x)!=query_keys.end())
        // {
        //     insert_time[x]++;
        // }
        if(high_level<DEPTH-1)
        {
            uint64_t x1=key&masks[high_level-1];
            x1=(x1<<32)+(high_level-1);
            // if(query_keys.find(x1)!=query_keys.end())
            // {
            //     insert_time[x1]++;
            // }
        }
        // passed_val=find_cell(b,bucket_idx,key,high_level,threshold,i,passed_val,query_keys,decay_other,decayed_time,replaced_time,insert_time);
        if(high_level==DEPTH-1)
        {
            high_level=1;
            passed_val=0;
        }
        else
        {
            high_level+=2;
            if(high_level>=DEPTH)
            {
                high_level=DEPTH-1;
            }
        }
        // if(high_level==DEPTH-1)
        // {
        //     high_level--;
        // }
        // else
        // {
        //     high_level-=2;
        //     if(high_level<0)
        //     {
        //         high_level=DEPTH-1;
        //     }
        // }  
    }
}

std::vector<std::pair<uint64_t,uint32_t>> query_test(BUCKET *b,int bucket_num,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &est_size,std::unordered_map<uint64_t,uint32_t> &est_concnt)
{

    std::unordered_map<u_int64_t,u_int32_t> flow_size;
    std::vector<std::pair<uint64_t,uint32_t>> HHH;
    std::unordered_set<u_int64_t> nearest;
    std::vector<std::vector<uint32_t>> level_cell_idx(DEPTH);
    for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            if((b+i)->cells[j].is_empty||(b+i)->cells[j].count*(DEPTH)/D<threshold)
            {
                continue;
            }
            int level=(b+i)->cells[j].level;
            level_cell_idx[level].push_back((i*CELL_NUM+j));
        }
    }
    for(int i=0;i<DEPTH;i++)
    {
        size_t j=0;
        for(j=0;j<level_cell_idx[i].size();j++)
        {
            int cell_idx=level_cell_idx[i][j];
            int bucket_idx=cell_idx/CELL_NUM;
            int in_bucket_cell_idx=(cell_idx)%CELL_NUM;
            uint64_t key=(b+bucket_idx)->cells[in_bucket_cell_idx].key&masks[i];
            uint32_t size=(b+bucket_idx)->cells[in_bucket_cell_idx].count;
            key=(key<<32)+i;
            size=size*(DEPTH)/D;
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