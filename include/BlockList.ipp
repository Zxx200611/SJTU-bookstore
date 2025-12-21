// Block part **********************************************************************

template<typename T>
Block<T>::Block(int _this_pos,FileOperator &fo) noexcept
{
    if(_this_pos!=-1&&_this_pos<fo.size()) fo.read(_this_pos,this);
    else
    {
        prev_pos=next_pos=-1,siz=0;
    }
    this_pos=_this_pos;
}
template<typename T>
T& Block<T>::at(int i) noexcept
{
    if(i==-1) assert(0);
    return data[i];
}
template<typename T>
void Block<T>::write(FileOperator &fo) noexcept
{
    if(this_pos==-1) return;
    fo.write(this_pos,this);
}
template<typename T>
int& Block<T>::prev() noexcept {return prev_pos;}
template<typename T>
int& Block<T>::next() noexcept {return next_pos;}
template<typename T>
int& Block<T>::pos() noexcept {return this_pos;}
template<typename T>
int& Block<T>::size() noexcept {return siz;}
template<typename T>
T Block<T>::maxElement() noexcept
{
    assert(size()!=0);
    return data[size()-1];
}
template<typename T>
void Block<T>::copyInsert(int l,int r,std::vector<T> &res) noexcept
{
    res.insert(res.end(),data+l,data+r);
}

// *********************************************************************************



// BlockList part ******************************************************************

template<typename T,typename Compare>
BlockList<T,Compare>::BlockList(const std::string &file_name) noexcept:fo(file_name),comp()
{
    if(fo.size()==0)
    {
        node_head_pos=-1;
        fo.write(0,&node_head_pos);
    }
    else fo.read(0,&node_head_pos);
}
template<typename T,typename Compare>
BlockList<T,Compare>::~BlockList() noexcept
{
    fo.write(0,&node_head_pos);
}
template<typename T,typename Compare>
std::pair<int,int> BlockList<T,Compare>::lowerBound(const T &t) noexcept
{
// #ifdef DEBUG
//     std::cout<<"LowerBounding "<<t.idx<<" "<<t.val<<std::endl;
// #endif
    int u_pos=node_head_pos;
    Block<T> u(u_pos,fo);
// #ifdef DEBUG
//         std::cout<<u.pos()<<std::endl;
//         std::cout<<u.maxElement().idx<<" "<<u.maxElement().val<<std::endl;
//         std::cout<<comp(u.maxElement(),t)<<std::endl;
// #endif
    for(;u_pos!=-1&&u.next()!=-1;u=Block<T>(u_pos=u.next(),fo))
    {
        if(!comp(u.maxElement(),t)) break;
    }
    if(u_pos==-1) return std::make_pair(-1,-1);

    int v_pos=0;
    for(;v_pos<u.size();v_pos++)
    {
// #ifdef DEBUG
//         std::cout<<u.at(v_pos).idx<<" "<<u.at(v_pos).val<<std::endl;
//         std::cout<<comp(u.at(v_pos),t)<<std::endl;
// #endif
        if(!comp(u.at(v_pos),t)) break;
    }
    return std::make_pair(u_pos,v_pos);
}
template<typename T,typename Compare>
bool BlockList<T,Compare>::trySplit(Block<T> &b) noexcept
{
#ifdef DEBUG
    std::cout<<"Try split at "<<b.pos()<<" ( size = "<<b.size()<<" )"<<std::endl;
    std::cout<<"Try split at "<<b.pos()<<" ( size = "<<b.size()<<" )"<<std::endl;
    std::cout<<"Next"<<std::endl;
#endif
    if(b.pos()==-1||b.size()<=Block<T>::block_max_size)
    {
#ifdef DEBUG
        std::cout<<"Returning "<<std::endl;
#endif
        return 0;
    }
#ifdef DEBUG
    std::cout<<"Splitting "<<b.pos()<<std::endl;
#endif

    int bn_siz=(b.size()+1)/2,nw_siz=b.size()-bn_siz;
    Block<T> nw(fo.size(),fo),nxt(b.next(),fo);
    nw.prev()=b.pos(),nw.next()=nxt.pos(),nw.size()=nw_siz;
    std::copy(b.data+bn_siz,b.data+b.size(),nw.data);
    b.size()=bn_siz,b.next()=nw.pos(),nxt.prev()=nw.pos();

    b.write(fo),nw.write(fo),nxt.write(fo);
    return 1;
}
template<typename T,typename Compare>
bool BlockList<T,Compare>::tryMerge(Block<T> &b) noexcept
{
    Block<T> nxt(b.next(),fo),nxtnxt(nxt.next(),fo);
    if(b.pos()==-1||nxt.pos()==-1||b.size()+nxt.size()>Block<T>::block_max_size) return 0;

    std::copy(nxt.data,nxt.data+nxt.size(),b.data+b.size());
    b.size()+=nxt.size(),b.next()=nxtnxt.pos(),nxtnxt.prev()=b.pos();

    b.write(fo),nxtnxt.write(fo);       // do not erase nxt's data
    return 1;
}

