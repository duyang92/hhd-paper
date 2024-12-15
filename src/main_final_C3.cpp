#include "our_C3.h"

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
    BUCKET b[BUCKET_NUM];
    init(b,BUCKET_NUM);
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
    uint32_t hit_time=0;
	uint32_t judge_time=0;
    threshold=threshold*D*3/(DEPTH);
    double start_time=now_us();
    for(int i=0;i<size;i+=4)
	{
		total_packet_num++;
        uint32_t src=*(uint32_t*)(buffer+i);
		#if DEPTH==5
		insert(b,BUCKET_NUM,src,threshold);
		#else
        insert_bit(b,BUCKET_NUM,src,threshold);
		#endif
	}
	double end_time=now_us();
	double total_time=end_time-start_time;
    std::cout<<"insert done"<<std::endl;
	int actual_HHH_in_cell=0;
    int HHH_cell=0;
    int replace_cell=0;
    int sum_empty=0;
	uint32_t level_sum[33];
	memset(level_sum,0,sizeof(level_sum));
	uint32_t actual_level_sum[33];
	memset(actual_level_sum,0,sizeof(actual_level_sum));
	std::unordered_set<uint64_t> HHH_in_cell_set;
	int false_cell=0;
    double relative_error=0;
	for(int i=0;i<BUCKET_NUM;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            uint32_t right_cnt=(b+i)->cells[j].right_child_count;
            uint32_t cell_val=(b+i)->cells[j].count;
            uint32_t left_cnt=cell_val-right_cnt;
            uint32_t left_left_cnt=left_cnt-(b+i)->cells[j].left_right_child_count;
            uint32_t left_right_cnt=(b+i)->cells[j].left_right_child_count;
            uint32_t right_left_cnt=(b+i)->cells[j].right_child_count-(b+i)->cells[j].right_right_child_count;
            uint32_t right_right_cnt=(b+i)->cells[j].right_right_child_count;
			int flag=0;
			if((b+i)->cells[j].key==0&&(b+i)->cells[j].level==0)
			{
				sum_empty++;
				continue;
			}
            uint64_t key=(b+i)->cells[j].key;
            uint32_t level=(b+i)->cells[j].level;
			key=key&masks[level];
            uint64_t x=(key<<32)+level;
            if(actual_HHH.find(x)!=actual_HHH.end())
            {
                level_sum[level]++;
                actual_HHH_in_cell++;
                relative_error+=abs((double)cell_val*(DEPTH)/D-actual_HHH_size[x])/actual_HHH_size[x];
                HHH_in_cell_set.insert(x);
                flag=1;
            }
            uint64_t left_child=0;
            uint64_t right_child=0;
            uint64_t left_left_child=0;
            uint64_t left_right_child=0;
            uint64_t right_left_child=0;
            uint64_t right_right_child=0;
            left_child=(key<<32)+(level-1);
            right_child=key+(1<<(level-1));
            right_child=(right_child<<32)+(level-1);
            left_left_child=(key<<32)+(level-2);
            left_right_child=(key+(1<<(level-2)));
            left_right_child=(left_right_child<<32)+(level-2);
            right_left_child=key+(1<<(level-1));
            right_left_child=(right_left_child<<32)+(level-2);
            right_right_child=key+(1<<(level-1))+(1<<(level-2));
            right_right_child=(right_right_child<<32)+(level-2);
            if(actual_HHH.find(left_child)!=actual_HHH.end())
            {
                flag=1;
                actual_HHH_in_cell++;
                HHH_in_cell_set.insert(left_child);
                relative_error+=abs((double)left_cnt*(DEPTH)/D/3-actual_HHH_size[left_child])/actual_HHH_size[left_child];
                level_sum[level-1]++;
            }
            if(actual_HHH.find(right_child)!=actual_HHH.end())
            {
                flag=1;
                actual_HHH_in_cell++;
                HHH_in_cell_set.insert(right_child);
                relative_error+=abs((double)right_cnt*(DEPTH)/D/3-actual_HHH_size[right_child])/actual_HHH_size[right_child];
                level_sum[level-1]++;
            }
            if(actual_HHH.find(left_left_child)!=actual_HHH.end())
            {
                flag=1;
                actual_HHH_in_cell++;
                HHH_in_cell_set.insert(left_left_child);
                relative_error+=abs((double)left_left_cnt*(DEPTH)/D/3-actual_HHH_size[left_left_child])/actual_HHH_size[left_left_child];
                level_sum[level-2]++;
            }
            if(actual_HHH.find(left_right_child)!=actual_HHH.end())
            {
                flag=1;
                actual_HHH_in_cell++;
                HHH_in_cell_set.insert(left_right_child);
                relative_error+=abs((double)left_right_cnt*(DEPTH)/D/3-actual_HHH_size[left_right_child])/actual_HHH_size[left_right_child];
                level_sum[level-2]++;
            }
            if(actual_HHH.find(right_left_child)!=actual_HHH.end())
            {
                flag=1;
                actual_HHH_in_cell++;
                HHH_in_cell_set.insert(right_left_child);
                relative_error+=abs((double)right_left_cnt*(DEPTH)/D/3-actual_HHH_size[right_left_child])/actual_HHH_size[right_left_child];
                level_sum[level-2]++;
            }
            if(actual_HHH.find(right_right_child)!=actual_HHH.end())
            {
                flag=1;
                actual_HHH_in_cell++;
                HHH_in_cell_set.insert(right_right_child);
                relative_error+=abs((double)right_right_cnt*(DEPTH)/D/3-actual_HHH_size[right_right_child])/actual_HHH_size[right_right_child];
                level_sum[level-2]++;
            }
        }
    }
    relative_error/=actual_HHH_in_cell;
	std::cout<<"total_time:"<<total_time<<std::endl;
	std::cout<<"total_packet_num:"<<total_packet_num<<std::endl;
	std::cout<<"throughput:"<<total_packet_num/total_time<<std::endl;
    std::cout<<"total num of actual HHH"<<actual_HHH.size()<<std::endl;
    std::cout<<"actual_HHH_in_cell:"<<actual_HHH_in_cell<<std::endl;
	std::cout<<"false_cell_num:"<<false_cell<<std::endl;
	std::cout<<"sum_empty:"<<sum_empty<<std::endl;
    std::cout<<"relative_error:"<<relative_error<<std::endl;
	time_t t;
	std::vector<std::pair<uint64_t,uint32_t>> output=query(b,BUCKET_NUM,threshold);
    std::cout<<"query done"<<std::endl;
	for(auto hhh:output)
	{
		fout<<hhh.first<<' '<<hhh.second<<std::endl;
	}
	fout.close();
    return 0;
}