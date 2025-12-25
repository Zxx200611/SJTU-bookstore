#include<Book.hpp>

// Book part ***********************************************************************

Book::Book(const std::string &_ISBN,const std::string &_name,const std::string &_auth,const std::string &_key,int _cnt,int _cost,int _tot_cost) noexcept
{
    assert(_ISBN.size()<=ISBN_SIZE    ),memcpy(ISBN,_ISBN.c_str(),strlen(_ISBN.c_str())+1);
    assert(_name.size()<=BOOKNAME_SIZE),memcpy(name,_name.c_str(),strlen(_name.c_str())+1);
    assert(_auth.size()<=AUTHOR_SIZE  ),memcpy(auth,_auth.c_str(),strlen(_auth.c_str())+1);
    assert(_key .size()<=KEY_SIZE     ),memcpy(key ,_key .c_str(),strlen(_key .c_str())+1);
    cnt=_cnt,cost=_cost,tot_cost=_tot_cost;
}
bool Book::CompareByISBN::operator () (const Book &a,const Book &b) const noexcept
{
    int tmpISBN=strcmp(a.ISBN,b.ISBN);
    if(tmpISBN!=0) return tmpISBN<0;
    int tmpName=strcmp(a.name,b.name);
    if(tmpName!=0) return tmpName<0;
    int tmpAuth=strcmp(a.auth,b.auth);
    if(tmpAuth!=0) return tmpAuth<0;
    int tmpKey =strcmp(a.key ,b.key );
    if(tmpKey !=0) return tmpKey <0;
    
    if(a.cnt!=b.cnt) return a.cnt<b.cnt;
    if(Utils::comp(a.cost,b.cost)!=0) return Utils::comp(a.cost,b.cost)<0;    // here is a bug: should use eps
    return Utils::comp(a.tot_cost,b.tot_cost)<0;
}

// *********************************************************************************


// BookOperator part****************************************************************

StringAndISBN::StringAndISBN(const std::string &_index,const std::string &_ISBN) noexcept
{
    memcpy(index,_index.c_str(),strlen(_index.c_str())+1);
    memcpy(ISBN ,_ISBN .c_str(),strlen(_ISBN .c_str())+1);
}
bool StringAndISBN::CompareByIndex::operator () (const StringAndISBN &a,const StringAndISBN &b) const noexcept
{
    int tmpIndex=strcmp(a.index,b.index);
    if(tmpIndex!=0) return tmpIndex<0;
    return strcmp(a.ISBN,b.ISBN)<0;
}


BookOperator::BookOperator() noexcept:
    books("books.dat"),name_to_ISBN("name_to_ISBN.dat"),
    auth_to_ISBN("auth_to_ISBN.dat"),key_to_ISBN("key_to_ISBN.dat")
{}
std::pair<bool,Book> BookOperator::findBookByISBN(const std::string &ISBN) noexcept
{
    const std::vector<Book> &tmp=books.find
    (
        Book(ISBN,"" ,"" ,"" ,-1,-1,-1),
        Book(ISBN,"~","~","~",-1,-1,-1)
    );
    assert(tmp.size()<=1);
    if(tmp.size()==0) return std::make_pair(0,Book("~","~","~","~",-1,-1,-1));
    return std::make_pair(1,tmp.front());
}
std::vector<std::string> BookOperator::getISBNByName(const std::string &name) noexcept
{
    const std::vector<StringAndISBN> &tmp=name_to_ISBN.find
    (
        StringAndISBN(name,"" ),
        StringAndISBN(name,"~")
    );
    std::vector<std::string> tmp2(0);
    for(const StringAndISBN &e:tmp) tmp2.emplace_back(e.ISBN);
    return tmp2;
}
std::vector<std::string> BookOperator::getISBNByAuthor(const std::string &auth) noexcept
{
    const std::vector<StringAndISBN> &tmp=name_to_ISBN.find
    (
        StringAndISBN(auth,"" ),
        StringAndISBN(auth,"~")
    );
    std::vector<std::string> tmp2(0);
    for(const StringAndISBN &e:tmp) tmp2.emplace_back(e.ISBN);
    return tmp2;
}
std::vector<std::string> BookOperator::getISBNByKey(const std::string &key) noexcept
{
    const std::vector<StringAndISBN> &tmp=name_to_ISBN.find
    (
        StringAndISBN(key,"" ),
        StringAndISBN(key,"~")
    );
    std::vector<std::string> tmp2(0);
    for(const StringAndISBN &e:tmp) tmp2.emplace_back(e.ISBN);
    return tmp2;
}
bool BookOperator::insertBook(const std::string &_ISBN,const std::string &_name,const std::string &_auth,
                              const std::string &_key,int cnt,int cost,int tot_cost) noexcept
{
    auto [flg,u]=findBookByISBN(_ISBN);
    if(flg==1) return 0;

    books.insert(Book(_ISBN,_name,_auth,_key,cnt,cost,tot_cost));
    name_to_ISBN.insert(StringAndISBN(_name,_ISBN));
    auth_to_ISBN.insert(StringAndISBN(_auth,_ISBN));

    const std::vector<std::string> &tmp=Utils::splitStringByVert(_key);
    for(const std::string &e:tmp) key_to_ISBN.insert(StringAndISBN(e,_ISBN));

    return 1;
}
bool BookOperator::removeBook(const std::string &ISBN) noexcept
{
    auto [flg,u]=findBookByISBN(ISBN);
    if(flg==0) return 0;

    books.remove(u);
    name_to_ISBN.remove(StringAndISBN(u.name,ISBN));
    auth_to_ISBN.remove(StringAndISBN(u.auth,ISBN));

    const std::vector<std::string> &tmp=Utils::splitStringByVert(u.key);
    for(const std::string &e:tmp) key_to_ISBN.insert(StringAndISBN(e,ISBN));

    return 1;
}
bool BookOperator::updateBookData(const std::string &ISBN,const std::string &new_ISBN,const std::string &name,
                                  const std::string &auth,const std::string &key,int cost) noexcept
{
    auto [flg,u]=findBookByISBN(ISBN);
    if(flg==0) return 0;

    removeBook(ISBN);
    insertBook(new_ISBN,name,auth,key,u.cnt,cost,u.tot_cost);
    return 1;
}
bool BookOperator::updateBookQuantity(const std::string &ISBN,int delta,int delta_tot_cost) noexcept
{
    auto [flg,u]=findBookByISBN(ISBN);
    if(flg==0) return 0;

    removeBook(ISBN);
    insertBook(u.ISBN,u.name,u.auth,u.key,u.cnt+delta,u.cost,u.tot_cost+delta_tot_cost);
    return 1;
}

// *********************************************************************************