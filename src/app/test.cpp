#include<BlockList.hpp>
#include<iostream>
#include<climits>
#include<cstring>

struct Data
{
    static const int memory_size=4+64;
    char idx[64];
    int val;

    Data()=default;
    Data(const char *idx_tmp,int val_tmp)
    {
        memcpy(idx,idx_tmp,64);
        val=val_tmp;
    }
};
struct DataCompare
{
    bool operator () (Data a,Data b) const
    {
        if(strcmp(a.idx,b.idx)!=0) return strcmp(a.idx,b.idx)<0;
        return a.val<b.val;
    }
};

int main()
{
    BlockList<Data,DataCompare> bl("test.msav");
    std::cout<<"constructed"<<std::endl;

    int q,value;
    std::string op;
    char index[64];
    std::cin>>q;
    while(q--)
    {
        std::cin>>op;
        if(op=="insert")
        {
            std::cin>>index>>value;
            bl.insert(Data(index,value));
        }
        if(op=="find")
        {
            std::cin>>index;
            const std::vector<Data> &res=bl.find(Data(index,-1),Data(index,INT_MAX));
            std::cout<<"find done"<<std::endl;
            for(Data d:res) std::cout<<d.val<<" ";
            std::cout<<std::endl;
        }
        if(op=="delete")
        {
            std::cin>>index>>value;
            bl.remove(Data(index,value));
        }
        std::cout<<"done"<<std::endl;
        bl.print();
    }
}