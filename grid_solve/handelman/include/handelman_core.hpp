#ifndef HANDELMAN_CORE
#define HANDELMAN_CORE
#include "expr.hpp"
#include "solver.hpp"
#include "vars.hpp"



class handelman_core{
    std::vector<multinomial*> lhs;
    std::vector<int> lhs_degree;
    std::vector<multinomial*> rhs_arr;
    solver *s;//will also be used to return solutions::
    int max_deg;
    
    public:
    void plain_solve(int type, int silent);//does not involve handelman::
    handelman_core(vars& av){
       s=new solver(&av);//remember to delete the solver::
       max_deg=2;
    }
    void make_deg_arr(){
        lhs_degree.clear();
        for(auto& a: lhs){
            lhs_degree.push_back(a->get_degree(false));
        }
    }
    void set_max_degree(int adeg=0);
    void add_lhs(multinomial& e);
    void add_lhs(handelman::expr& e);
    void add_rhs(multinomial& e);
    void add_rhs(handelman::expr& e);
    void add_st(multinomial& e,bool is_ineq);
    void add_st(handelman::expr& e,bool is_ineq);
    multinomial& gen_pol(int curr_deg,int first_index,std::vector<int> &id_arr);
    void solve(int type,int silent=1);//heavy operation::
    solver* solver_(){
        return s;
    }
    
    ~handelman_core();
};



#endif;