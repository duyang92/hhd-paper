#include "our.h"

#include <stdlib.h>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include <time.h>
#include<sys/time.h>
#include<string.h>
#include<unordered_set>
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
	uint32_t bucket_num=(std::atoi(argv[2])*1024*8/(double)70/CELL_NUM);
    BUCKET b[bucket_num];
    init(b,bucket_num);
	std::ifstream fin;
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
	int total_packet_num=0;
	double start_time=now_us();
    for(int i=0;i<size;i+=4)
	{
		total_packet_num++;
        uint32_t src=*(uint32_t*)(buffer+i);
		#if DEPTH==5
		insert(b,bucket_num,src,threshold);
		#else
		insert_bit(b,bucket_num,src,threshold);
		#endif
	}
	double end_time=now_us();
	double total_time=end_time-start_time;
	int actual_HHH_in_cell=0;
	uint32_t level_sum[33];
	memset(level_sum,0,sizeof(level_sum));
	std::unordered_set<uint64_t> HHH_in_cell_set;
	for(int i=0;i<bucket_num;i++)
    {
        for(int j=0;j<CELL_NUM;j++)
        {
            uint64_t key=(b+i)->cells[j].key;
            uint8_t level=(b+i)->cells[j].level;
			key=key&masks[level];
            uint64_t x=(key<<32)+level;
			HHH_in_cell_set.insert(x);
            if(actual_HHH.find(x)!=actual_HHH.end())
            {
                actual_HHH_in_cell++;
            }
        }
    }
	for(auto x:actual_HHH)
	{
		if(HHH_in_cell_set.find(x)==HHH_in_cell_set.end())
		{
			uint32_t level=x&0xffffffff;
			level_sum[level]++;
			std::cout<<x<<" "<<level<<std::endl;
		}
	}
	std::cout<<"total_time:"<<total_time<<std::endl;
	std::cout<<"total_packet_num:"<<total_packet_num<<std::endl;
	std::cout<<"throughput:"<<total_packet_num/total_time<<std::endl;

    std::cout<<"total num of actual HHH"<<actual_HHH.size()<<std::endl;
    std::cout<<"actual_HHH_in_cell:"<<actual_HHH_in_cell<<std::endl;
	time_t t;
	srand((unsigned)(time(&t)));
	std::vector<std::pair<uint64_t,uint32_t>> output=query(b,bucket_num,threshold);
	for(auto hhh:output)
	{
		fout<<hhh.first<<' '<<hhh.second<<std::endl;
	}
	fout.close();
    return 0;
}