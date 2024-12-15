#include<fstream>
#include<unordered_map>
#include<vector>
#include<iostream>
#include<unordered_set>
#include<stdlib.h>
#include<string.h>
using namespace std;

#ifdef BIT
#define MAX_LEVEL 32
u_int32_t masks[33]={
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
#define MAX_LEVEL 4
u_int32_t masks[5]={
    0xFFFFFFFFu,
    0xFFFFFF00u,
    0xFFFF0000u,
    0xFF000000u,
    0x00000000u
};
#endif

int main(int argc,char *argv[])
{
    ifstream fin;
    fin.open("data_2D",std::ios::binary | std::ios::ate);
    uint32_t threshold=atoi(argv[1]);
    ofstream fout;
    int DEPTH=0; 
    #ifdef BIT
    DEPTH=65;
    fout.open("flow_actual_size_bit_2D"+to_string(threshold));
    #else
    fout.open("flow_actual_size_byte_2D"+to_string(threshold));
    DEPTH=9;
    #endif
    unordered_map<__uint128_t,u_int32_t> flow_size;
    int total_packet_num=0;
    vector<unordered_set<__uint128_t>> levels(DEPTH);
    std::streamsize size=fin.tellg();
    fin.seekg(0, std::ios::beg);
    char *buffer=(char*)malloc(size);
    fin.read(buffer,size);
    cout<<size<<endl;
    char* write_buffer=(char*)malloc(10*1024*1024);
    int write_size=0;
    for(int i=0;i<size;i+=8)
    {
        uint64_t srcdst=*(uint64_t*)(buffer+i);
        uint64_t src=srcdst>>32;
        uint64_t dst=srcdst&0xffffffff;
        for(int j=0;j<DEPTH;j++)
        {
            for(int k=0;k<=j;k++)
            {
                uint64_t src_level=k;
                uint64_t dst_level=j-k;
                if(k>MAX_LEVEL||j-k>MAX_LEVEL)
                {
                    continue;
                }
                src=src&masks[src_level];
                dst=dst&masks[dst_level];
                __uint128_t x=src;
                x=(x<<32)+dst;
                x=(x<<64)+(src_level<<32)+dst_level;
                levels[j].insert(x);
                flow_size[x]+=1;
            }
        }
        total_packet_num+=1;
    }
    uint32_t beyond_threshold=0;
    for(auto it=flow_size.begin();it!=flow_size.end();it++)
    {
        if(it->second>threshold)
        {
            beyond_threshold++;
        }
        __uint128_t x=it->first;
        memcpy(write_buffer+write_size,&x,sizeof(x));
        write_size+=sizeof(x);
        memcpy(write_buffer+write_size,&it->second,sizeof(it->second));
        write_size+=sizeof(it->second);
        if(write_size==10*1024*1024)
        {
            fout.write(write_buffer,write_size);
            write_size=0;
        }
    }
    if(write_size!=0)
    {
        fout.write(write_buffer,write_size);
        write_size=0;
    }
    cout<<beyond_threshold<<endl;
    fin.close();
    fout.close();
    #ifdef BIT
    fout.open("flow_conditional_count_bit_2D"+to_string(threshold));
    #else
    fout.open("flow_conditional_count_byte_2D"+to_string(threshold));
    #endif
    unordered_map<__uint128_t,u_int32_t> conditional_count;
    unordered_map<__uint128_t,u_int32_t> par;
    unordered_set<__uint128_t> HHH;
    unordered_set<__uint128_t> nearest;
    for(int i=0;i<DEPTH;i++)
    {
        for(auto x:levels[i])
        {
            int is_HHH=0;
            conditional_count[x]=flow_size[x];
            unordered_set<__uint128_t> toerase;
            uint64_t flow_id=x>>64;
            uint64_t level=x&0xffffffffffffffff;
            uint64_t src_level=x>>32;
            uint64_t dst_level=x&0xffffffff;
            for(auto y:nearest)
            {
                uint64_t tmp=y>>64;
                uint64_t src=tmp>>32;
                uint64_t dst=tmp&0xffffffff;
                uint64_t level=(y&0xffffffffffffffff);
                uint64_t src_level1=level>>32;
                uint64_t dst_level1=level&0xffffffff;
                src=src&masks[src_level1];
                dst=dst&masks[dst_level1];
                if(src_level1<=src_level&&dst_level1<=dst_level&&tmp==flow_id)
                {
                    conditional_count[x]-=flow_size[y];
                    toerase.insert(y);
                }
            }
            if(conditional_count[x]>threshold)
            {
                HHH.insert(x);
                for(auto y:toerase)
                {
                    nearest.erase(y);
                }
                nearest.insert(x);    
            }
            memcpy(write_buffer+write_size,&x,sizeof(x));
            write_size+=sizeof(x);
            memcpy(write_buffer+write_size,&conditional_count[x],sizeof(conditional_count[x]));
            write_size+=sizeof(conditional_count[x]);
            if(write_size==10*1024*1024)
            {
                fout.write(write_buffer,write_size);
                write_size=0;
            }
        }
    }
    if(write_size!=0)
    {
        fout.write(write_buffer,write_size);
        write_size=0;
    } 
    fout.close();
    return 0;
}