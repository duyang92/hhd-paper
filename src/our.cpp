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
void insert(BUCKET *b, int bucket_num, uint32_t key,int threshold)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH);
    for(int k=0;k<D;k++)
    { 
        uint64_t x = key & masks[hash_level];
        x=(x<<32)+hash_level;
        uint32_t hash_value = 0;
        uint32_t bucket_idx=0;
        MurmurHash3_x86_32(&x, 8, SEED, &hash_value);
        // uint32_t x1=0;
        // int is_heavy_child_in_bucket=0;
        // uint32_t heavy_child_size=0;
        // if(hash_level>0)
        // {
        //     x1=key&masks[hash_level-1];
        //     uint32_t hash_value1=0;
        //     MurmurHash3_x86_32(&x1, 4, SEED, &hash_value1); 
        //     uint32_t child_bucket_idx=hash_value1%bucket_num;
        //     for(int i=0;i<CELL_NUM;i++)
        //     {
        //         if((b+child_bucket_idx)->cells[i].is_empty==0&&(b+child_bucket_idx)->cells[i].level==hash_level-1&&((b+child_bucket_idx)->cells[i].key&masks[hash_level-1])==x1)
        //         {
        //             if((b+child_bucket_idx)->cells[i].count*DEPTH>=threshold)
        //             {
        //                 is_heavy_child_in_bucket=1;
        //                 heavy_child_size=(b+child_bucket_idx)->cells[i].count*DEPTH;
        //             }
        //         }
        //     }
        // }
        bucket_idx=hash_value%bucket_num; 
        int min_val_cell_idx = -1;
        int empty_cell_idx = -1;
        int has_empty_cell = 0;
        int is_inserted=0;
        uint32_t min_cell_val=UINT32_MAX;
        for (int i = 0; i < CELL_NUM; i++)
        {
            if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==hash_level&&((b + bucket_idx)->cells[i].key&masks[hash_level])== (x>>32))
            {
                is_inserted=1;
                (b + bucket_idx)->cells[i].count+=1;
                // uint32_t c=(b + bucket_idx)->cells[i].count;
                // uint32_t h=(b + bucket_idx)->cells[i].heavy_child_size;
                // uint32_t tmp1=0;uint32_t tmp2=1;
                // if((b+bucket_idx)->cells[i].flag==1&&(c<=h&&(c-h)*DEPTH<threshold))
                // {
                //     break;
                // }
                // if((b+bucket_idx)->cells[i].flag==0&&(c<h||(c-h)*DEPTH<threshold))
                // {
                //     (b+bucket_idx)->cells[i].flag=1;
                //     tmp1=c;tmp2=h;
                // }
                // else if((b+bucket_idx)->cells[i].flag==1&&(c>=h&&(c-h)*DEPTH>=threshold))
                // {
                //     (b+bucket_idx)->cells[i].flag=0;
                //     tmp1=h;tmp2=c;
                // }
                // for(int j=hash_level+1;j<DEPTH;j++)
                // {
                //     uint32_t par_key=key&masks[j];
                //     uint32_t cell_idx=find_cell(b,bucket_num,par_key,j);
                //     if(cell_idx<bucket_num*CELL_NUM)
                //     {
                //         if((b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size<tmp1)
                //         {
                //             (b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size=0;
                //         }
                //         else
                //         {
                //             (b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size-=tmp1;
                //         }
                //         (b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size+=tmp2;
                //         if((b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].flag==0)
                //         {
                //             break;
                //         }
                //     }
                // }
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
            (b + bucket_idx)->cells[empty_cell_idx].level = hash_level;
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
                    (b + bucket_idx)->cells[min_val_cell_idx].key=key;
                    (b + bucket_idx)->cells[min_val_cell_idx].level=hash_level;
                    (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
                    (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
                }
            }
        }
        hash_level=(hash_level+1)%DEPTH;
    }
}
uint32_t find_cell(BUCKET *b, int bucket_num, uint32_t key,uint32_t level)
{
    uint32_t hash_value = 0;
    uint64_t x=key;
    x=(x<<32)+level;
    MurmurHash3_x86_32(&x, 8, SEED, &hash_value);
    uint32_t bucket_idx = hash_value % bucket_num;
    for (int i = 0; i < CELL_NUM; i++)
    {
        if (((b + bucket_idx)->cells[i].key&masks[level] )== key&&!(b + bucket_idx)->cells[i].is_empty&&(b + bucket_idx)->cells[i].level==level)
        {
            return bucket_idx*CELL_NUM+i;
        }
    }
    return bucket_num*CELL_NUM;
}
void insert_bit(BUCKET *b, int bucket_num, uint32_t key,int threshold)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH);
    uint32_t high_level=hash_level-hash_level%D+(D-1);
    if(high_level>=DEPTH)
    {
        high_level=DEPTH-1;
    }
    for(int k=0;k<D;k++)
    {
        uint64_t x=key&masks[high_level-k];
        x=(x<<32)+(high_level-k); 
        __uint128_t hash_value = 0;
    MurmurHash3_x64_128(&x, 8, SEED, &hash_value);
    uint32_t bucket_idx=hash_value%bucket_num;
        // uint32_t x1=0;
        // int is_heavy_child_in_bucket=0;
        // uint32_t heavy_child_size=0;
        // if(hash_level>0)
        // {
        //     x1=key&masks[hash_level-1];
        //     uint32_t hash_value1=0;
        //     MurmurHash3_x86_32(&x1, 4, SEED, &hash_value1); 
        //     uint32_t child_bucket_idx=hash_value1%bucket_num;
        //     for(int i=0;i<CELL_NUM;i++)
        //     {
        //         if((b+child_bucket_idx)->cells[i].is_empty==0&&(b+child_bucket_idx)->cells[i].level==hash_level-1&&((b+child_bucket_idx)->cells[i].key&masks[hash_level-1])==x1)
        //         {
        //             if((b+child_bucket_idx)->cells[i].count*DEPTH>=threshold)
        //             {
        //                 is_heavy_child_in_bucket=1;
        //                 heavy_child_size=(b+child_bucket_idx)->cells[i].count*DEPTH;
        //             }
        //         }
        //     }
        // }
        int min_val_cell_idx = -1;
        int empty_cell_idx = -1;
        int has_empty_cell = 0;
        int is_inserted=0;
        uint32_t min_cell_val=UINT32_MAX;
        for (int i = 0; i < CELL_NUM; i++)
        {
            if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==high_level-k&&((b + bucket_idx)->cells[i].key&masks[high_level-k])== (x>>32))
            {
                is_inserted=1;
                (b + bucket_idx)->cells[i].count+=1;
                // uint32_t c=(b + bucket_idx)->cells[i].count;
                // uint32_t h=(b + bucket_idx)->cells[i].heavy_child_size;
                // uint32_t tmp1=0;uint32_t tmp2=1;
                // if((b+bucket_idx)->cells[i].flag==1&&(c<=h&&(c-h)*DEPTH<threshold))
                // {
                //     break;
                // }
                // if((b+bucket_idx)->cells[i].flag==0&&(c<h||(c-h)*DEPTH<threshold))
                // {
                //     (b+bucket_idx)->cells[i].flag=1;
                //     tmp1=c;tmp2=h;
                // }
                // else if((b+bucket_idx)->cells[i].flag==1&&(c>=h&&(c-h)*DEPTH>=threshold))
                // {
                //     (b+bucket_idx)->cells[i].flag=0;
                //     tmp1=h;tmp2=c;
                // }
                // for(int j=hash_level+1;j<DEPTH;j++)
                // {
                //     uint32_t par_key=key&masks[j];
                //     uint32_t cell_idx=find_cell(b,bucket_num,par_key,j);
                //     if(cell_idx<bucket_num*CELL_NUM)
                //     {
                //         if((b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size<tmp1)
                //         {
                //             (b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size=0;
                //         }
                //         else
                //         {
                //             (b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size-=tmp1;
                //         }
                //         (b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].heavy_child_size+=tmp2;
                //         if((b+cell_idx/CELL_NUM)->cells[cell_idx%CELL_NUM].flag==0)
                //         {
                //             break;
                //         }
                //     }
                // }
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
            (b + bucket_idx)->cells[empty_cell_idx].level = high_level-k;
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
                    (b + bucket_idx)->cells[min_val_cell_idx].key=key;
                    (b + bucket_idx)->cells[min_val_cell_idx].level=high_level-k;
                    (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
                    (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
                }
            }
        }
        if((high_level-k)%D==0)
        {
            break;
        }
    }
}
std::vector<std::pair<uint64_t,uint32_t>> query(BUCKET* b,int bucket_num,uint32_t threshold)
{

    std::unordered_map<u_int64_t,u_int32_t> flow_size;
    std::vector<std::pair<uint64_t,uint32_t>> HHH;
    std::unordered_set<u_int64_t> nearest;
    std::vector<std::vector<uint32_t>> level_cell_idx(DEPTH);
    for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            if((b+i)->cells[j].is_empty)
            {
                continue;
            }
            int level=(b+i)->cells[j].level;
            level_cell_idx[level].push_back(i*CELL_NUM+j);
        }
    }
    for(int i=0;i<DEPTH;i++)
    {
        for(int j=0;j<level_cell_idx[i].size();j++)
        {
            int cell_idx=level_cell_idx[i][j];
            int bucket_idx=cell_idx/CELL_NUM;
            int in_bucket_cell_idx=cell_idx%CELL_NUM;
            uint64_t key=(b+bucket_idx)->cells[in_bucket_cell_idx].key&masks[i];
            key=(key<<32)+i;
            flow_size[key]=std::max(flow_size[key],(b+bucket_idx)->cells[in_bucket_cell_idx].count*(DEPTH)/D);
            if(i==0)
            {
                for(int k=i+1;k<DEPTH;k++)
                {
                    uint64_t par_key=(b+bucket_idx)->cells[in_bucket_cell_idx].key&masks[k];
                    par_key=(par_key<<32)+k;
                    flow_size[par_key]+=flow_size[key];
                }
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