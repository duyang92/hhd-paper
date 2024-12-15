#include "our.h"

#include <stdlib.h>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include <time.h>
#include<sys/time.h>
#include<string.h>
#include<random>
#include<unordered_set>
#include<algorithm>
#include<vector>
#if DEPTH==5
    u_int32_t masks[DEPTH]={
    0xFFFFFFFFu,
    0xFFFFFF00u,
    0xFFFF0000u,
    0xFF000000u,
    0x00000000u
    };
#else
    u_int32_t masks[DEPTH]={
    0xFFFFFFFFu << 0,
	0xFFFFFFFFu << 1,
	0xFFFFFFFFu << 2,
	0xFFFFFFFFu << 3,
	0xFFFFFFFFu << 4,
	0xFFFFFFFFu << 5,
	0xFFFFFFFFu << 6,
	0xFFFFFFFFu << 7,
	0xFFFFFFFFu << 8,
	0xFFFFFFFFu << 9,
	0xFFFFFFFFu << 10,
	0xFFFFFFFFu << 11,
	0xFFFFFFFFu << 12,
	0xFFFFFFFFu << 13,
	0xFFFFFFFFu << 14,
	0xFFFFFFFFu << 15,
	0xFFFFFFFFu << 16,
	0xFFFFFFFFu << 17,
	0xFFFFFFFFu << 18,
	0xFFFFFFFFu << 19,
	0xFFFFFFFFu << 20,
	0xFFFFFFFFu << 21,
	0xFFFFFFFFu << 22,
	0xFFFFFFFFu << 23,
	0xFFFFFFFFu << 24,
	0xFFFFFFFFu << 25,
	0xFFFFFFFFu << 26,
	0xFFFFFFFFu << 27,
	0xFFFFFFFFu << 28,
	0xFFFFFFFFu << 29,
	0xFFFFFFFFu << 30,
	0xFFFFFFFFu << 31,
	0x00000000u
};
#endif

