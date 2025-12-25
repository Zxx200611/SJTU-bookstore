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
    for(const LoginElement &e:login_stk) if(e.uid==uid) return 0;
    return 1;
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
        const std::vector<std::string> &ops=Utils::splitStringBySpace(tmp_str);

        try
        {
            if(ops.size()==0) throw -1;
            if(ops[0]=="quit"||ops[0]=="exit") break;

            // user part
            else if(ops[0]=="su")
            {
                if(ops.size()!=2&&ops.size()!=3) throw -1;
                auto [flg,u]=uo.findUserByUID(ops[1]);
                if(flg==0) throw -1;
                if(ops.size()==2&&(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per>=u.per)) throw -1;
                if(ops.size()==3&&ops[2]!=u.passwd) throw -1;

                login_stk.push_back(LoginElement(u.uid));

                oo.addOperationLog(login_stk.back().uid,"login");
            }
            else if(ops[0]=="logout")
            {
                if(ops.size()!=1) throw -1;
                if(login_stk.size()==0) throw -1;
                login_stk.pop_back();

                oo.addOperationLog(login_stk.back().uid,"logout");
            }
            else if(ops[0]=="register")
            {
                if(ops.size()!=4) throw -1;
                if(!uo.insertUser(ops[1],ops[2],ops[3],1)) throw -1;

                oo.addOperationLog("???","register user (uid = "+ops[1]+" )");
            }
            else if(ops[0]=="passwd")
            {
                if(ops.size()!=3||ops.size()!=4) throw -1;
                auto [flg,u]=uo.findUserByUID(ops[1]);
                if(flg==0) throw -1;
                if(login_stk.size()==0) throw -1;
                if(ops.size()==3&&uo.findUserByUID(login_stk.back().uid).second.per!=7) throw -1;

                if(ops.size()==3) if(!uo.changePassword(ops[1],"",ops[2],1)) throw -1;
                if(ops.size()==4) if(!uo.changePassword(ops[1],ops[2],ops[3])) throw -1;

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
                if(ops.size()!=2) throw -1;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per!=7) throw -1;
                if(logined(ops[1])) throw -1;
                if(!uo.removeUser(ops[1])) throw -1;
    
                oo.addOperationLog(login_stk.back().uid,"delete user (uid = "+ops[1]+" )");
            }

            // Book part
            else if(ops[0]=="buy")
            {
                if(ops.size()!=3) throw -1;
                if(login_stk.size()==0) throw -1;

                int c=stoi(ops[2]);
                auto [x,b]=bo.findBookByISBN(ops[1]);
                if(x==0||b.cnt<c||c<0) throw -1;

                std::cout<<std::setprecision(2)<<std::fixed<<c*b.cost<<"\n";
                bo.updateBookQuantity(ops[1],-c,-c*b.cost);

                oo.addOperationLog(login_stk.back().uid,"buy "+ops[2]+" copies of book (ISBN = "+ops[1]+" )");
                fo.addFinanceLog(c*b.cost,0);
            }
            else if(ops[0]=="select")
            {
                if(ops.size()!=2) throw -1;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<3) throw -1;
                if(!bo.findBookByISBN(ops[1]).first) throw -1;

                login_stk.back().slt_isbn=ops[1];
                oo.addOperationLog(login_stk.back().uid,"select book (ISBN = "+ops[1]+" )");
            }
            else if(ops[0]=="modify")
            {
                if(ops.size()<=1) throw -1;
                if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<3) throw -1;
                if(login_stk.back().slt_isbn==""||bo.findBookByISBN(login_stk.back().slt_isbn).first==0) throw -1;

                std::string new_isbn="",new_name="",new_auth="",new_key="";
                int new_price=-1;

                for(int i=1;i<ops.size();i++)
                {
                    if(ops[i].substr(0,6)=="-ISBN=")
                    {
                        if(new_isbn!="") throw -1;
                        new_isbn=ops[i].substr(6);
                    }
                    else if(ops[i].substr(0,6)=="-name=")
                    {
                        if(new_name!="") throw -1;
                        new_name=ops[i].substr(6);
                    }
                    else if(ops[i].substr(0,8)=="-author=")
                    {
                        if(new_auth!="") throw -1;
                        new_auth=ops[i].substr(8);
                    }
                    else if(ops[i].substr(0,9)=="-keyword=")
                    {
                        if(new_key!="") throw -1;
                        new_key=ops[i].substr(9);
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
                bo.updateBookData(pre_isbn,
                    new_isbn ==""?b.ISBN:new_isbn,
                    new_name ==""?b.name:new_name,
                    new_auth ==""?b.auth:new_auth,
                    new_key  ==""?b.key :new_key ,
                    new_price==-1?b.cost:new_price
                );

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
                if(ops.size()<=1) throw -1;
                if(ops[1]=="finance")   // show finance
                {
                    if(ops.size()!=3) throw -1;
                    if(login_stk.size()==0||uo.findUserByUID(login_stk.back().uid).second.per<7) throw -1;
                    if(stoi(ops[2])<0||stoi(ops[2])>fo.totFinanceCount()) throw -1;

                    fo.printLastKFinanceLog(stoi(ops[2]));
                    if(stoi(ops[2])==0) std::cout<<"\n";
                }
                else    // show book
                {
                    std::vector<std::string> isbns(0);

                    for(int i=1;i<ops.size();i++)
                    {
                        if(ops[i].substr(0,6)=="-ISBN=") Utils::intersect(isbns,std::vector<std::string>({ops[i].substr(6)}),i==1);
                        else if(ops[i].substr(0,6)=="-name=") Utils::intersect(isbns,bo.getISBNByName(ops[i].substr(6)),i==1);
                        else if(ops[i].substr(0,8)=="-author=") Utils::intersect(isbns,bo.getISBNByAuthor(ops[i].substr(8)),i==1);
                        else if(ops[i].substr(0,9)=="-keyword=")
                        {
                            const std::vector<std::string> keys=Utils::splitStringByVert(ops[i].substr(9));
                            for(int j=0;j<keys.size();j++)
                            {
                                Utils::intersect(isbns,bo.getISBNByKey(keys[j]),i==1&&j==0);
                            }
                        }
                        else throw -1;
                    }

                    bool flg=0;
                    for(const std::string &e:isbns)
                    {
                        auto [x,b]=bo.findBookByISBN(e);
                        if(x==0) continue;
                        flg=1;
                        std::cout<<b.ISBN<<"\t"<<b.name<<"\t"<<b.auth<<"\t"<<b.key<<"\t"<<b.cost<<"\t"<<b.cnt<<"\n";
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
    }
}