template<typename T,typename Compare>
void BlockList<T,Compare>::insert(const T &t) noexcept
{
    if(node_head_pos==-1)
    {
        Block<T> u(fo.size(),fo);
        node_head_pos=u.pos();
        u.size()=1,u.at(0)=t;
#ifdef DEBUG
        std::cout<<"assign vals done"<<std::endl;
#endif
        u.write(fo);
#ifdef DEBUG
        std::cout<<"written"<<std::endl;
#endif
        return;
    }

    auto [u_pos,v_pos]=lowerBound(t);
#ifdef DEBUG
    std::cout<<"insert at "<<u_pos<<" "<<v_pos<<std::endl;
#endif
    Block<T> u(u_pos,fo);
    std::copy_backward(u.data+v_pos,u.data+u.size(),u.data+u.size()+1);
    u.at(v_pos)=t,u.size()++;
    u.write(fo);
#ifdef DEBUG
    for(int v_pos=0;v_pos<u.size();v_pos++)
    {
        std::cout<<"( "<<u.at(v_pos).idx<<" , "<<u.at(v_pos).val<<" ) ";
    }
    std::cout<<std::endl;
#endif
    trySplit(u);
}
template<typename T,typename Compare>
bool BlockList<T,Compare>::remove(const T &t) noexcept
{
    if(node_head_pos==-1) return 0;
    auto [u_pos,v_pos]=lowerBound(t);
#ifdef DEBUG
    std::cout<<"remove at "<<u_pos<<" "<<v_pos<<std::endl;
#endif
    Block<T> u(u_pos,fo);
    if(v_pos==u.size()||comp(u.at(v_pos),t)||comp(t,u.at(v_pos))) return 0;

    std::copy(u.data+v_pos+1,u.data+u.size(),u.data+v_pos);
    u.size()--;
    u.write(fo);

    if(u.next()==-1&&u.prev()==-1&&u.size()==0)
    {
        node_head_pos=-1;
        return 1;
    }

    while(u.next()!=-1&&tryMerge(u)) u=Block<T>(u.next(),fo);
    Block<T> v(u.prev(),fo);
    while(u.prev()!=-1&&tryMerge(v)) u=v,v=Block<T>(v.prev(),fo);
    return 1;
}
template<typename T,typename Compare>
std::vector<T> BlockList<T,Compare>::find(const T &l,const T&r) noexcept
{
#ifdef DEBUG
    std::cout<<"Finding "<<l.idx<<std::endl;
#endif
    auto [lu_pos,lv_pos]=lowerBound(l);
    auto [ru_pos,rv_pos]=lowerBound(r);
#ifdef DEBUG
    std::cout<<"L( "<<lu_pos<<" , "<<lv_pos<<" ) , R( "<<ru_pos<<" , "<<rv_pos<<" )"<<std::endl;
#endif

    Block<T> u(lu_pos,fo);
    std::vector<T> res(0);
    if(lu_pos==ru_pos)
    {
        u.copyInsert(lv_pos,rv_pos,res);
        return res;
    }

    u.copyInsert(lv_pos,u.size(),res);
    for(int i=u.next();i!=ru_pos;i=u.next())
    {
        u=Block<T>(i,fo);
        u.copyInsert(0,u.size(),res);
    }
    if(ru_pos!=-1)
    {
        u=Block<T>(u.next(),fo),assert(u.pos()==ru_pos);
        u.copyInsert(0,rv_pos,res);
    }
    return res;
}

#ifdef DEBUG
template<typename T,typename Compare>
void BlockList<T,Compare>::print() noexcept
{
    for(Block<T> u(node_head_pos,fo);u.pos()!=-1;u=Block<T>(u.next(),fo))
    {
        std::cout<<"Block #"<<u.pos()<<" : { ";
        for(int v_pos=0;v_pos<u.size();v_pos++)
        {
            std::cout<<"( "<<u.at(v_pos).idx<<" , "<<u.at(v_pos).val<<" ) ";
        }
        std::cout<<"}"<<std::endl;
    }
}
#endif

// *********************************************************************************