#include "our_C4.h"
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
#include<queue>
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
            for(int k=0;k<(1<<(L-1))-1;k++)
            {
                (b+i)->cells[j].subcounts[k]=0;
            }
//             (b+i)->cells[j].subcounts[0]=0;
            b[i].cells[j].right_child_count=0;
        }
    }
}
uint32_t find_cell(BUCKET *b, int bucket_idx, uint32_t key,int level,int threshold)
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
        int subcountidx=0;
//         if(key&(1<<(level-1)))
//         {
//             b[bucket_idx].cells[empty_cell_idx].right_child_count=1;
//         }
        for(int i=level-1;i>level-L;i--)
        {
            if(key&(1<<i))
            {
           
                b[bucket_idx].cells[empty_cell_idx].subcounts[subcountidx]=1;
                subcountidx=2*subcountidx+2;
            }
            else
            {
                subcountidx=2*subcountidx+1;
            }
        }
        b[bucket_idx].cells[empty_cell_idx].count = 1;
        return 0;
    }
    for (int i = 0; i < CELL_NUM; i++)
    {
        if (b[bucket_idx].cells[i].level==level&&(b[bucket_idx].cells[i].key&masks[level])== masked_key)
        {
            is_inserted=1;
            b[bucket_idx].cells[i].count+=1;
            int subcountidx=0;
//             if(key&(1<<(level-1)))
//         {
//             b[bucket_idx].cells[i].right_child_count+=1;
//         }
            for(int j=level-1;j>level-L;j--)
            {
                if(key&(1<<j))
                {
                    b[bucket_idx].cells[i].subcounts[subcountidx]+=1;
                    subcountidx=2*subcountidx+2;
                }
                else
                {
                    subcountidx=2*subcountidx+1;
                }
            }
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
//             memset(b[bucket_idx].cells[min_val_cell_idx].subcounts,0,sizeof(b[bucket_idx].cells[min_val_cell_idx].subcounts));
        }
    }
    return 0;
}

