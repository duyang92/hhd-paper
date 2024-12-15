
#include <stdlib.h>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include "lib/datatypes.hpp"
#include"mvpipe.hpp"
#include <time.h>
#include<sys/time.h>
#if NUM_MASKS==5
KEYTYPE masks[NUM_MASKS] = {
    0xFFFFFFFFu,
	0xFFFFFF00u,
	0xFFFF0000u,
	0xFF000000u,
	0x00000000u,
};

int width[NUM_MASKS] = {0};


#endif
#if NUM_MASKS==33
KEYTYPE masks[NUM_MASKS] = {
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
int width[NUM_MASKS] = {0};
#endif
inline double now_us ()
{
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return (tv.tv_sec * (uint64_t) 1000000 + (double)tv.tv_nsec/1000);
}
int main(int argc,char *argv[])
{
	int lgn = sizeof(KEYTYPE)*8;
	int threshold = atoi(argv[1]);
	int memory = atoi(argv[2]);
	int total_bucket = memory*8*1024/128;
	int row_bucket = total_bucket/NUM_MASKS;
	int flag = 0;
	for (int i  = NUM_MASKS; i > 0; i--) {
		long total = (long)1<<((NUM_MASKS-i)*33/(NUM_MASKS-1));
		if (total < (long)row_bucket) {
			flag = NUM_MASKS-i;
			width[i-1] = (int)total;
			total_bucket -= (int)total;
			row_bucket = total_bucket/(i-1);
		} else {
			width[i-1] = row_bucket;
		}
	}
    MVPipe *mv = new MVPipe(NUM_MASKS, width, lgn, NUM_MASKS-flag-1);
	std::ifstream fin;
	fin.open("data2016",std::ios::binary|std::ios::ate);
	std::ofstream fout;
	std::streamsize size=fin.tellg();
	fin.seekg(0,std::ios::beg);
	fout.open("output");
	char *buffer=(char*)malloc(size);
    fin.read(buffer,size);
	int total_packet_num=0;
	std::cout<<size<<std::endl;
	double start_time=now_us();

    for(int i=0;i<size;i+=4)
	{
        uint32_t src=*(uint32_t*)(buffer+i);
		mv->Update(0,src,1);
		total_packet_num++;
	}
	double end_time=now_us();
	double total_time=end_time-start_time;
	std::cout<<"total_time:"<<total_time<<std::endl;
	std::cout<<"total_packet_num:"<<total_packet_num<<std::endl;
	std::cout<<"throughput:"<<total_packet_num/total_time<<std::endl;
	std::vector<std::pair<unsigned long, int> > output;
	mv->Query(threshold,output);
	for(auto it=output.begin();it!=output.end();it++)
	{
		fout<<it->first<<" "<<it->second<<std::endl;
	}
	fout.close();
    return 0;
}