#include<fstream>
#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<stdlib.h>
#include<string.h>
using namespace std;
int main(int argc,char *argv[])
{
    ifstream fin;
    uint32_t threshold=atoi(argv[1]);
    fin.open("output",ios::binary|ios::ate);
	std::ofstream fout;
	std::streamsize size=fin.tellg();
	fin.seekg(0,std::ios::beg);
    __uint128_t key;
    uint32_t count=0;
    unordered_map<__uint128_t,uint32_t> our_output;
    int HHH_num=0;
    char *buffer=(char*)malloc(size);
    fin.read(buffer,size);
    for(int i=0;i<size;i+=20)
    {
        key=*(__uint128_t*)(buffer+i);
        count=*(uint32_t*)(buffer+i+16);
        our_output[key]=count;
        if(count>threshold)
        {
            HHH_num++;
        }
    }
    fin.close();
    #ifdef BIT
    fin.open("flow_actual_size_bit_2D"+to_string(threshold),ios::binary|ios::ate);
    #else
    fin.open("flow_actual_size_byte_2D"+to_string(threshold),ios::binary|ios::ate);
    #endif
    unordered_map<__uint128_t,uint32_t> flow_actual_size;
    size=fin.tellg();
    fin.seekg(0,std::ios::beg);
    buffer=(char*)malloc(size);
    fin.read(buffer,size);
    for(int i=0;i<size;i+=20)
    {
        key=*(__uint128_t*)(buffer+i);
        count=*(uint32_t*)(buffer+i+16);
        flow_actual_size[key]=count;
        if(count>threshold)
        {
            HHH_num++;
        }
    }
    fin.close();
    #ifdef BIT
    fin.open("flow_conditional_count_bit_2D"+to_string(threshold),ios::binary|ios::ate);
    #else
    fin.open("flow_conditional_count_byte_2D"+to_string(threshold),ios::binary|ios::ate);
    #endif
    unordered_map<__uint128_t,uint32_t> flow_conditional_count;
    unordered_set<__uint128_t> actual_HHH;
    int is_HHH=0;
    uint32_t level_sum[33];
    memset(level_sum,0,sizeof(level_sum));
    size=fin.tellg();
    fin.seekg(0,std::ios::beg);
    buffer=(char*)malloc(size);
    fin.read(buffer,size);
    for(int i=0;i<size;i+=20)
    {
        key=*(__uint128_t*)(buffer+i);
        count=*(uint32_t*)(buffer+i+16);
        flow_conditional_count[key]=count;
        if(count>threshold)
        {
            actual_HHH.insert(key);
        }
    }
    int true_positive=0;
    int false_positive=0;
    int true_negative=0;
    int false_negative=0;
    for(auto it=our_output.begin();it!=our_output.end();it++)
    {
        if(actual_HHH.find(it->first)!=actual_HHH.end())
        {
            true_positive++;
        }
        else
        {
            false_positive++;
        }
    }
    for(auto it=actual_HHH.begin();it!=actual_HHH.end();it++)
    {
        if(our_output.find(*it)==our_output.end())
        {
            false_negative++;
        }
    }
    true_negative=flow_actual_size.size()-true_positive;
    double precision=(double)true_positive/(true_positive+false_positive);
    double recall=(double)true_positive/(true_positive+false_negative);
    cout<<"true_positive:"<<true_positive<<endl;
    cout<<"false_positive:"<<false_positive<<endl;
    cout<<"true_negative:"<<true_negative<<endl;
    cout<<"false_negative:"<<false_negative<<endl;
    cout<<"precision:"<<precision<<endl;
    cout<<"recall:"<<recall<<endl;
    double relative_error=0;
    for(auto it=our_output.begin();it!=our_output.end();it++)
    {
        relative_error+=abs((double)it->second-flow_actual_size[it->first])/(double)flow_actual_size[it->first];
    }
    relative_error/=our_output.size();
    cout<<"relative_error:"<<relative_error<<endl;
}