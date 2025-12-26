#pragma once

#include<BlockList.hpp>

const int UID_SIZE=30;
const int PASSWD_SIZE=30;
const int USERNAME_SIZE=30;

class User
{
public:
    char uid[UID_SIZE+1],passwd[PASSWD_SIZE+1],name[USERNAME_SIZE+1];
    int per;

    static const int memory_size=(UID_SIZE+1+PASSWD_SIZE+1+USERNAME_SIZE+1)+sizeof(int);

    inline User()=default;
    inline User(const std::string &_uid,const std::string &_passwd,const std::string &_name,int _per) noexcept;

    class CompareByUID
    {
    public:
        inline bool operator () (const User &a,const User &b) const noexcept;
    };
};

class UserOperator
{
private:
    BlockList<User,User::CompareByUID> users;

public:
    inline UserOperator() noexcept;
    inline std::pair<bool,User> findUserByUID(const std::string &uid) noexcept;

    inline bool insertUser(const std::string &_uid,const std::string &_passwd,const std::string &_name,int _per) noexcept;
    inline bool removeUser(const std::string &_uid) noexcept;
    inline bool changePassword(const std::string &_uid,const std::string &_passwd,const std::string &new_passwd,int backdoor=0) noexcept;
};

#include<User_implement.hpp>