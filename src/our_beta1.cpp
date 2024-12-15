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
int judge(uint32_t num,int threshold)
{
    return ((num*(DEPTH+1))/D)>=threshold;
}
void init(BUCKET *b, int bucket_num)
{
    for (int i = 0; i < bucket_num; i++)
    {
        (b + i)->update_time = 0;
        for (int j = 0; j < CELL_NUM; j++)
        {
//             (b + i)->cells[j].count = 0;
            (b + i)->cells[j].key = 0;
            (b + i)->cells[j].level = 0;
            (b+i)->cells[j].is_empty=1;
//             (b + i)->cells[j].flag = 0 ; 
//             (b + i)->cells[j].heavy_child_size = 0;
//             (b+i)->cells[j].replace_time=0;
            (b+i)->cells[j].left_child_count=0;
            (b+i)->cells[j].right_child_count=0;
//             (b+i)->cells[j].pass_val=0;
//             (b+i)->cells[j].replaced_val=0;
        }
    }
}
// uint32_t trigger(BUCKET *b, int bucket_idx1, uint32_t key,uint32_t level,int threshold,uint32_t passed_val,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
// {
//     judge_time++;
//     for(int l=level-2;l>=0;l-=2)
//     {
// //         if((level-l)/2>P)
// //         {
// //             break;
// //         }
//         uint64_t masked_key=key&masks[l];
//         uint64_t x=(masked_key<<32)+l;
//         uint32_t bucket_idx=0;
//         MurmurHash3_x86_32(&x, 8, SEED, &bucket_idx);
//         bucket_idx=bucket_idx%BUCKET_NUM;
//         for (int i = 0; i < CELL_NUM; i++)
//         {
//             if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==l&&((b + bucket_idx)->cells[i].key&masks[l])== masked_key)
//             {
//                 uint32_t left_cnt=(b+bucket_idx)->cells[i].left_child_count;
//                 uint32_t right_cnt=(b+bucket_idx)->cells[i].right_child_count;
//                 uint32_t sum_cnt=left_cnt+right_cnt;
//                 if((b + bucket_idx)->cells[i].flag==0&&sum_cnt>=threshold)
//                 {
//                     passed_val=sum_cnt;
//                     break;
//                 }
//             }
//         }
//         if(passed_val>0)
//         {
//             break;
//         }
//     }
//     for(int l=level;l<=DEPTH;l+=2)
//     {
//         uint64_t masked_key=key&masks[l];
//         uint64_t x=(masked_key<<32)+l;
//         uint32_t bucket_idx=0;
//         MurmurHash3_x86_32(&x, 8, SEED, &bucket_idx);
//         bucket_idx=bucket_idx%BUCKET_NUM;
//         if(l==DEPTH)
//         {
//             l--;
//         }
//         for (int i = 0; i < CELL_NUM; i++)
//         {
//             if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==l&&((b + bucket_idx)->cells[i].key&masks[l])== masked_key)
//             {
//                 uint32_t left_cnt=(b+bucket_idx)->cells[i].left_child_count;
//                 uint32_t right_cnt=(b+bucket_idx)->cells[i].right_child_count;
//                 uint32_t sum_cnt=left_cnt+right_cnt;
//                 (b+bucket_idx)->cells[i].heavy_child_size=passed_val;
//                 if((sum_cnt<passed_val||sum_cnt-passed_val<threshold))
//                 {
//                     (b+bucket_idx)->cells[i].flag=1;
// //                     false_HHH.insert(x);
// //                     uint64_t left_key=(x>>32)&(~(1<<(l-1)));
// //                     left_key=(left_key<<32)+(l-1);
// //                     false_HHH.insert(left_key);
// //                     uint64_t right_key=(x>>32)|(1<<(l-1));
// //                     right_key=(right_key<<32)+(l-1);
// //                     false_HHH.insert(right_key);
//                 }
//                 else
//                 {
//                     (b+bucket_idx)->cells[i].flag=0;
//                     passed_val=sum_cnt;
//                 }
//                 break;
//             }
//         }
//     }
//     return 0;
// }
uint32_t find_cell(BUCKET *b, int bucket_idx, uint32_t key,uint32_t level,int threshold,uint32_t passed_val,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
{
    int min_val_cell_idx = -1;
    int empty_cell_idx = -1;
    int has_empty_cell = 0;
    int is_inserted=0;
    uint32_t tmp=0;
    uint32_t min_cell_val=UINT32_MAX;
    (b + bucket_idx)->update_time++;
    uint64_t masked_key=key&masks[level];
    uint64_t x=(masked_key<<32)+level;
    int false_cell_idx=-1;
//     if((b + bucket_idx)->keys.find(x)==(b + bucket_idx)->keys.end())
//     {
//         (b + bucket_idx)->keys.insert(x);
//     }
    for (int i = 0; i < CELL_NUM; i++)
    {
        if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level&&((b + bucket_idx)->cells[i].key&masks[level])== masked_key)
        {
            is_inserted=1;
//             uint16_t left_heavy_cnt=(b + bucket_idx)->cells[i].heavy_child_size>>16;
//             uint16_t right_heavy_cnt=(b + bucket_idx)->cells[i].heavy_child_size&0xffff;
//             (b + bucket_idx)->cells[i].count+=1;
            hit_time++;
            if(level==DEPTH-1)
            {
                break;
            }
            if(key&(1<<(level-1)))
            {
                (b + bucket_idx)->cells[i].right_child_count+=1;
//                 right_heavy_cnt+=passed_val;
         
            }
            else
            {
                (b + bucket_idx)->cells[i].left_child_count+=1;
//                 left_heavy_cnt+=passed_val;

            }
//             if(replaced_val>0)
//             {
//                 if(left_cnt>=right_cnt)
//                 {
//                     left_cnt+=replaced_val;
//                 }
//                 else
//                 {
//                     right_cnt+=replaced_val;
//                 }
// //                 left_cnt+=replaced_val*left_cnt/sum_cnt;
// //                 right_cnt+=replaced_val*right_cnt/sum_cnt;
//                 sum_cnt=left_cnt+right_cnt;
//                 (b+bucket_idx)->cells[i].left_child_count=left_cnt;
//                 (b+bucket_idx)->cells[i].right_child_count=right_cnt;
//                 (b+bucket_idx)->cells[i].replaced_val=0;
//             }
//             if(query_keys.find(x)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//             uint64_t x_left=(x>>32)&(~(1<<(level-1)));
//             x_left=(x_left<<32)+(level-1);
//             if(query_keys.find(x_left)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//             uint64_t x_right=(x>>32)|(1<<(level-1));
//             x_right=(x_right<<32)+(level-1);
//             if(query_keys.find(x_right)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }

//             if(level<=21&&level%2==1&&sum_cnt>0&&(sum_cnt%(threshold/16)==0))
//             {            
//                 trigger(b,bucket_idx,key,level,threshold,0,query_keys,replace_other,replaced_time,insert_time,false_HHH,judge_time,hit_time);
//             }
            break; 
        }
        else if ((b + bucket_idx)->cells[i].is_empty)
        {
            has_empty_cell = 1;
            empty_cell_idx=i;
        }
        else
        {
            uint32_t left_cnt=(b+bucket_idx)->cells[i].left_child_count;
            uint32_t right_cnt=(b+bucket_idx)->cells[i].right_child_count;
            uint32_t cell_val=left_cnt+right_cnt;
//             uint32_t c1=b[bucket_idx].cells[i].heavy_child_size>>16;
//             uint32_t c2=b[bucket_idx].cells[i].heavy_child_size&0xffff;
//             if((b+bucket_idx)->cells[i].flag)
//             {
//                 if(cell_val>b[bucket_idx].cells[i].heavy_child_size)
//                 {
//                     cell_val-=b[bucket_idx].cells[i].heavy_child_size;
//                 }
//                 else
//                 {
//                     cell_val=0;
//                 }
//             }
           if(min_val_cell_idx==-1||min_cell_val>cell_val)
           {
                min_cell_val=cell_val;
                min_val_cell_idx=i;
           }
        }
    }
    if(is_inserted==0&&has_empty_cell==1)
    {
        hit_time++;
        (b + bucket_idx)->cells[empty_cell_idx].key = key;
        (b + bucket_idx)->cells[empty_cell_idx].level = level;
        if(key&(1<<(level-1)))
        {
            (b + bucket_idx)->cells[empty_cell_idx].right_child_count+=1;
        }
        else
        {
            (b + bucket_idx)->cells[empty_cell_idx].left_child_count+=1;
        }
//         (b + bucket_idx)->cells[empty_cell_idx].count = 1;
        (b + bucket_idx)->cells[empty_cell_idx].is_empty=0;
//         (b + bucket_idx)->cells[empty_cell_idx].flag=0;
    }
    else if (is_inserted==0&&has_empty_cell == 0&&min_val_cell_idx!=-1)
    {
//         if (min_cell_val<threshold/2&&rand()* pow(1.08,min_cell_val) < RAND_MAX)
//         {
//             if((b + bucket_idx)->cells[min_val_cell_idx].level==1)
//             {
//                 if(min_cell_val>=threshold/4)
//                 {
//                     return tmp;
//                 }
//             }
//             (b + bucket_idx)->cells[min_val_cell_idx].count-=1;
//             uint64_t key1=(b + bucket_idx)->cells[min_val_cell_idx].key;
//             uint32_t level1=(b + bucket_idx)->cells[min_val_cell_idx].level;
//             key1=key1&masks[level1];
//             key1=(key1<<32)+(level1);
            
//             if(min_cell_val<=1)
//             {
//                 if(query_keys.find(key1)!=query_keys.end())
//                 {
//                     replaced_time[key1]++;
//                 }
//                 (b + bucket_idx)->cells[min_val_cell_idx].count=1;
//                 (b + bucket_idx)->cells[min_val_cell_idx].key=key;
//                 (b + bucket_idx)->cells[min_val_cell_idx].level=high_level-k;
//                 (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
//                 (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
//             }
//         }
        if (min_cell_val==0||(rand() < RAND_MAX/min_cell_val))
        {
            if((b + bucket_idx)->cells[min_val_cell_idx].level==1)
            {
                if(min_cell_val>=threshold/4)
                {
                    return tmp;
                }
            }
//             if((b + bucket_idx)->cells[min_val_cell_idx].flag)
//             {
//                 min_cell_val+=(b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size;
//             }
//             replaced_time[key1]++;
//             if(query_keys.find(key1)!=query_keys.end())
//             {
//                 std::cout<<"replaced_val"<<" "<<min_cell_val<<" "<<sum_cnt<<" "<<flag<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
//             uint64_t key1_left=(key1>>32)&(~(1<<((b + bucket_idx)->cells[min_val_cell_idx].level-1)));
//             key1_left=(key1_left<<32)+((b + bucket_idx)->cells[min_val_cell_idx].level-1);
//             replaced_time[key1_left]++;
//             if(query_keys.find(key1_left)!=query_keys.end())
//             {
//                 std::cout<<"replaced_val"<<" "<<min_cell_val<<" "<<sum_cnt<<" "<<flag<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
//             uint64_t key1_right=(key1>>32)|(1<<((b + bucket_idx)->cells[min_val_cell_idx].level-1));
//             key1_right=(key1_right<<32)+((b + bucket_idx)->cells[min_val_cell_idx].level-1);
//             replaced_time[key1_right]++;
//             if(query_keys.find(key1_right)!=query_keys.end())
//             {
//                 std::cout<<"replaced_val"<<" "<<min_cell_val<<" "<<sum_cnt<<" "<<flag<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
            (b + bucket_idx)->cells[min_val_cell_idx].key=key;
//             uint64_t replacer=key&masks[level];
//             replacer=(replacer<<32)+level;
//             replace_other[replacer]++;
//             replacer=key&masks[level-1];
//             replacer=(replacer<<32)+level-1;
//             replace_other[replacer]++;
//             if(query_keys.find(x)!=query_keys.end())
//             {
//                 std::cout<<"replace_success"<<" "<<(b + bucket_idx)->cells[min_val_cell_idx].left_child_count+(b + bucket_idx)->cells[min_val_cell_idx].right_child_count<<" "<<flag<<" "<<(b + bucket_idx)->cells[min_val_cell_idx].replaced_val<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
//             uint64_t x_left=(x>>32)&(~(1<<(level-1)));
//             x_left=(x_left<<32)+(level-1);
//             if(query_keys.find(x_left)!=query_keys.end())
//             {
//                 std::cout<<"replace_success"<<" "<<(b + bucket_idx)->cells[min_val_cell_idx].left_child_count+(b + bucket_idx)->cells[min_val_cell_idx].right_child_count<<" "<<flag<<" "<<(b + bucket_idx)->cells[min_val_cell_idx].replaced_val<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
//             uint64_t x_right=(x>>32)|(1<<(level-1));
//             x_right=(x_right<<32)+(level-1);
//             if(query_keys.find(x_right)!=query_keys.end())
//             {
//                 std::cout<<"replace_success"<<" "<<(b + bucket_idx)->cells[min_val_cell_idx].left_child_count+(b + bucket_idx)->cells[min_val_cell_idx].right_child_count<<" "<<flag<<" "<<(b + bucket_idx)->cells[min_val_cell_idx].replaced_val<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
            (b + bucket_idx)->cells[min_val_cell_idx].level=level;
        
//             (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
//             (b + bucket_idx)->cells[min_val_cell_idx].pass_val=0;
//             (b + bucket_idx)->cells[min_val_cell_idx].replace_time++;
//             (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=0;
//             (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=0;
            (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=min_cell_val/2;
            (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=min_cell_val/2;
//             if((b + bucket_idx)->cells[min_val_cell_idx].flag)
//             {
//                 if((b + bucket_idx)->cells[min_val_cell_idx].replaced_val==0)
//                 {
//                     (b + bucket_idx)->cells[min_val_cell_idx].replaced_val=min_cell_val;
//                 }
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=(b + bucket_idx)->cells[min_val_cell_idx].replaced_val/2;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=(b + bucket_idx)->cells[min_val_cell_idx].replaced_val/2;
//             }
//             else
//             {
//                 (b + bucket_idx)->cells[min_val_cell_idx].replaced_val=(b + bucket_idx)->cells[min_val_cell_idx].replaced_val;
//                 (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=(b + bucket_idx)->cells[min_val_cell_idx].replaced_val/2;
//                 (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=(b + bucket_idx)->cells[min_val_cell_idx].replaced_val/2;
                
//             }
//             (b + bucket_idx)->cells[min_val_cell_idx].flag=0;
        }
//         else
//         {
//             uint64_t replacer=key&masks[level];
//             replacer=(replacer<<32)+level;
//             uint64_t key1=(b + bucket_idx)->cells[min_val_cell_idx].key;
//             key1=key1&masks[(b + bucket_idx)->cells[min_val_cell_idx].level];
//             key1=(key1<<32)+((b + bucket_idx)->cells[min_val_cell_idx].level);
//             int flag=(b + bucket_idx)->cells[min_val_cell_idx].flag;
//             if(query_keys.find(replacer)!=query_keys.end())
//             {
//                 std::cout<<"fail_val"<<" "<<min_cell_val<<" "<<sum_cnt<<" "<<flag<<" "<<key1<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
//             replacer=key&masks[level-1];
//             replacer=(replacer<<32)+level-1;
//             if(query_keys.find(replacer)!=query_keys.end())
//             {
//                 std::cout<<"fail_val"<<" "<<min_cell_val<<" "<<sum_cnt<<" "<<flag<<" "<<key1<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
//             }
//         }
    }
    return tmp;
}
// uint32_t find_cell(BUCKET *b, int bucket_idx, uint32_t key1,uint32_t level1,uint32_t key2,uint32_t level2,int threshold,uint32_t passed_val,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
// {
//     int min_val_cell_idx1 = -1;
//     int empty_cell_idx1 = -1;
//     int has_empty_cell = 0;
//     int is_inserted1=0;
//     int min_val_cell_idx2 = -1;
//     int empty_cell_idx2 = -1;
//     int is_inserted2=0;
//     uint32_t tmp=0;
//     uint32_t min_cell_val1=UINT32_MAX;
//     uint32_t min_cell_val2=UINT32_MAX;
//     (b + bucket_idx)->update_time++;
//     uint64_t masked_key1=key1&masks[level1];
//     uint64_t x1=(masked_key1<<32)+level1;
//     uint64_t masked_key2=key2&masks[level2];
//     uint64_t x2=(masked_key2<<32)+level1;
//     int false_cell_idx=-1;
// //     if((b + bucket_idx)->keys.find(x)==(b + bucket_idx)->keys.end())
// //     {
// //         (b + bucket_idx)->keys.insert(x);
// //     }
//     for (int i = 0; i < CELL_NUM; i++)
//     {
//         if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level1&&((b + bucket_idx)->cells[i].key&masks[level1])== masked_key1)
//         {
//             is_inserted1=1;
// //             (b + bucket_idx)->cells[i].count+=1;
//             hit_time++;
//             if(key1&(1<<(level1-1)))
//             {
//                 (b + bucket_idx)->cells[i].right_child_count+=1;
//             }
//             else
//             {
//                 (b + bucket_idx)->cells[i].left_child_count+=1;
//             }
//             uint32_t left_cnt=(b+bucket_idx)->cells[i].left_child_count;
//             uint32_t right_cnt=(b+bucket_idx)->cells[i].right_child_count;
//             uint32_t replaced_val=(b+bucket_idx)->cells[i].replaced_val;
//             uint32_t sum_cnt=left_cnt+right_cnt;
// //             if(replaced_val>0)
// //             {
// //                 if(left_cnt>=right_cnt)
// //                 {
// //                     left_cnt+=replaced_val;
// //                 }
// //                 else
// //                 {
// //                     right_cnt+=replaced_val;
// //                 }
// // //                 left_cnt+=replaced_val*left_cnt/sum_cnt;
// // //                 right_cnt+=replaced_val*right_cnt/sum_cnt;
// //                 sum_cnt=left_cnt+right_cnt;
// //                 (b+bucket_idx)->cells[i].left_child_count=left_cnt;
// //                 (b+bucket_idx)->cells[i].right_child_count=right_cnt;
// //                 (b+bucket_idx)->cells[i].replaced_val=0;
// //             }
//             if(query_keys.find(x1)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//             uint64_t x_left=(x1>>32)&(~(1<<(level1-1)));
//             x_left=(x_left<<32)+(level1-1);
//             if(query_keys.find(x_left)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//             uint64_t x_right=(x1>>32)|(1<<(level1-1));
//             x_right=(x_right<<32)+(level1-1);
//             if(query_keys.find(x_right)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//         }
//         else if((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level2&&((b + bucket_idx)->cells[i].key&masks[level2])== masked_key2)
//         {
//             is_inserted2=1;
// //             (b + bucket_idx)->cells[i].count+=1;
//             hit_time++;
//             if(key2&(1<<(level2-1)))
//             {
//                 (b + bucket_idx)->cells[i].right_child_count+=1;
//             }
//             else
//             {
//                 (b + bucket_idx)->cells[i].left_child_count+=1;
//             }
//             uint32_t left_cnt=(b+bucket_idx)->cells[i].left_child_count;
//             uint32_t right_cnt=(b+bucket_idx)->cells[i].right_child_count;
//             uint32_t replaced_val=(b+bucket_idx)->cells[i].replaced_val;
//             uint32_t sum_cnt=left_cnt+right_cnt;
// //             if(replaced_val>0)
// //             {
// //                 if(left_cnt>=right_cnt)
// //                 {
// //                     left_cnt+=replaced_val;
// //                 }
// //                 else
// //                 {
// //                     right_cnt+=replaced_val;
// //                 }
// //                 sum_cnt=left_cnt+right_cnt;
// //                 (b+bucket_idx)->cells[i].left_child_count=left_cnt;
// //                 (b+bucket_idx)->cells[i].right_child_count=right_cnt;
// //                 (b+bucket_idx)->cells[i].replaced_val=0;
// //             }
//             if(query_keys.find(x2)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//             uint64_t x_left=(x2>>32)&(~(1<<(level2-1)));
//             x_left=(x_left<<32)+(level2-1);
//             if(query_keys.find(x_left)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//             uint64_t x_right=(x2>>32)|(1<<(level2-1));
//             x_right=(x_right<<32)+(level2-1);
//             if(query_keys.find(x_right)!=query_keys.end())
//             {
//                 std::cout<<"insert_success"<<" "<<sum_cnt<<" "<<bucket_idx*CELL_NUM+i<<std::endl;
//             }
//         }
//         else if ((b + bucket_idx)->cells[i].is_empty)
//         {
//             if(empty_cell_idx1==-1)
//             {
//                 empty_cell_idx1=i;
//             }
//             else
//             {
//                 empty_cell_idx2=i;
//             }
//         }
//         else
//         {
//             uint32_t left_cnt=(b+bucket_idx)->cells[i].left_child_count;
//             uint32_t right_cnt=(b+bucket_idx)->cells[i].right_child_count;
//             uint32_t cell_val=left_cnt+right_cnt+(b+bucket_idx)->cells[i].replaced_val;
//             if((b+bucket_idx)->cells[i].flag)
//             {
//                 if(cell_val>b[bucket_idx].cells[i].heavy_child_size)
//                 {
//                     cell_val-=b[bucket_idx].cells[i].heavy_child_size;
//                 }
//                 else
//                 {
//                     cell_val=0;
//                 }
//             }
//            if(min_val_cell_idx1==-1||min_cell_val1>cell_val)
//            {
//                 min_cell_val1=cell_val;
//                 min_val_cell_idx1=i;
//            }
//            else if(min_val_cell_idx2==-1||min_cell_val2>cell_val)
//            {
//                 min_cell_val2=cell_val;
//                 min_val_cell_idx2=i;
//            }
//         }
//     }
//     if(is_inserted1==0&&empty_cell_idx1!=-1)
//     {
//         hit_time++;
//         (b + bucket_idx)->cells[empty_cell_idx1].key = key1;
//         (b + bucket_idx)->cells[empty_cell_idx1].level = level1;
//         if(key1&(1<<(level1-1)))
//         {
//             (b + bucket_idx)->cells[empty_cell_idx1].right_child_count+=1;
//         }
//         else
//         {
//             (b + bucket_idx)->cells[empty_cell_idx1].left_child_count+=1;
//         }
// //         (b + bucket_idx)->cells[empty_cell_idx].count = 1;
//         (b + bucket_idx)->cells[empty_cell_idx1].is_empty=0;
//         (b + bucket_idx)->cells[empty_cell_idx1].flag=0;
//         is_inserted1=1;
//         empty_cell_idx1=-1;
//     }
//     if(is_inserted2==0&&(empty_cell_idx2!=-1||empty_cell_idx1!=-1))
//     {
//         int empty_cell_idx=empty_cell_idx2;
//         if(empty_cell_idx==-1)
//         {
//             empty_cell_idx=empty_cell_idx1;
//         }
//         hit_time++;
//         (b + bucket_idx)->cells[empty_cell_idx].key = key2;
//         (b + bucket_idx)->cells[empty_cell_idx].level = level2;
//         if(key2&(1<<(level2-1)))
//         {
//             (b + bucket_idx)->cells[empty_cell_idx].right_child_count+=1;
//         }
//         else
//         {
//             (b + bucket_idx)->cells[empty_cell_idx].left_child_count+=1;
//         }
// //         (b + bucket_idx)->cells[empty_cell_idx].count = 1;
//         (b + bucket_idx)->cells[empty_cell_idx].is_empty=0;
//         (b + bucket_idx)->cells[empty_cell_idx].flag=0;
//         is_inserted2=1;
//     }
//     if(is_inserted1==0)
//     {
//         uint32_t sum_cnt=(b + bucket_idx)->cells[min_val_cell_idx1].right_child_count+(b + bucket_idx)->cells[min_val_cell_idx1].left_child_count;
//         if (min_cell_val1==0||(rand() < RAND_MAX/min_cell_val1))
//         {
//             int flag=(b + bucket_idx)->cells[min_val_cell_idx1].flag;
//             (b + bucket_idx)->cells[min_val_cell_idx1].key=key1;
//             (b + bucket_idx)->cells[min_val_cell_idx1].level=level1;
//             (b + bucket_idx)->cells[min_val_cell_idx1].heavy_child_size=0;
//             (b + bucket_idx)->cells[min_val_cell_idx1].replace_time++;
// //             (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=0;
// //             (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=0;
//             (b + bucket_idx)->cells[min_val_cell_idx1].left_child_count=min_cell_val1/2;
//             (b + bucket_idx)->cells[min_val_cell_idx1].right_child_count=min_cell_val1/2;
//             (b + bucket_idx)->cells[min_val_cell_idx1].flag=0;
//             min_val_cell_idx1=-1;
//         }
//     }
//     if (is_inserted2==0)
//     {
//         int min_val_cell_idx=min_val_cell_idx2;
//         int min_cell_val=min_cell_val2;
//         if(min_val_cell_idx1!=-1)
//         {
//             min_val_cell_idx=min_val_cell_idx1;
//             min_cell_val=min_cell_val1;
//         }
//         uint32_t sum_cnt=(b + bucket_idx)->cells[min_val_cell_idx].right_child_count+(b + bucket_idx)->cells[min_val_cell_idx].left_child_count;
//         if (min_cell_val==0||(rand() < RAND_MAX/min_cell_val))
//         {
//             int flag=(b + bucket_idx)->cells[min_val_cell_idx].flag;
//             (b + bucket_idx)->cells[min_val_cell_idx1].key=key1;
//             (b + bucket_idx)->cells[min_val_cell_idx].level=level2;
//             (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
// //             (b + bucket_idx)->cells[min_val_cell_idx].pass_val=0;
//             (b + bucket_idx)->cells[min_val_cell_idx].replace_time++;
// //             (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=0;
// //             (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=0;
//             (b + bucket_idx)->cells[min_val_cell_idx].left_child_count=min_cell_val/2;
//             (b + bucket_idx)->cells[min_val_cell_idx].right_child_count=min_cell_val/2;
//             (b + bucket_idx)->cells[min_val_cell_idx].flag=0;
//         }
//     }
//     return tmp;
// }
// void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
// {
//     int hash_level = 0;
//     hash_level=rand()%(DEPTH+1);
//     int high_level=hash_level-hash_level%2+1;
//     if(high_level>=DEPTH)
//     {
//         high_level=DEPTH-1;
//     }
//     int round=D/2;
//     uint64_t x=0;
//     x=key&masks[high_level];
//     x=(x<<32)+high_level;
//     uint32_t bucket_idx=0;
//     MurmurHash3_x86_32(&x, 8, SEED, &bucket_idx);
//     bucket_idx=bucket_idx%bucket_num;
//     int next_high_level=high_level-2;
//     if(next_high_level<0)
//     {
//         next_high_level=32;
//     }
//         find_cell(b,bucket_idx,key,high_level,key,next_high_level,threshold,0,query_keys,replace_other,replaced_time,insert_time,false_HHH,judge_time,hit_time);
        
//         // if(high_level==DEPTH-1)
//         // {
//         //     high_level--;
//         // }
//         // else
//         // {
//         //     high_level-=2;
//         //     if(high_level<0)
//         //     {
//         //         high_level=DEPTH-1;
//         //     }
//         // }  
// }
void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
{
    int hash_level = 0;
    hash_level=rand()%(DEPTH+1);
    int high_level=hash_level-hash_level%2+1;
    if(high_level>=DEPTH)
    {
        high_level=DEPTH-1;
    }
    int round=D/2;
    uint64_t passed_val=0;
    uint32_t block_idx=0;
//     int block_idx_level=high_level-high_level%BLOCK_LEVEL+BLOCK_LEVEL-1;
//     if(block_idx_level>=DEPTH)
//     {
//         block_idx_level=DEPTH-1;
//     }
//     uint64_t block_key=key&masks[block_idx_level];
//     block_key=(block_key<<32)+block_idx_level;
//     MurmurHash3_x86_32(&block_key, 8, SEED, &block_idx);
//     block_idx=block_idx%BLOCK_NUM;
    for(int i=0;i<round;i++)
    {
        uint64_t x=0;
        x=key&masks[high_level];
        x=(x<<32)+high_level;
//         if(query_keys.find(x)!=query_keys.end())
//         {
//             insert_time[x]++;
//         }
        uint64_t left_key=key&masks[high_level-1];
        left_key=(left_key<<32)+high_level-1;
//         if(query_keys.find(left_key)!=query_keys.end())
//         {
//             insert_time[left_key]++;
//         }

//         int new_block_idx_level=high_level-high_level%BLOCK_LEVEL+BLOCK_LEVEL-1;
//         if(new_block_idx_level>=DEPTH)
//         {
//             new_block_idx_level=DEPTH-1;
//         }
//         if(new_block_idx_level!=block_idx_level)
//         {
//             block_idx_level=new_block_idx_level;
//             block_key=key&masks[block_idx_level];
//             block_key=(block_key<<32)+block_idx_level;
//             MurmurHash3_x86_32(&block_key, 8, SEED, &block_idx);
//         //     if(x==2225341165874446353)
//         // {
//         //     std::cout<<" "<<block_key<<" "<<block_idx<<std::endl;
//         //     continue;

//         // }
//         block_idx=block_idx%BLOCK_NUM;
//         }
// //         uint32_t in_block_idx=0;
// //         MurmurHash3_x86_32(&x, 8, SEED1, &in_block_idx);
//         uint32_t in_block_idx=(x*8821)>>60;
//         replaced_time[in_block_idx%BLOCK_SIZE]++;
//         uint32_t bucket_idx=0;
//             bucket_idx=block_idx*BLOCK_SIZE+in_block_idx%BLOCK_SIZE;
        uint32_t bucket_idx=0;
        MurmurHash3_x86_32(&x,8,SEED,&bucket_idx);
        bucket_idx=bucket_idx%bucket_num;
        passed_val=find_cell(b,bucket_idx,key,high_level,threshold,passed_val,query_keys,replace_other,replaced_time,insert_time,false_HHH,judge_time,hit_time);
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
// uint32_t trigger(BUCKET *b, int bucket_idx1, uint32_t key,uint32_t level,int threshold,uint32_t passed_val,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
// {
//     judge_time++;
//     for(int l=level-1;l>=0;l--)
//     {
//         if((level-l)>P)
//         {
//             break;
//         }
//         uint64_t masked_key=key&masks[l];
//         uint64_t x=(masked_key<<32)+l;
//         uint32_t bucket_idx=0;
//         MurmurHash3_x86_32(&x, 8, SEED, &bucket_idx);
//         bucket_idx=bucket_idx%BUCKET_NUM;
//         for (int i = 0; i < CELL_NUM; i++)
//         {
//             if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==l&&((b + bucket_idx)->cells[i].key&masks[l])== masked_key)
//             {
//                 uint32_t sum_cnt=(b + bucket_idx)->cells[i].count;
//                 if((b + bucket_idx)->cells[i].flag==0&&sum_cnt>=threshold)
//                 {
//                     passed_val=sum_cnt;
//                     break;
//                 }
//             }
//         }
//         if(passed_val>0)
//         {
//             break;
//         }
//     }
//     for(int l=level;l<DEPTH;l++)
//     {
//         uint64_t masked_key=key&masks[l];
//         uint64_t x=(masked_key<<32)+l;
//         uint32_t bucket_idx=0;
//         MurmurHash3_x86_32(&x, 8, SEED, &bucket_idx);
//         bucket_idx=bucket_idx%BUCKET_NUM;
//         for (int i = 0; i < CELL_NUM; i++)
//         {
//             if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==l&&((b + bucket_idx)->cells[i].key&masks[l])== masked_key)
//             {
//                 uint32_t sum_cnt=(b + bucket_idx)->cells[i].count;
//                 (b+bucket_idx)->cells[i].heavy_child_size=passed_val;
//                 if((sum_cnt<passed_val||sum_cnt-passed_val<threshold))
//                 {
//                     (b+bucket_idx)->cells[i].flag=1;
                    
//                 }
//                 else
//                 {
//                     (b+bucket_idx)->cells[i].flag=0;
//                     passed_val=sum_cnt;
//                 }
//                 break;
//             }
//         }
//     }
//     return 0;
// }
// uint32_t find_cell(BUCKET *b, int bucket_idx, uint32_t key,uint32_t level,int threshold,uint32_t passed_val,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
// {
//     int min_val_cell_idx = -1;
//     int empty_cell_idx = -1;
//     int has_empty_cell = 0;
//     int is_inserted=0;
//     uint32_t tmp=0;
//     uint32_t min_cell_val=UINT32_MAX;
//     (b + bucket_idx)->update_time++;
//     uint64_t masked_key=key&masks[level];
//     uint64_t x=(masked_key<<32)+level;
//     int false_cell_idx=-1;
// //     if((b + bucket_idx)->keys.find(x)==(b + bucket_idx)->keys.end())
// //     {
// //         (b + bucket_idx)->keys.insert(x);
// //     }
//     for (int i = 0; i < CELL_NUM; i++)
//     {
//         if ((b + bucket_idx)->cells[i].is_empty==0&&(b + bucket_idx)->cells[i].level==level&&((b + bucket_idx)->cells[i].key&masks[level])== masked_key)
//         {
//             is_inserted=1;
//             (b + bucket_idx)->cells[i].count+=1;
//             hit_time++;
//             if(level==DEPTH-1)
//             {
//                 break;
//             }
//             uint32_t sum_cnt=(b + bucket_idx)->cells[i].count;
//             if(level==0&&sum_cnt>0&&(sum_cnt%(threshold)==0))
//             {            trigger(b,bucket_idx,key,level,threshold,0,query_keys,replace_other,replaced_time,insert_time,false_HHH,judge_time,hit_time);
//             }
//             break; 
//         }
//         else if ((b + bucket_idx)->cells[i].is_empty)
//         {
//             has_empty_cell = 1;
//             empty_cell_idx=i;
//         }
//         else
//         {
//             uint32_t cell_val=(b+bucket_idx)->cells[i].count;
//             if((b+bucket_idx)->cells[i].flag)
//             {
//                 if(cell_val>(b+bucket_idx)->cells[i].heavy_child_size)
//                 {
//                     cell_val-=(b+bucket_idx)->cells[i].heavy_child_size;
//                 }
//                 else
//                 {
//                     cell_val=0;
//                 }
//             }
//            if(min_val_cell_idx==-1||min_cell_val>cell_val)
//            {
//                 min_cell_val=cell_val;
//                 min_val_cell_idx=i;
//            }
//         }
//     }
//     if(is_inserted==0&&has_empty_cell==1)
//     {
//         hit_time++;
//         (b + bucket_idx)->cells[empty_cell_idx].key = key;
//         (b + bucket_idx)->cells[empty_cell_idx].level = level;
//         (b + bucket_idx)->cells[empty_cell_idx].is_empty=0;
//         (b + bucket_idx)->cells[empty_cell_idx].flag=0;
//         (b + bucket_idx)->cells[empty_cell_idx].count = 1;
//     }
//     else if (is_inserted==0&&has_empty_cell == 0)
//     {
//         // if (min_cell_val*DEPTH/D<threshold&&rand()* pow(1.08,min_cell_val) < RAND_MAX)
//         // {
//         //     (b + bucket_idx)->cells[min_val_cell_idx].count-=1;
//         //     uint64_t key1=(b + bucket_idx)->cells[min_val_cell_idx].key;
//         //     uint32_t level1=(b + bucket_idx)->cells[min_val_cell_idx].level;
//         //     key1=key1&masks[level1];
//         //     key1=(key1<<32)+(level1);
//         //     if(query_keys.find(key1)!=query_keys.end())
//         //     {
//         //         decayed_time[key1]++;
//         //     }
//         //     if(query_keys.find(x)!=query_keys.end())
//         //     {
//         //         decay_other[x]++;
//         //     }
//         //     if(min_cell_val<=1)
//         //     {
//         //         if(query_keys.find(key1)!=query_keys.end())
//         //         {
//         //             replaced_time[key1]++;
//         //         }
//         //         (b + bucket_idx)->cells[min_val_cell_idx].count=1;
//         //         (b + bucket_idx)->cells[min_val_cell_idx].key=key;
//         //         (b + bucket_idx)->cells[min_val_cell_idx].level=high_level-k;
//         //         (b + bucket_idx)->cells[min_val_cell_idx].flag=1;
//         //         (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
//         //     }
//         // }
//         if (min_cell_val==0||(rand() < RAND_MAX/min_cell_val))
//         {
//             uint64_t key1=(b + bucket_idx)->cells[min_val_cell_idx].key;
//             key1=key1&masks[(b + bucket_idx)->cells[min_val_cell_idx].level];
//             key1=(key1<<32)+((b + bucket_idx)->cells[min_val_cell_idx].level);
//             int flag=(b + bucket_idx)->cells[min_val_cell_idx].flag;
// //             if((b + bucket_idx)->cells[min_val_cell_idx].level==1)
// //             {
// //                 if(min_cell_val>=threshold/4)
// //                 {
// //                     return tmp;
// //                 }
// //             }
//             replaced_time[key1]++;
// //             if(query_keys.find(key1)!=query_keys.end())
// //             {
// //                 std::cout<<"replaced_val"<<" "<<min_cell_val<<" "<<flag<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
// //             }
//             (b + bucket_idx)->cells[min_val_cell_idx].key=key;
//             uint64_t replacer=key&masks[level];
//             replacer=(replacer<<32)+level;
//             replace_other[replacer]++;
//             (b + bucket_idx)->cells[min_val_cell_idx].level=level;
//             (b + bucket_idx)->cells[min_val_cell_idx].flag=0;
//             (b + bucket_idx)->cells[min_val_cell_idx].heavy_child_size=0;
//         }
// //         else
// //         {
// //             uint64_t replacer=key&masks[level];
// //             replacer=(replacer<<32)+level;
// //             uint64_t key1=(b + bucket_idx)->cells[min_val_cell_idx].key;
// //             key1=key1&masks[(b + bucket_idx)->cells[min_val_cell_idx].level];
// //             key1=(key1<<32)+((b + bucket_idx)->cells[min_val_cell_idx].level);
// //             int flag=(b + bucket_idx)->cells[min_val_cell_idx].flag;
// //             if(query_keys.find(replacer)!=query_keys.end())
// //             {
// //                 std::cout<<"fail_val"<<" "<<min_cell_val<<" "<<flag<<" "<<key1<<" "<<bucket_idx*CELL_NUM+min_val_cell_idx<<std::endl;
// //             }
// //         }
//     }
//     return tmp;
// }
// void insert_test(BUCKET *b, int bucket_num, uint32_t key,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &replace_other,std::unordered_map<uint64_t,uint32_t> &replaced_time,std::unordered_map<uint64_t,uint32_t> &insert_time,std::unordered_set<uint64_t> &false_HHH,uint32_t &judge_time,uint32_t &hit_time)
// {
//     int hash_level = 0;
//     hash_level=rand()%(DEPTH);
//     int round=D;
//     uint64_t passed_val=0;
//     for(int i=0;i<round;i++)
//     {
//         uint64_t x=0;
//         x=key&masks[hash_level];
//         x=(x<<32)+hash_level;
//         uint32_t bucket_idx=0;
//         MurmurHash3_x86_32(&x, 8, SEED, &bucket_idx);
//         bucket_idx=bucket_idx%bucket_num;
//         passed_val=find_cell(b,bucket_idx,key,hash_level,threshold,passed_val,query_keys,replace_other,replaced_time,insert_time,false_HHH,judge_time,hit_time);
//         hash_level=(hash_level+1)%DEPTH;
         
//     } 
// }
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
            if((b+i)->cells[j].is_empty||((b+i)->cells[j].left_child_count+(b+i)->cells[j].right_child_count)<threshold)
            {
                continue;
            }
            int level=(b+i)->cells[j].level;
            level_cell_idx[level].push_back((i*CELL_NUM+j)*3);
            if((b+i)->cells[j].left_child_count>=threshold)
            {
                level_cell_idx[level-1].push_back((i*CELL_NUM+j)*3+1);
            }
            if((b+i)->cells[j].right_child_count>=threshold)
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
            uint32_t size=(b+bucket_idx)->cells[in_bucket_cell_idx].left_child_count+(b+bucket_idx)->cells[in_bucket_cell_idx].right_child_count;
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
            flow_size[key]+=size;
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
                HHH.push_back(std::make_pair(key,flow_size[key]*(DEPTH+1)/D));
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
// std::vector<std::pair<uint64_t,uint32_t>> query_test(BUCKET *b,int bucket_num,int threshold,std::unordered_set<uint64_t> &query_keys,std::unordered_map<uint64_t,uint32_t> &est_size,std::unordered_map<uint64_t,uint32_t> &est_concnt)
// {

//     std::unordered_map<u_int64_t,u_int32_t> flow_size;
//     std::vector<std::pair<uint64_t,uint32_t>> HHH;
//     std::unordered_set<u_int64_t> nearest;
//     std::vector<std::vector<uint32_t>> level_cell_idx(DEPTH);
//     for(int i=0;i<bucket_num;i++)
//     {
//         for(int j=0;j<CELL_NUM;j++)
//         {
//             if((b+i)->cells[j].is_empty||(b+i)->cells[j].count<threshold)
//             {
//                 continue;
//             }
//             int level=(b+i)->cells[j].level;
//             level_cell_idx[level].push_back((i*CELL_NUM+j)*3);
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
//             key=(key<<32)+i;
//             flow_size[key]=std::max(flow_size[key],size);
//             if(query_keys.find(key)!=query_keys.end())
//             {
//                 est_size[key]=flow_size[key];
//             }
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
//             if(query_keys.find(key)!=query_keys.end())
//             {
//                 est_concnt[key]=conditional_estimation;
//             }
//             if(conditional_estimation>threshold)
//             {
//                 HHH.push_back(std::make_pair(key,flow_size[key]));
//                 for(auto y:toerase)
//                 {
//                     nearest.erase(y);
//                 }
//                 nearest.insert(key);    
//             }
            
//         }
//     }
//     return HHH;
// }