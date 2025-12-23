#include<Book.hpp>
#include<iostream>

int main()
{
    std::string ISBN,name,auth,key;
    int cnt,cost,tot_cost;
    std::cin>>ISBN>>name>>auth>>key;
    std::cin>>cnt>>cost>>tot_cost;

    Book a(ISBN,name,auth,key,cnt,cost,tot_cost);

    std::cout<<a.ISBN<<std::endl;
}