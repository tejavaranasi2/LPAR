#ifndef VARS_H
#define VARS_H

#include "multinomial.hpp"


class vars{
    std::vector<multinomial*> var_arr;
    int N_vars;
    
    public:
    std::map<std::string,int> mp;
    vars(int n){
        for(int i=0;i<n;i++){
            std::string aname="var_"+std::to_string(var_arr.size());
            mp[aname]=var_arr.size();
            multinomial* temp=new multinomial(new symbol(false),true);
            var_arr.push_back(temp);
        }
    }
    ~vars(){
        //destructor::
        for(auto& a:var_arr){
            delete a->prim_key;
            a->custom_delete(true);
            delete a;
        }
        //std::cout<<created_polys<<" : "<<deleted_polys<<"\n";
        
    }

    std::string add_var(bool ais_template,std::string aname);

    std::string add_var(int n,bool ais_template,std::string aname);

    multinomial& operator[](int idx){
        return *var_arr[idx];//
    }

    multinomial& operator[](std::string name){
        return *var_arr[mp[name]];
    }
    multinomial& operator()(std::string name, int idx){
        std::string to_s=name+"_"+std::to_string(idx);
        if(mp.find(to_s)==mp.end()){
            throw "No such name ["+to_s+"]\n";
        }
        return *var_arr[mp[to_s]];
    }
    int sz(){
        return var_arr.size();
    }

    bool is_templ(int idx){
        return var_arr[idx]->prim_key->is_template();
    }

};

#endif;