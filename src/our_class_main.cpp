#include "our_class.h"

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
	uint32_t bucket_num=(std::atoi(argv[2])*1024*8/(double)102/CELL_NUM);
    our_class *our=new our_class(bucket_num,threshold);
    our->init();
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
	std::unordered_map<uint64_t,uint32_t> decay_other;
	std::unordered_map<uint64_t,uint32_t> decayed_time;
	std::unordered_map<uint64_t,uint32_t> replaced_time;
	std::unordered_map<uint64_t,uint32_t> est_size;
	std::unordered_map<uint64_t,uint32_t> est_concnt;
	std::unordered_map<uint64_t,uint32_t> insert_time;
    double start_time=now_us();
    for(int i=0;i<size;i+=4)
	{
		total_packet_num++;
        uint32_t src=*(uint32_t*)(buffer+i);
		#if DEPTH==5
		our->insert(bucket_num,src,threshold);
		#else
		our->insert(bucket_num,src,threshold);
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
            uint64_t key=our->get_b()[i].cells[j].key;
            uint8_t level=our->get_b()[i].cells[j].level;
			key=key&masks[level];
            uint64_t x=(key<<32)+level;
			HHH_in_cell_set.insert(x);
            if(actual_HHH.find(x)!=actual_HHH.end())
            {
                actual_HHH_in_cell++;
            }
			uint64_t left_child=0;
			uint64_t right_child=0;
			if(level<DEPTH-1)
			{
				left_child=(key<<32)+(level-1);
				right_child=key+(1<<(level-1));
				right_child=(right_child<<32)+(level-1);
				if(actual_HHH.find(left_child)!=actual_HHH.end())
				{
					actual_HHH_in_cell++;
				}
				if(actual_HHH.find(right_child)!=actual_HHH.end())
				{
					actual_HHH_in_cell++;
				}
			}
        }
    }
	std::cout<<"total_time:"<<total_time<<std::endl;
	std::cout<<"total_packet_num:"<<total_packet_num<<std::endl;
	std::cout<<"throughput:"<<total_packet_num/total_time<<std::endl;

    std::cout<<"total num of actual HHH"<<actual_HHH.size()<<std::endl;
    std::cout<<"actual_HHH_in_cell:"<<actual_HHH_in_cell<<std::endl;
	time_t t;
	srand((unsigned)(time(&t)));
	std::vector<std::pair<uint64_t,uint32_t>> output=our->query(bucket_num,threshold,query_keys,est_size,est_concnt);
	for(auto hhh:output)
	{
		fout<<hhh.first<<' '<<hhh.second<<std::endl;
	}
	for(auto key:query_keys)
	{
		std::cout<<key<<":"<<insert_time[key]<<" "<<est_concnt[key]<<" "<<est_size[key]<<" "<<decay_other[key]<<" "<<decayed_time[key]<<" "<<replaced_time[key]<<std::endl;
	}
	fout.close();
    return 0;
}