void insert_bit(BUCKET *b, int bucket_num, uint32_t key,int threshold)
{
    int hash_level = 0;
    hash_level=rand()%(36);
    int high_level=hash_level-hash_level%L+L-1;
    if(high_level>=DEPTH)
    {
        high_level=DEPTH-1;
    }
    #ifdef USEBLOCK
    uint32_t block_idx=0;
    int block_idx_level=high_level-high_level%BLOCK_LEVEL+BLOCK_LEVEL-1;
    if(block_idx_level>=DEPTH)
    {
        block_idx_level=DEPTH-1;
    }
    uint64_t block_key=key&masks[block_idx_level];
    block_key=(block_key<<32)+block_idx_level;
    block_idx=MurmurHash2(&block_key, 8, SEED)%BLOCK_NUM;
    #endif
    for(int i=0;i<D;i++)
    {
        uint64_t x=0;
        x=key&masks[high_level];
        x=(x<<32)+high_level;
        #ifdef USEBLOCK
        int new_block_idx_level=high_level-high_level%BLOCK_LEVEL+BLOCK_LEVEL-1;
        if(new_block_idx_level>=DEPTH)
        {
            new_block_idx_level=DEPTH-1;
        }
        if(new_block_idx_level!=block_idx_level)
        {
            block_idx_level=new_block_idx_level;
            block_key=key&masks[block_idx_level];
            block_key=(block_key<<32)+block_idx_level;
            block_idx=MurmurHash2(&block_key, 8, SEED);
            block_idx=block_idx%BLOCK_NUM;
        }
        uint32_t in_block_idx=MurmurHash2(&x, 8, SEED1);
        uint32_t bucket_idx=0;
        bucket_idx=block_idx*BLOCK_SIZE+in_block_idx%BLOCK_SIZE;
        #else
        uint32_t bucket_idx=MurmurHash2(&x, 8, SEED1);
        bucket_idx=bucket_idx%BUCKET_NUM;
        #endif
        find_cell(b,bucket_idx,key,high_level,threshold);
        if(high_level==DEPTH-1)
        {
            high_level=L-1;
        }
        else
        {
            high_level+=L;
            if(high_level>=DEPTH)
            {
                high_level=DEPTH-1;
            }
        }
    } 
}
std::vector<std::pair<uint64_t,uint32_t>> query(BUCKET *b,int bucket_num,int threshold)
{
//     std::unordered_map<u_int64_t,u_int32_t> flow_size;
//     std::vector<std::pair<uint64_t,uint32_t>> HHH;
//     std::unordered_set<u_int64_t> nearest;
//     std::vector<std::vector<uint32_t>> level_cell_idx(DEPTH);
//     for(int i=0;i<BUCKET_NUM;i++)
//     {
//         for(int j=0;j<CELL_NUM;j++)
//         {
//             int level=(b+i)->cells[j].level;
//             level_cell_idx[level].push_back((i*CELL_NUM+j)*3);
//             level_cell_idx[level-1].push_back((i*CELL_NUM+j)*3+1);
//             level_cell_idx[level-1].push_back((i*CELL_NUM+j)*3+2);
//         }
//     }
//     for(int i=0;i<DEPTH;i++)
//     {
//         size_t j=0;
//         for(j=0;j<level_cell_idx[i].size();j++)
//         {
//             int cell_idx=level_cell_idx[i][j];
//             int bucket_idx=cell_idx/3/CELL_NUM;
//             int in_bucket_cell_idx=(cell_idx/3)%CELL_NUM;
//             uint64_t key=(b+bucket_idx)->cells[in_bucket_cell_idx].key&masks[i];
//             uint32_t size=(b+bucket_idx)->cells[in_bucket_cell_idx].count;
//             if(i%2==0&&cell_idx%3==1)
//             {
//                 key=key&(~(1<<i));
//                 size=(b+bucket_idx)->cells[in_bucket_cell_idx].count-(b+bucket_idx)->cells[in_bucket_cell_idx].subcounts[0];
//             }
//             else if(i%2==0&&cell_idx%3==2)
//             {
//                 key=key|(1<<i); 
//                 size=(b+bucket_idx)->cells[in_bucket_cell_idx].subcounts[0];
//             }
//             key=(key<<32)+i;
//             flow_size[key]+=size;
//             uint32_t conditional_estimation=flow_size[key];
//             std::unordered_set<uint64_t> toerase;
//             for(auto y:nearest)
//             {
//                 uint32_t tmp=y>>32;
//                 if((tmp&masks[i])==(key>>32))
//                 {
//                     if(conditional_estimation>flow_size[y]) conditional_estimation-=flow_size[y];
//                     else conditional_estimation=0;
//                     toerase.insert(y);
//                 }
//             }
//             if(conditional_estimation>threshold)
//             {
//                 HHH.push_back(std::make_pair(key,flow_size[key]*(DEPTH+1)/(D*2)));
//                 for(auto y:toerase)
//                 {
//                     nearest.erase(y);
//                 }
//                 nearest.insert(key);    
//             }
            
//         }
//     }
//     return HHH;

    std::unordered_map<u_int64_t,u_int32_t> flow_size;
    std::vector<std::pair<uint64_t,uint32_t>> HHH;
    std::unordered_set<u_int64_t> nearest;
    std::vector<std::vector<std::pair<uint64_t,uint32_t>>> level_cell_idx(DEPTH);
    for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            int level=(b+i)->cells[j].level;
            uint64_t key=(b+i)->cells[j].key&masks[level];
            key=(key<<32)+level;
            if(key==0)
            {
                continue;
            }
            int subcountidx=0;
            std::queue<std::pair<uint64_t,uint32_t>> tmp;
            std::pair<uint64_t,uint32_t> root={key,(b+i)->cells[j].count};
//             level_cell_idx[level].push_back(root);
//             uint64_t child=root.first>>32;
//                child=(child<<32)+level-1;
//                std::pair<uint64_t,uint32_t> p1={child,(b+i)->cells[j].count-(b+i)->cells[j].subcounts[0]};
//                tmp.push(p1);
//             level_cell_idx[level-1].push_back(p1);
//                child=child>>32;
//                 child=child|(1<<(level-1));
//                 child=(child<<32)+level-1;
//                 std::pair<uint64_t,uint32_t> p2={child,(b+i)->cells[j].subcounts[0]};
//                 tmp.push(p2);
//             level_cell_idx[level-1].push_back(p2);
            tmp.push(root);
            while(!tmp.empty())
            {
               std::pair<uint64_t,uint32_t> p=tmp.front();
               tmp.pop();
               int level1=p.first&0xFFFFFFFF;
                if(level1>=33||level1<0)
                {
                    std::cout<<level<<std::endl;
                    break;
                }
               level_cell_idx[level1].push_back(p);
                int count=p.second;
                if(level1==level-L+1)
                {
                    continue;
                }
               uint64_t child=p.first>>32;
               child=(child<<32)+level1-1;
                if(subcountidx>=3)
                {
                    std::cout<<"adsd"<<std::endl;
                    break;
                }
               std::pair<uint64_t,uint32_t> p1={child,count-(b+i)->cells[j].subcounts[subcountidx]};
//                 level_cell_idx[level1-1].push_back(p1);
               tmp.push(p1);
               child=child>>32;
                child=child|(1<<(level1-1));
                child=(child<<32)+level1-1;
                std::pair<uint64_t,uint32_t> p2={child,(b+i)->cells[j].subcounts[subcountidx]};
                tmp.push(p2);
//                 level_cell_idx[level1-1].push_back(p2);
//                 break;
                subcountidx++;
            }
        }
    }
    for(int i=0;i<DEPTH;i++)
    {
        size_t j=0;
        for(j=0;j<level_cell_idx[i].size();j++)
        {
            
            uint64_t key=level_cell_idx[i][j].first;
            uint32_t size=level_cell_idx[i][j].second;
            flow_size[key]+=size;
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
                HHH.push_back(std::make_pair(key,flow_size[key]*(36)/(D*L)));
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
