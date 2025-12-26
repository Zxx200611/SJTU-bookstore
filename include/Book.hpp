#pragma once

#include<BlockList.hpp>
#include<Utilities.hpp>
#include<cstring>

const int ISBN_SIZE=20;
const int BOOKNAME_SIZE=60;
const int AUTHOR_SIZE=60;
const int KEY_SIZE=60;

class Book
{
public:
    char ISBN[ISBN_SIZE+1],name[BOOKNAME_SIZE+1],auth[AUTHOR_SIZE+1],key[KEY_SIZE+1];
    int cnt;
    double cost,tot_cost;

    static const int memory_size=(ISBN_SIZE+1+BOOKNAME_SIZE+1+AUTHOR_SIZE+1+KEY_SIZE+1)+sizeof(int)+2*sizeof(double);

    inline Book()=default;
    inline Book(const std::string &_ISBN,const std::string &_name,const std::string &_auth,
                const std::string &_key,int cnt,double cost,double tot_cost) noexcept;

    class CompareByISBN
    {
    public:
        inline bool operator () (const Book &a,const Book &b) const noexcept;
    };
};

class StringAndISBN
{
public:
    char index[BOOKNAME_SIZE+1];
    char ISBN[ISBN_SIZE+1];

    static const int memory_size=BOOKNAME_SIZE+1+ISBN_SIZE+1;

    inline StringAndISBN()=default;
    inline StringAndISBN(const std::string &_index,const std::string &ISBN) noexcept;
    
    class CompareByIndex
    {
    public:
        inline bool operator () (const StringAndISBN &a,const StringAndISBN &b) const noexcept;
    };
};
class BookOperator
{
private:
    BlockList<Book,Book::CompareByISBN> books;
    BlockList<StringAndISBN,StringAndISBN::CompareByIndex> name_to_ISBN,auth_to_ISBN,key_to_ISBN;

public:
    inline BookOperator() noexcept;
    inline std::pair<bool,Book> findBookByISBN(const std::string &ISBN) noexcept;
    inline std::vector<std::string> getISBNByName(const std::string &name) noexcept;
    inline std::vector<std::string> getISBNByAuthor(const std::string &auth) noexcept;
    inline std::vector<std::string> getISBNByKey(const std::string &key) noexcept;
    inline std::vector<Book> allBooks() noexcept;

    inline bool insertBook(const std::string &_ISBN,const std::string &_name,const std::string &_auth,
                           const std::string &_key,int cnt,double cost,double tot_cost) noexcept;
    inline bool removeBook(const std::string &ISBN) noexcept;


    inline bool updateBookData(const std::string &ISBN,const std::string &new_ISBN,const std::string &name,
                               const std::string &auth,const std::string &key,double cost) noexcept;
    inline bool updateBookQuantity(const std::string &ISBN,int delta,double delta_tot_cost) noexcept;
};

#include<Book.ipp>