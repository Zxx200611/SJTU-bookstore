#include<string>
#include<vector>


namespace Utils
{
const double eps=1e-7;

inline std::vector<std::string> splitStringByVert(const std::string &s)
{
    std::vector<std::string> res(0);
    if(s.size()==0) return res;

    std::string tmp="";
    for(int i=0;i<s.size();i++)
    {
        if(s[i]!='|') tmp+=s[i];
        else res.push_back(tmp);
    }

    return res;
}

inline std::vector<std::string> splitStringBySpace(const std::string &s)
{
    std::vector<std::string> res(0);
    if(s.size()==0) return res;

    std::string tmp="";
    for(int i=0;i<s.size();i++)
    {
        if(s[i]!=' ') tmp+=s[i];
        else res.push_back(tmp);
    }

    return res;
}

inline int comp(double a,double b)      // -1 if a < b
{
    if(std::abs(b-a)<eps) return 0;
    if(a<b) return -1;
    return 1;
}


inline void intersect(std::vector<std::string> &vec1,std::vector<std::string> vec2,bool fir=0)
{
    sort(vec1.begin(),vec1.end());
    sort(vec2.begin(),vec2.end());
    if(fir==1)
    {
        vec1.swap(vec2);
        return;
    }

    std::vector<std::string> vec3(0);
    for(int i=0,j=0;i<vec1.size();i++)
    {
        while(j<vec2.size()&&vec2[j]<vec1[i]) j++;
        if(j==vec2.size()) break;
        if(vec2[j]==vec1[i]) vec3.push_back(vec2[j]);
    }
    vec1.swap(vec3);
}

}