inline double now_us ()
{
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return (tv.tv_sec * (uint64_t) 1000000 + (double)tv.tv_nsec/1000);
}
int main(int argc,char *argv[])
{
	uint32_t threshold=std::atoi(argv[1]);
	uint32_t bucket_num=(std::atoi(argv[2])*1024*8/(double)128/CELL_NUM);
    BUCKET b[bucket_num];
    init(b,bucket_num);
	std::ifstream fin;
	uint64_t query_key=0;
	std::unordered_set<uint64_t> query_keys;
	for(int i=3;i<argc;i++)
	{
		query_key=std::stoull(argv[i]);
		query_keys.insert(query_key);
	}
	fin.open("data",std::ios::binary|std::ios::ate);
	std::ofstream fout;
	std::streamsize size=fin.tellg();
	fin.seekg(0,std::ios::beg);
	fout.open("output");
	char *buffer=(char*)malloc(size);
    fin.read(buffer,size);
	fin.close();
	 #if DEPTH==5
    fin.open("flow_conditional_count_byte_"+std::to_string(threshold));
    #else
    fin.open("flow_conditional_count_bit_"+std::to_string(threshold));
    #endif
    std::unordered_set<uint64_t> actual_HHH;
    std::unordered_map<uint64_t,uint32_t> actual_HHH_size;
    uint64_t src=0;
    uint32_t count=0;
    int is_HHH=0;
    while(fin>>src&&fin>>count&&fin>>is_HHH)
    {
        if(is_HHH)
        {
            actual_HHH.insert(src);
        }
    }
    fin.close();
    #if DEPTH==5
    fin.open("flow_actual_size_byte_"+std::to_string(threshold));
    #else
    fin.open("flow_actual_size_bit_"+std::to_string(threshold));
    #endif
    while(fin>>src&&fin>>count)
    {
        actual_HHH_size[src]=count;
    }
	int total_packet_num=0;
	std::unordered_set<uint64_t> false_HHH;
	std::unordered_map<uint64_t,uint32_t> replace_other;
	std::unordered_map<uint64_t,uint32_t> decayed_time;
	std::unordered_map<uint64_t,uint32_t> replaced_time;
	std::unordered_map<uint64_t,uint32_t> est_size;
	std::unordered_map<uint64_t,uint32_t> est_concnt;
	std::unordered_map<uint64_t,uint32_t> insert_time;
    uint32_t hit_time=0;
	uint32_t judge_time=0;
    double start_time=now_us();
    threshold=threshold*D/(DEPTH+1);
    for(int i=0;i<size;i+=4)
	{
		total_packet_num++;
        uint32_t src=*(uint32_t*)(buffer+i);
		#if DEPTH==5
		insert(b,bucket_num,src,threshold);
		#else
		insert_test(b,bucket_num,src,threshold,query_keys,replace_other,replaced_time,insert_time,false_HHH,judge_time,hit_time);
//         insert_test(b,bucket_num,src,threshold);
		// insert(b,bucket_num,src,threshold);
		#endif
	}
    std::cout<<"insert done"<<std::endl;
	double end_time=now_us();
	double total_time=end_time-start_time;
	int actual_HHH_in_cell=0;
    int HHH_cell=0;
    int replace_cell=0;
	uint32_t level_sum[33];
	memset(level_sum,0,sizeof(level_sum));
	uint32_t actual_level_sum[33];
	memset(actual_level_sum,0,sizeof(actual_level_sum));
	std::unordered_set<uint64_t> HHH_in_cell_set;
	int false_cell=0;
	std::unordered_set<uint64_t> dups;
	int sum_empty=0;
    int min_time=100000000;
    int max_time=0;
    int sum_key_in_block=0;
    std::vector<int> bucket_keys_num;
    std::vector<int> bucket_keys_num_d;
    double relative_error=0;
//     for(int i=0;i<bucket_num;i++)
//     {
//         for(int j=0;j<CELL_NUM;j++)
//         {
//             uint64_t key=(b+i)->cells[j].key;
//             uint8_t level=(b+i)->cells[j].level;
//             if(level%2==1&&level<=15)
//             {
//                  trigger(b,i,key,1,threshold,0,query_keys,replace_other,replaced_time,insert_time,false_HHH,judge_time,hit_time);
//             }
//         }
//     }
	for(int i=0;i<bucket_num;i++)
    {
        if((i+1)%BLOCK_SIZE==0)
        {
            double abs_sum=0;
            int avg=sum_key_in_block/BLOCK_SIZE;
            for(auto num:bucket_keys_num)
            {
                abs_sum+=abs(num-avg)*abs(num-avg);
            }
            abs_sum=abs_sum/(int)bucket_keys_num.size();
            abs_sum=pow(abs_sum,0.5);
            bucket_keys_num_d.push_back(abs_sum);
            sum_key_in_block=0;
            bucket_keys_num.clear();
        }
//         bucket_keys_num.push_back((b+i)->keys.size());
//         sum_key_in_block+=(b+i)->keys.size();
        min_time=std::min(min_time,(b+i)->update_time);
        max_time=std::max(max_time,(b+i)->update_time);
        for(int j=0;j<CELL_NUM;j++)
        {
            uint32_t left_cnt=(b+i)->cells[j].left_child_count;
            uint32_t right_cnt=(b+i)->cells[j].right_child_count;
            uint32_t cell_val=left_cnt+right_cnt;
//             uint32_t cell_val=(b+i)->cells[j].count;
			int flag=0;
			if((b+i)->cells[j].is_empty)
			{
				sum_empty++;
				continue;
			}
            uint64_t key=(b+i)->cells[j].key;
            uint32_t level=(b+i)->cells[j].level;
			key=key&masks[level];
            uint64_t x=(key<<32)+level;
//             if((b+i)->cells[j].flag)
// 			{
// 				false_HHH.insert(x);
// 			}
            if(actual_HHH.find(x)!=actual_HHH.end())
            {
				level_sum[level]++;
                actual_HHH_in_cell++;
                relative_error+=abs((double)cell_val*(DEPTH+1)/D-actual_HHH_size[x])/actual_HHH_size[x];
//                 if(abs((double)cell_val*(DEPTH+1)/D-actual_HHH_size[x])/actual_HHH_size[x]>0.1)
//                 {
//                     std::cout<<x<<" "<<level<<" "<<cell_val*(DEPTH+1)/D<<" "<<actual_HHH_size[x]<<std::endl;
//                 }
				// if(dups.find(x)!=dups.end())
				// {
				// 	std::cout<<"dup:"<<x<<std::endl;
				// }
				// else
				// {
				// 	dups.insert(x);
				// }
				HHH_in_cell_set.insert(x);
				flag=1;
            }
			uint64_t left_child=0;
			uint64_t right_child=0;
			if(level<DEPTH-1)
			{
				left_child=(key<<32)+(level-1);
				right_child=key+(1<<(level-1));
				right_child=(right_child<<32)+(level-1);
//                 if((b+i)->cells[j].flag)
//                 {
//                     false_HHH.insert(left_child);
//                     false_HHH.insert(right_child);
//                 }
				if(actual_HHH.find(left_child)!=actual_HHH.end())
				{
					flag=1;
					actual_HHH_in_cell++;
					HHH_in_cell_set.insert(left_child);
                    relative_error+=abs((double)left_cnt*(DEPTH+1)/D-actual_HHH_size[left_child])/actual_HHH_size[left_child];
//                     if(abs((double)left_cnt*(DEPTH+1)/D-actual_HHH_size[left_child])/actual_HHH_size[left_child]>0.1)
//                 {
//                     std::cout<<left_child<<" "<<level-1<<" "<<left_cnt*(DEPTH+1)/D<<" "<<actual_HHH_size[left_child]<<std::endl;
//                 }
					level_sum[level-1]++;
				}
				if(actual_HHH.find(right_child)!=actual_HHH.end())
				{
					flag=1;
					actual_HHH_in_cell++;
					HHH_in_cell_set.insert(right_child);
                    relative_error+=abs((double)right_cnt*(DEPTH+1)/D-actual_HHH_size[right_child])/actual_HHH_size[right_child];
//                     if(abs((double)right_cnt*(DEPTH+1)/D-actual_HHH_size[right_child])/actual_HHH_size[right_child]>0.1)
//                 {
//                     std::cout<<right_child<<" "<<level-1<<" "<<right_cnt*(DEPTH+1)/D<<" "<<actual_HHH_size[right_child]<<std::endl;
//                 }
					level_sum[level-1]++;
				}
			}
			if(!flag)
			{
				false_cell++;
			}
            else
            {
                HHH_cell++;
//                 if((b+i)->cells[j].replace_time>3)
//                 {
//                     replace_cell++;
//                 }
            }
        }
    }
    relative_error/=actual_HHH_in_cell;
    std::sort(bucket_keys_num_d.begin(),bucket_keys_num_d.end());
    uint32_t sumd=0;
    for(auto d:bucket_keys_num_d)
    {
        sumd+=d;
    }
    int avg_d=sumd/(int)bucket_keys_num_d.size();
	int false_judge=0;
	for(auto actual_HHH_key:actual_HHH)
	{
		if(false_HHH.find(actual_HHH_key)!=false_HHH.end())
		{
			false_judge++;
		}
		if(HHH_in_cell_set.find(actual_HHH_key)==HHH_in_cell_set.end())
		{
			int level=actual_HHH_key&0xffffffff;
			std::cout<<actual_HHH_key<<" "<<level<<std::endl;
		}
		uint32_t l=actual_HHH_key&0xffffffff;
		actual_level_sum[l]++;
	}
	for(int i=0;i<33;i++)
	{
		std::cout<<i<<":"<<" "<<level_sum[i]<<" "<<actual_level_sum[i]<<std::endl;
	}
//     for(int i=0;i<BLOCK_SIZE;i++)
// 	{
// 		std::cout<<i<<":"<<" "<<replaced_time[i]<<std::endl;
// 	}
	std::cout<<"total_time:"<<total_time<<std::endl;
	std::cout<<"total_packet_num:"<<total_packet_num<<std::endl;
	std::cout<<"throughput:"<<total_packet_num/total_time<<std::endl;
    std::cout<<"total num of actual HHH"<<actual_HHH.size()<<std::endl;
    std::cout<<"actual_HHH_in_cell:"<<actual_HHH_in_cell<<std::endl;
	std::cout<<"false_cell_num:"<<false_cell<<std::endl;
	std::cout<<"sum_empty:"<<sum_empty<<std::endl;
    std::cout<<"min_time:"<<min_time<<std::endl;
    std::cout<<"max_time:"<<max_time<<std::endl;
	std::cout<<"judge_time:"<<judge_time<<std::endl;
    std::cout<<"hit_time:"<<hit_time<<std::endl;
	std::cout<<"false_judge:"<<false_judge<<" "<<false_HHH.size()<<std::endl;
    std::cout<<"max_d:"<<bucket_keys_num_d[0]<<std::endl;
    std::cout<<"avg_d:"<<avg_d<<std::endl;
    std::cout<<"relative_error:"<<relative_error<<std::endl;
    std::cout<<"replace_cell:"<<replace_cell<<" "<<HHH_cell<<std::endl;
	time_t t;
	std::vector<std::pair<uint64_t,uint32_t>> output=query_test(b,bucket_num,threshold,query_keys,est_size,est_concnt);
    std::cout<<"query done"<<std::endl;
	for(auto hhh:output)
	{
		fout<<hhh.first<<' '<<hhh.second<<std::endl;
	}
	for(auto key:query_keys)
	{
		std::cout<<key<<":"<<insert_time[key]<<" "<<est_concnt[key]<<" "<<est_size[key]<<" "<<replace_other[key]<<" "<<replaced_time[key]<<std::endl;
	}
	fout.close();
    return 0;
}