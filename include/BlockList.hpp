#pragma once

#ifdef DEBUG
    #include<iostream>
#endif

#include<FileOperator.hpp>
#include<algorithm>
#include<vector>
#include<utility>
#include<cassert>
#include<cstring>

template<typename T>
class SemiBlock
{
public:
    int this_pos,prev_pos,next_pos,siz;
    T max_e;

    static const int memory_size=4*sizeof(int)+T::memory_size;

    explicit SemiBlock(int this_pos,FileOperator &fo) noexcept;
    inline int prev() noexcept;
    inline int next() noexcept;
    inline int pos() noexcept;
    inline T maxElement() noexcept;
};

template<typename T>
class Block
{
private:
    int this_pos,prev_pos,next_pos,siz;

public:
    static const int block_max_size=1000;
    static const int memory_size=4*4+T::memory_size*(block_max_size+10);
    T data[block_max_size+10];

    explicit Block(int this_pos,FileOperator &fo) noexcept;
    inline T& at(int i) noexcept;
    inline void write(FileOperator &fo) noexcept;
    inline int& prev() noexcept;
    inline int& next() noexcept;
    inline int& pos() noexcept;
    inline int& size() noexcept;
    inline T maxElement() noexcept;
    inline void copyInsert(int l,int r,std::vector<T> &res) noexcept;
};

template<typename T,typename Compare>
class BlockList
{
private:
    int node_head_pos;
    FileOperator fo;
    Compare comp;

    inline std::pair<int,int> lowerBound(const T &t) noexcept;

    inline bool trySplit(Block<T> &b) noexcept;
    inline bool tryMerge(Block<T> &b) noexcept;      //merge this and next

public:
    explicit BlockList(const std::string &file_name) noexcept;
    ~BlockList() noexcept;

    inline void insert(const T &t) noexcept;
    inline bool remove(const T &t) noexcept;
    inline std::vector<T> find(const T &l,const T&r) noexcept; // find [l,r)

#ifdef DEBUG
    inline void print() noexcept;
#endif
};

#include"Blocklist.ipp"