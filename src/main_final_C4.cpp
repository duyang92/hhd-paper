#include "our_C4.h"

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
    threshold=threshold*D*L/(36);
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