#include<Log.hpp>

FinanceLog::FinanceLog(int _time,double _income,double _outcome) noexcept:
    tme(_time),inc(_income),ouc(_outcome)
{}
bool FinanceLog::CompareByTime::operator () (const FinanceLog &a,const FinanceLog &b) const noexcept
{
    if(a.tme!=b.tme) return a.tme<b.tme;
    if(Utils::comp(a.inc,b.inc)!=0) return Utils::comp(a.inc,b.inc)<0;
    return Utils::comp(a.ouc,b.ouc)<0;
}

FinanceLogOperator::FinanceLogOperator() noexcept: fncs("finance.dat")
{
    now_time=fncs.size();
}
void FinanceLogOperator::addFinanceLog(double _income,double _outcome) noexcept
{
    fncs.insert(FinanceLog(++now_time,_income,_outcome));
}
void FinanceLogOperator::printLastKFinanceLog(int k) noexcept
{
    const std::vector<FinanceLog> &tmp=fncs.find
    (
        FinanceLog(now_time-k+1,-1,-1),
        FinanceLog(now_time+1,-1,-1)
    );
    for(const FinanceLog &e:tmp)
    {
        std::cout<<std::setprecision(2)<<std::fixed<<"+ "<<e.inc<<" - "<<e.ouc<<"\n";
    }
    std::cout.flush();
}
void FinanceLogOperator::printFinanceTable() noexcept
{
    const std::vector<FinanceLog> &tmp=fncs.find
    (
        FinanceLog(0,-1,-1),
        FinanceLog(now_time+1,-1,-1)
    );

    for(const FinanceLog &e:tmp)
    {
        std::cout<<"Finance log #"<<e.tme<<" : income = "<<e.inc<<" , outcome = "<<e.ouc<<'\n';
    }
    std::cout.flush();
}
int FinanceLogOperator::totFinanceCount() noexcept {return now_time;}


OperationLog::OperationLog(int _time,const std::string &_uid,const std::string &_message) noexcept
{
    assert(_uid    .size()<=UID_SIZE),memcpy(uid,_uid    .c_str(),strlen(_uid    .c_str())+1);
    assert(_message.size()<=UID_SIZE),memcpy(msg,_message.c_str(),strlen(_message.c_str())+1);
    tme=_time;
}
bool OperationLog::CompareByUID::operator () (const OperationLog &a,const OperationLog &b) const noexcept
{
    int tmpUID=strcmp(a.uid,b.uid);
    if(tmpUID!=0) return tmpUID<0;
    if(a.tme!=b.tme) return a.tme<b.tme;
    int tmpMsg=strcmp(a.msg,b.msg);
    return tmpMsg<0;
}

OperationLogOperator::OperationLogOperator() noexcept: opls("operations.dat")
{
    now_time=opls.size();
}
void OperationLogOperator::addOperationLog(const std::string &_uid,const std::string &_message) noexcept
{
    opls.insert(OperationLog(++now_time,_uid,_message));
}
void OperationLogOperator::reportAllOperationLog() noexcept
{
    const std::vector<OperationLog> &tmp=opls.find
    (
        OperationLog(-1,"",""),
        OperationLog(-1,"~","~")
    );
    
    for(const OperationLog &e:tmp)
    {
        std::cout<<"OperationLog #"<<e.tme<<" : User "<<e.uid<<" "<<e.msg<<'\n';
    }
    std::cout.flush();
}