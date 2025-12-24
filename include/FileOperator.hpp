#pragma once


#ifdef DEBUG
    #include<iostream>
#endif

#include<fstream>
#include<string>

#pragma pack(1)

class FileOperator
{
private:
    const std::string file_name;
    std::fstream file;

public:
    explicit FileOperator(const std::string &file_name) noexcept;
    template<typename T> inline void read(int pos,T *t) noexcept;
    template<typename T> inline void write(int pos,T *t) noexcept;
    inline void read(int pos,int *t) noexcept;
    inline void write(int pos,int *t) noexcept;
    inline int size() noexcept;
};

#include"FileOperator.ipp"