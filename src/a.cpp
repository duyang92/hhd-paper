#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<unordered_set>
#include<unordered_map>
#include<vector>
using namespace std;
int main()
{
    ifstream fin;
    fin.open("data",ios::binary|ios::ate);
    streamsize size=fin.tellg();
    fin.seekg(0,ios::beg);
    char *buffer=(char*)malloc(size);
    fin.read(buffer,size);
    fin.close();   
    uint64_t src=0;
    vector<uint32_t> allsrc;
    for(int i=0;i<size;i+=4)
    {
        uint32_t tmp=*(uint32_t*)(buffer+i);
        allsrc.push_back(tmp);
    }
    ofstream fout;
    fout.open("trace");
    for(int i=0;i<allsrc.size();i++)
    {
        uint32_t tmp=0;
        tmp=(allsrc[i]>>24)&0xFF;
        fout<<tmp<<" ";
        tmp=(allsrc[i]>>16)&0xFF;
        fout<<tmp<<" ";
        tmp=(allsrc[i]>>8)&0xFF;
        fout<<tmp<<" ";
        tmp=allsrc[i]&0xFF;
        fout<<tmp<<"\n";
    }
    fout.close();
}
