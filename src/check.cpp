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
    std::unordered_set<uint64_t> query_keys;
	for(int i=2;i<argc;i++)
	{
		uint64_t query_key=std::stoull(argv[i]);
		query_keys.insert(query_key);
	}
    fin.open("output");
    uint64_t src=0;
    uint32_t count=0;
    unordered_map<uint64_t,uint32_t> our_output;
    int HHH_num=0;
    while(fin>>src&&fin>>count)
    {
        HHH_num++;
        our_output[src]=count;
    }
    fin.close();
    #ifdef BIT
    fin.open("flow_actual_size_bit_"+to_string(threshold));
    #else
    fin.open("flow_actual_size_byte_"+to_string(threshold));
    #endif
    unordered_map<uint64_t,uint32_t> flow_actual_size;
    while(fin>>src&&fin>>count)
    {
        flow_actual_size[src]=count;
    }
    cout<<flow_actual_size.size()<<endl;
    fin.close();
    #ifdef BIT
    fin.open("flow_conditional_count_bit_"+to_string(threshold));
    #else
    fin.open("flow_conditional_count_byte_"+to_string(threshold));
    #endif
    unordered_map<uint64_t,uint32_t> flow_conditional_count;
    unordered_set<uint64_t> actual_HHH;
    int is_HHH=0;
    uint32_t level_sum[33];
    memset(level_sum,0,sizeof(level_sum));
    while(fin>>src&&fin>>count&&fin>>is_HHH)
    {
        flow_conditional_count[src]=count;
        uint32_t level=src&0xffffffff;
        if(is_HHH)
        {
            actual_HHH.insert(src);
        }
    }
    int true_positive=0;
    int false_positive=0;
    int true_negative=0;
    int false_negative=0;
    cout<<"false_positive:"<<endl;
    double relative_error=0;
    for(auto it=our_output.begin();it!=our_output.end();it++)
    {
            if(actual_HHH.find(it->first)!=actual_HHH.end())
        {
            true_positive++;
           relative_error+=abs((double)it->second-flow_actual_size[it->first])/(double)flow_actual_size[it->first];
        }
        else
        {
            false_positive++;
            uint32_t level=it->first&0xffffffff;
        }
    }
    cout<<"false_negative:"<<endl;
    for(auto it=actual_HHH.begin();it!=actual_HHH.end();it++)
    {
        uint32_t tmp=*it>>32;
        uint32_t level=*it&0xffffffff;
        level_sum[level]++;
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
    relative_error/=true_positive;
    cout<<"relative_error:"<<relative_error<<endl;
}