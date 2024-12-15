#include "our_2D.h"

#include <stdlib.h>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include <time.h>
#include<sys/time.h>
#ifdef BIT
   uint32_t masks[33]={
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
#else
    uint32_t masks[5]={
        0xFFFFFFFFu,
        0xFFFFFF00u,
        0xFFFF0000u,
        0xFF000000u,
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
	uint32_t bucket_num=(std::atoi(argv[2])*1024*8/(double)104/CELL_NUM);
    BUCKET b[bucket_num];
    init(b,bucket_num);
	std::ifstream fin;
	fin.open("data_2D",std::ios::binary|std::ios::ate);
	std::ofstream fout;
	std::streamsize size=fin.tellg();
	fin.seekg(0,std::ios::beg);
	fout.open("output");
	char *buffer=(char*)malloc(size);
    fin.read(buffer,size);
	fin.close();
	fin.open("flow_actual_size");
	int total_packet_num=0;
	double start_time=now_us();
    for(int i=0;i<size;i+=8)
	{
		total_packet_num++;
        uint64_t src=*(uint32_t*)(buffer+i);
        uint64_t dst=*(uint32_t*)(buffer+i+4);
        uint64_t key=(src<<32)+dst;
		#ifdef BIT
		insert_bit(b,bucket_num,key,threshold);
		#else
		insert(b,bucket_num,key,threshold);
		#endif
	}
	double end_time=now_us();
	double total_time=end_time-start_time;
	std::cout<<"total_time:"<<total_time<<std::endl;
	std::cout<<"total_packet_num:"<<total_packet_num<<std::endl;
	std::cout<<"throughput:"<<total_packet_num/total_time<<std::endl;
	time_t t;
	srand((unsigned)(time(&t)));
	std::vector<std::pair<__uint128_t,uint32_t>> output=query(b,bucket_num,threshold);
	for(auto hhh:output)
	{
		fout.write((char*)&hhh.first,sizeof(__uint128_t));
        fout.write((char*)&hhh.second,sizeof(uint32_t));
	}
	fout.close();
    return 0;
}