#pragma once

#include<BlockList.hpp>
#include<Utilities.hpp>
#include<User.hpp>
#include<iostream>
#include<iomanip>

const int MESSAGE_SIZE=300;

class FinanceLog
{
public:
    int tme;
    double inc,ouc;

    static const int memory_size=sizeof(int)+2*sizeof(double);

    inline FinanceLog()=default;
    inline FinanceLog(int _time,double _income,double _outcome) noexcept;

    class CompareByTime
    {
    public:
        inline bool operator () (const FinanceLog &a,const FinanceLog &b) const noexcept;
    };
};
class FinanceLogOperator
{
private:
    BlockList<FinanceLog,FinanceLog::CompareByTime> fncs;
    int now_time;

public:
    inline FinanceLogOperator() noexcept;
    inline void addFinanceLog(double _income,double _outcome) noexcept;
    inline void printLastKFinanceLog(int k) noexcept;
    inline void printFinanceTable() noexcept;
    inline int totFinanceCount() noexcept;
};

class OperationLog
{
public:
    int tme;
    char uid[UID_SIZE+1],msg[MESSAGE_SIZE+1];

    static const int memory_size=sizeof(int)+UID_SIZE+1+MESSAGE_SIZE+1;

    inline OperationLog()=default;
    inline OperationLog(int _time,const std::string &_uid,const std::string &_message) noexcept;

    class CompareByUID
    {
    public:
        inline bool operator () (const OperationLog &a,const OperationLog &b) const noexcept;
    };
};
class OperationLogOperator
{
private:
    BlockList<OperationLog,OperationLog::CompareByUID> opls;
    int now_time;

public:
    inline OperationLogOperator() noexcept;
    inline void addOperationLog(const std::string &_uid,const std::string &_message) noexcept;
    inline void reportAllOperationLog() noexcept;
};

#include"Log.ipp"
