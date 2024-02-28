#ifndef EXPR_H
#define EXPR_H

#include "multinomial.hpp"
namespace handelman{
class expr{

    bool in_mem;//tells if we have to delete this::
    public:
    multinomial* m;//created via new::
    expr(){
        m=NULL;
        in_mem=false;//we create only when we assign a poly to it::
    }
    multinomial& operator=(expr& e);
    multinomial& operator=(multinomial& e);
    multinomial& operator=(const double& e);

    multinomial& operator+(expr& e);
    multinomial& operator+(multinomial& e);
    multinomial& operator+(const double& e);

    multinomial& operator*(expr& e);
    multinomial& operator*(multinomial& e);
    multinomial& operator*(const double& e);
    
    multinomial& operator-(expr& e);
    multinomial& operator-(multinomial& e);
    multinomial& operator-(const double& e);

    multinomial& operator^(const uint16_t& pow);
    ~expr(){
        if(in_mem && !m->save){
            m->custom_delete(true);
            delete m;
            in_mem=false;
        }
    }
    void rem_mem_flag(){
        if(m!=NULL){
            if(!m->save){
                in_mem=false;
            }
        }
    }

    vec_str print(){
        return m->print();
    }

    void set_save(bool asave){
       m->set_save(asave);
    }

    double substitute(std::vector<double> &v){
        return m->substitute(v);
    }

    std::string smt_print(std::string var_dep,bool fun_names,int idx){
        return m->smt_print(var_dep,fun_names,idx);
    }
    
};
}

#endif;
