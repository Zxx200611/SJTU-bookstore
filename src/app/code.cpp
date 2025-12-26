#include<Book.hpp>
#include<User.hpp>
#include<Log.hpp>

BookOperator bo;
UserOperator uo;
FinanceLogOperator fo;
OperationLogOperator oo;

struct LoginElement
{
    std::string uid;
    mutable std::string slt_isbn;

    LoginElement(const std::string &_uid):uid(_uid),slt_isbn(""){}
};
std::vector<LoginElement> login_stk;

bool logined(const std::string &uid)
{
    for(const LoginElement &e:login_stk) if(e.uid==uid) return 1;
    return 0;
}

int main()
{
    if(!uo.findUserByUID("root").first)
    {
        uo.insertUser("root","sjtu","root",7);
    }

    while(1)
    {
        static std::string tmp_str;
        std::getline(std::cin,tmp_str);
        if(std::cin.eof()) break;
        const std::vector<std::string> &ops=Utils::splitStringBySpace(tmp_str);

        try
        {
            if(ops.size()==0) continue;
            if(ops[0]=="quit"||ops[0]=="exit") break;

            // user part
            else if(ops[0]=="su")
            {
                if(ops.size()!=2&&ops.size()!=3) throw -1;
                auto [flg,u]=uo.findUserByUID(ops[1]);
                if(flg==0) throw -1;
                if(ops.size()==2&&(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<=u.per)) throw -1;
                if(ops.size()==3&&ops[2]!=u.passwd) throw -1;

                login_stk.push_back(LoginElement(u.uid));

                oo.addOperationLog(login_stk.back().uid,"login");
            }
            else if(ops[0]=="logout")
            {
                if(ops.size()!=1) throw -1;
                if(login_stk.size()==0) throw -1;
                oo.addOperationLog(login_stk.back().uid,"logout");
                login_stk.pop_back();
            }
            else if(ops[0]=="register")
            {
                if(ops.size()!=4) throw -1;
                if(!uo.insertUser(ops[1],ops[2],ops[3],1)) throw -1;

                oo.addOperationLog("???","register user (uid = "+ops[1]+" )");
            }
            else if(ops[0]=="passwd")
            {
            //     std::cerr<<ops.size()<<" : ";
            //     for(const std::string &s:ops) std::cerr<<s<<" ";
            //     std::cerr<<std::endl;

                // std::cerr<<"A"<<std::endl;
                if(ops.size()!=3&&ops.size()!=4) throw -1;
                // std::cerr<<"B"<<std::endl;
                auto [flg,u]=uo.findUserByUID(ops[1]);
                // std::cerr<<"C"<<std::endl;
                if(flg==0) throw -1;
                if(login_stk.size()==0) throw -1;
                // std::cerr<<"D"<<std::endl;
                if(ops.size()==3&&uo.findUserByUID(login_stk.back().uid).second.per!=7) throw -1;
                // std::cerr<<"E"<<std::endl;

                if(ops.size()==3) if(!uo.changePassword(ops[1],"",ops[2],1)) throw -1;
                if(ops.size()==4) if(!uo.changePassword(ops[1],ops[2],ops[3])) throw -1;
                // std::cerr<<"F"<<std::endl;

                oo.addOperationLog(login_stk.back().uid,"update passwd for user (uid = "+ops[1]+" )");
            }
            else if(ops[0]=="useradd")
            {
                if(ops.size()!=5) throw -1;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<=stoi(ops[3])) throw -1;
                if(!uo.insertUser(ops[1],ops[2],ops[4],stoi(ops[3]))) throw -1;

                oo.addOperationLog(login_stk.back().uid,"add user (uid = "+ops[1]+" )");
            }
            else if(ops[0]=="delete")
            {
                // std::cerr<<"A"<<std::endl;
                if(ops.size()!=2) throw -1;
                // std::cerr<<"B"<<std::endl;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per!=7) throw -1;
                // std::cerr<<"C"<<std::endl;
                if(logined(ops[1])) throw -1;
                // std::cerr<<"D"<<std::endl;
                if(!uo.removeUser(ops[1])) throw -1;
                // std::cerr<<"E"<<std::endl;

                oo.addOperationLog(login_stk.back().uid,"delete user (uid = "+ops[1]+" )");
            }

            // Book part
            else if(ops[0]=="buy")
            {
                if(ops.size()!=3) throw -1;
                if(login_stk.size()==0) throw -1;

                int c=stoi(ops[2]);
                auto [x,b]=bo.findBookByISBN(ops[1]);
                if(x==0||b.cnt<c||c<=0) throw -1;

                std::cout<<std::setprecision(2)<<std::fixed<<c*b.cost<<"\n";
                bo.updateBookQuantity(ops[1],-c,-c*b.cost);

                oo.addOperationLog(login_stk.back().uid,"buy "+ops[2]+" copies of book (ISBN = "+ops[1]+" )");
                fo.addFinanceLog(c*b.cost,0);
            }
            else if(ops[0]=="select")
            {
                if(ops.size()!=2) throw -1;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<3) throw -1;
                if(!bo.findBookByISBN(ops[1]).first) bo.insertBook(ops[1],"","","",0,0,0);

                login_stk.back().slt_isbn=ops[1];
                oo.addOperationLog(login_stk.back().uid,"select book (ISBN = "+ops[1]+" )");
            }
            else if(ops[0]=="modify")
            {
                if(ops.size()<=1) throw -1;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<3) throw -1;
                if(login_stk.back().slt_isbn==""||bo.findBookByISBN(login_stk.back().slt_isbn).first==0) throw -1;

                std::string new_isbn="",new_name="",new_auth="",new_key="";
                double new_price=-1;

                for(int i=1;i<ops.size();i++)
                {
                    if(ops[i].substr(0,6)=="-ISBN=")
                    {
                        if(new_isbn!=""||bo.findBookByISBN(ops[i].substr(6)).first) throw -1;
                        new_isbn=ops[i].substr(6);
                    }
                    else if(ops[i].substr(0,7)=="-name=\"")
                    {
                        if(new_name!="") throw -1;
                        new_name=ops[i].substr(7,ops[i].size()-7-1);
                    }
                    else if(ops[i].substr(0,9)=="-author=\"")
                    {
                        if(new_auth!="") throw -1;
                        new_auth=ops[i].substr(9,ops[i].size()-9-1);
                    }
                    else if(ops[i].substr(0,10)=="-keyword=\"")
                    {
                        if(new_key!="") throw -1;
                        new_key=ops[i].substr(10,ops[i].size()-10-1);
                    } 
                    else if(ops[i].substr(0,7)=="-price=")
                    {
                        if(new_price!=-1) throw -1;
                        new_price=stod(ops[i].substr(7));
                    }
                    else throw -1;
                }
                std::string pre_isbn=login_stk.back().slt_isbn;
                if(new_isbn!="")
                {
                    if(new_isbn==login_stk.back().slt_isbn) throw -1;
                    for(LoginElement &e:login_stk) if(e.slt_isbn==pre_isbn) e.slt_isbn=new_isbn;
                }

                Book b=bo.findBookByISBN(pre_isbn).second;
                if(!bo.updateBookData(pre_isbn,
                    new_isbn ==""?b.ISBN:new_isbn,
                    new_name ==""?b.name:new_name,
                    new_auth ==""?b.auth:new_auth,
                    new_key  ==""?b.key :new_key ,
                    Utils::comp(new_price,-1)==0?b.cost:new_price
                )) throw -1;

                oo.addOperationLog(login_stk.back().uid,"update book (prev ISBN = "+pre_isbn+" ) 's data");
            }
            else if(ops[0]=="import")
            {
                if(ops.size()!=3) throw -1;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<3) throw -1;
                if(login_stk.back().slt_isbn==""||bo.findBookByISBN(login_stk.back().slt_isbn).first==0) throw -1;

                int c=stoi(ops[1]);
                double tot_cst=stod(ops[2]);
                bo.updateBookQuantity(login_stk.back().slt_isbn,c,tot_cst);
                
                oo.addOperationLog(login_stk.back().uid,"import "+ops[1]+" copies book (ISBN = "+login_stk.back().slt_isbn+" ) at total price "+ops[2]);
                fo.addFinanceLog(0,tot_cst);
            }

            // Log part
            else if(ops[0]=="show")
            {
                if(ops[1]=="finance")   // show finance
                {
                    if(ops.size()<=1) throw -1;
                    if(ops.size()>3) throw -1;
                    if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<7) throw -1;

                    if(ops.size()==2)
                    {
                        fo.printLastKFinanceLog(fo.totFinanceCount());
                        if(fo.totFinanceCount()==0) std::cout<<"\n";
                    }
                    else if(ops.size()==3)
                    {
                        if(stoi(ops[2])<0||stoi(ops[2])>fo.totFinanceCount()) throw -1;

                        fo.printLastKFinanceLog(stoi(ops[2]));
                        if(stoi(ops[2])==0) std::cout<<"\n";
                    }
                }
                else    // show book
                {
                    if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<1) throw -1;

                    std::vector<std::string> isbns(0);

                    for(int i=1;i<ops.size();i++)
                    {
                        if(ops[i].substr(0,6)=="-ISBN=") Utils::intersect(isbns,std::vector<std::string>({ops[i].substr(6)}),i==1);
                        else if(ops[i].substr(0,7)=="-name=\"") Utils::intersect(isbns,bo.getISBNByName(ops[i].substr(7,ops[i].size()-7-1)),i==1);
                        else if(ops[i].substr(0,9)=="-author=\"") Utils::intersect(isbns,bo.getISBNByAuthor(ops[i].substr(9,ops[i].size()-9-1)),i==1);
                        else if(ops[i].substr(0,10)=="-keyword=\"")
                        {
                            const std::vector<std::string> keys=Utils::splitStringByVert(ops[i].substr(10,ops[i].size()-10-1));
                            if(keys.size()!=1) throw -1;
                            Utils::intersect(isbns,bo.getISBNByKey(keys[0]),i==1);
                        }
                        else throw -1;
                    }

                    bool flg=0;
                    if(ops.size()==1)
                    {
                        const std::vector<Book> &alb=bo.allBooks();
                        for(const Book &b:alb)
                        {
                            flg=1;
                            std::cout<<b.ISBN<<"\t"<<b.name<<"\t"<<b.auth<<"\t"<<b.key<<"\t";
                            std::cout<<std::setprecision(2)<<std::fixed<<b.cost<<"\t";
                            std::cout<<b.cnt<<"\n";
                        }
                    }
                    else
                    {
                        for(const std::string &e:isbns)
                        {
                            auto [x,b]=bo.findBookByISBN(e);
                            if(x==0) continue;
                            flg=1;
                            std::cout<<b.ISBN<<"\t"<<b.name<<"\t"<<b.auth<<"\t"<<b.key<<"\t";
                            std::cout<<std::setprecision(2)<<std::fixed<<b.cost<<"\t";
                            std::cout<<b.cnt<<"\n";
                        }
                    }
                    if(flg==0) std::cout<<"\n";

                    oo.addOperationLog(login_stk.back().uid,"show book");
                }
            }
            else if(ops[0]=="report")
            {
                if(ops.size()!=2) throw -1;
                if(ops[1]=="finance") fo.printFinanceTable();
                else if(ops[1]=="employee") oo.reportAllOperationLog();
                else throw -1;
            }
            else if(ops[0]=="log")
            {
                fo.printFinanceTable();
                oo.reportAllOperationLog();
            }
            else throw -1;
        }
        catch(int e)
        {
            if(e==-1)
            {
                std::cout<<"Invalid\n";
            }
        }
        catch(std::invalid_argument x)
        {
            std::cout<<"Invalid\n";
        }
    }
}