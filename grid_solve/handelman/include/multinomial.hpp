#ifndef MULTINOMIAL_H
#define MULTINOMIAL_H

#include "symbol.hpp"
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
class vec_str{
    public:
    int sz;
    std::vector<std::string> arr;
    vec_str(){
     sz=0;
    }
    vec_str& operator=(std::vector<std::string>& s){
       for(auto& e: s){
         if(e=="$"){
            break;
         }
         arr.push_back(e);
         sz+=1;
       }
       arr.push_back("$");
       return *this;
    }
    vec_str& operator=(vec_str& s){
       for(int i=0;i<s.sz;i++){
         arr.push_back(s[i]);
         sz+=1;
       }
       arr.push_back("$");
       return *this;
    }
    void push_back(std::string s){
        arr.push_back(s);
        sz+=1;
    }
    std::string operator[](int idx){
        return arr[idx];
    }
    

};
extern int created_polys;
extern int deleted_polys;
class multinomial{
    
    public:
    symbol* prim_key;
    std::map<uint16_t,void*> mp;//
    multinomial(){
        created_polys+=1;
        prim_key=NULL;
        mp[0]=(void*)new double(0);
        save=false;
       //empty intialisation::()
    }
    multinomial(symbol* aprim_key,bool asave){
        created_polys+=1;
        prim_key=aprim_key;
        mp[1]=new multinomial(1,asave);
        mp[0]=new multinomial(0.0,asave);
        save=asave;
    }
    multinomial(double c,bool asave){
        created_polys+=1;
        prim_key=NULL;
        mp[0]=new double(c);
        save=asave;
    }
    multinomial(double c){
        created_polys+=1;
        prim_key=NULL;
        mp[0]=new double(c);
        save=false;
    }
    multinomial& operator+(multinomial& m);
    multinomial& operator=(multinomial& m);
    multinomial& operator+(const double& m);
    multinomial& operator*(multinomial& m);
    multinomial& operator*(const double& c);
    
    multinomial& operator^(const uint16_t& pow);
    vec_str print();
    // multinomial& substitute(std::map<int,double> *sub_map=NULL){
    //     multinomial* ans= new multinomial(0);
        
    //     if(prim_key==NULL){
            
    //             ans=&(*ans + *((double*)mp[0]) );
            

    //     }
    //     else{
    //         for(auto &a:mp){
                    
                
    //             multinomial* temp=&((multinomial*)(a.second)->substitute(sub_map));
    //             if(a.first==0){
    //                 ans=&(*ans+*temp);
    //             }
    //             else{
    //                 //probably not a good practice
    //                 if(sub_map==NULL || (*sub_map).find(prim_key->get_id())==(*sub_map).end()){
    //                     z3::expr pw_e=z3::pw(e[m.prim_key->get_id()],a.first);
    //                     ans=ans+*temp;
    //                 }
    //                 else{
    //                     std::string val=std::to_string(pow((*sub_map)[m.prim_key->get_id()],a.first));
    //                     const char* myCString = val.c_str();

    //                     z3::expr mul_fac=(*c).real_val(myCString);
    //                     ans=ans+temp*(mul_fac);
    //                 }
    //             }
    //         }
    //     }
    //     return ans;
        
    // }

    uint32_t get_degree(bool count_template=true);
    void custom_delete(bool nested);//nested delete::

    bool is_zero();
    ~multinomial();
    bool save;//should we save this
    void set_save(bool asave);

    multinomial* simplify();

    std::vector<multinomial*> reduce();//reduce gives a set of priliminary multinomials that need to be zero::
    bool is_proper(uint32_t id=0);
    std::vector<int> ret_id_arr();//returning id array::

    bool cmp_poly(std::vector<int> &v);
    
    std::string smt_print(std::string var_dep,bool fun_names,int idx);



    double substitute(std::vector<double> &vals);//substitutes and returns a double::
};

extern void print_vector(vec_str arr);


#endif