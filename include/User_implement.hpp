#include<User.hpp>

// User part ***********************************************************************

User::User(const std::string &_uid,const std::string &_passwd,const std::string &_name,int _per) noexcept
{
    assert(_uid   .size()<=UID_SIZE),memcpy(uid   ,_uid   .c_str(),strlen(_uid   .c_str())+1);
    assert(_passwd.size()<=UID_SIZE),memcpy(passwd,_passwd.c_str(),strlen(_passwd.c_str())+1);
    assert(_name  .size()<=UID_SIZE),memcpy(name  ,_name  .c_str(),strlen(_name  .c_str())+1);
    per=_per;
}

bool User::CompareByUID::operator () (const User &a,const User &b) const noexcept
{
    int tmpUID=strcmp(a.uid,b.uid);
    if(tmpUID!=0) return tmpUID<0;
    int tmpPasswd=strcmp(a.passwd,b.passwd);
    if(tmpPasswd!=0) return tmpPasswd<0;
    int tmpName=strcmp(a.name,b.name);
    if(tmpName!=0) return tmpName<0;
    return a.per<b.per;
}
// *********************************************************************************


// UserOperator part ***************************************************************

UserOperator::UserOperator() noexcept:users("users.dat"){}

std::pair<bool,User> UserOperator::findUserByUID(const std::string &uid) noexcept
{
    const std::vector<User> &tmp=users.find
    (
        User(uid,"" ,"" ,0),
        User(uid,"~~~~~~~~~~","~~~~~~~~~~",8)
    );

    if(tmp.size()!=1) return std::make_pair(0,User("~~~~~~~~~~","~~~~~~~~~~","~~~~~~~~~~",0));
    return std::make_pair(1,tmp.front());
}

bool UserOperator::insertUser(const std::string &_uid,const std::string &_passwd,const std::string &_name,int _per) noexcept
{
    if(findUserByUID(_uid).first) return 0;
    users.insert(User(_uid,_passwd,_name,_per));
    return 1;
}
bool UserOperator::removeUser(const std::string &_uid) noexcept
{
    auto [b,u]=findUserByUID(_uid);
    if(b==0) return 0;
    users.remove(u);
    return 1;
}
bool UserOperator::changePassword(const std::string &_uid,const std::string &_passwd,const std::string &new_passwd,int backdoor) noexcept
{
    auto [b,u]=findUserByUID(_uid);
    if(b==0||(backdoor==0&&strcmp(_passwd.c_str(),u.passwd))!=0) return 0;
    users.remove(u);
    users.insert(User(u.uid,new_passwd,u.name,u.per));
    return 1;
}

// *********************************************************************************