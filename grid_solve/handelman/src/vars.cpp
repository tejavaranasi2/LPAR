#include "vars.hpp"



std::string vars::add_var(bool ais_template,std::string aname=""){
    if(aname=="" || mp.find(aname)!=mp.end()){
         aname=aname+"var_"+std::to_string(var_arr.size());
    }
    
    mp[aname]=var_arr.size();
    var_arr.push_back(new multinomial(new symbol(ais_template),true));

    return aname;
}

std::string vars::add_var(int n,bool ais_template,std::string aname=""){
        if(aname=="" || mp.find(aname)!=mp.end()){
            aname=aname+"grp"+std::to_string(mp.size());
        }
        std::string t="";
        for(int i=0;i<n;i++){
            t=aname+(std::string)"_"+std::to_string(i);
            mp[t]=var_arr.size();
            multinomial* temp=new multinomial(new symbol(ais_template),true);
            var_arr.push_back(temp);
        }

        return aname;
}