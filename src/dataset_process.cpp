#include<fstream>
#include<unordered_map>
#include<vector>
#include<iostream>
#include<unordered_set>
using namespace std;

#ifdef BIT
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
    fin.open("data",std::ios::binary | std::ios::ate);
    uint32_t threshold=atoi(argv[1]);
    ofstream fout;
    int DEPTH=0; 
    #ifdef BIT
    DEPTH=33;
    fout.open("flow_actual_size_bit_"+to_string(threshold));
    #else
    fout.open("flow_actual_size_byte_"+to_string(threshold));
    DEPTH=5;
    #endif
    char line[4];
    unordered_map<u_int64_t,u_int32_t> flow_size;
    int total_packet_num=0;
    vector<unordered_set<uint64_t>> levels(DEPTH);
    std::streamsize size=fin.tellg();
    fin.seekg(0, std::ios::beg);
    char *buffer=(char*)malloc(size);
    fin.read(buffer,size);
    cout<<size<<endl;
    for(int i=0;i<size;i+=4)
    {
        uint64_t src=*(uint32_t*)(buffer+i);
        for(int j=0;j<DEPTH;j++)
        {
            u_int64_t x=((src&masks[j])<<32)+j;
            levels[j].insert(x);
            flow_size[x]+=1;
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
        fout<<it->first<<' '<<it->second<<endl;
    }
    cout<<beyond_threshold<<endl;
    fin.close();
    fout.close();
    #ifdef BIT
    DEPTH=33;
    fout.open("flow_conditional_count_bit_"+to_string(threshold));
    #else
    fout.open("flow_conditional_count_byte_"+to_string(threshold));
    DEPTH=5;
    #endif
    unordered_map<u_int64_t,u_int32_t> conditional_count;
    unordered_map<u_int64_t,u_int32_t> par;
    unordered_set<u_int64_t> HHH;
    unordered_set<u_int64_t> nearest;
    for(int i=0;i<DEPTH;i++)
    {
        for(auto x:levels[i])
        {
            int is_HHH=0;
            conditional_count[x]=flow_size[x];
            unordered_set<uint64_t> toerase;
            for(auto y:nearest)
            {
                uint32_t tmp=y>>32;
                uint32_t x_level=x&0xffffffff;
                if((tmp&masks[x_level])==(x>>32))
                {
                    conditional_count[x]-=flow_size[y];
                    toerase.insert(y);
                }
            }
            if(conditional_count[x]>threshold)
            {
                is_HHH=1;
                HHH.insert(x);
                for(auto y:toerase)
                {
                    nearest.erase(y);
                }
                nearest.insert(x);
            }
            fout<<x<<' '<<conditional_count[x]<<' '<<is_HHH<<endl;
        }
    } 
    fout.close();
    return 0;
}