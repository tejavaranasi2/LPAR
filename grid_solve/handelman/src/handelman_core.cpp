#include "handelman_core.hpp"


void handelman_core::add_lhs(multinomial& e){
    bool is_save=e.save;
    e.set_save(true);
    multinomial *t=new multinomial((double)0,true);
    *t=e;
    e.set_save(is_save);
    t->set_save(true);
    lhs.push_back(t);
}
void handelman_core::add_lhs(handelman::expr& e){
    bool is_save=e.m->save;
    e.m->set_save(true);
    multinomial *t=new multinomial((double)0,true);
    *t=*e.m;
    e.m->set_save(is_save);
    t->set_save(true);
    lhs.push_back(t);
}
void handelman_core::add_st(multinomial& e,bool is_ineq=false){
     s->add(e,is_ineq);//add this to solver::
}
void handelman_core::add_st(handelman::expr& e,bool is_ineq=false){
    s->add(*e.m,is_ineq);
}
void handelman_core::add_rhs(multinomial& e){
    
    bool is_save=e.save;
    e.set_save(true);
    multinomial* rhs=new multinomial((double)0,true);

    *rhs=e;
    rhs->set_save(true);
    rhs_arr.push_back(rhs);
    
    
    e.set_save(is_save);
}
void handelman_core::add_rhs(handelman::expr& e){
    
    bool is_save=e.m->save;
    e.m->set_save(true);
    multinomial* rhs=new multinomial((double)0,true);
    *rhs=*e.m;
    rhs->set_save(true);
    rhs_arr.push_back(rhs);

    e.m->set_save(is_save);
}

multinomial& handelman_core::gen_pol(int curr_deg, int first_index,std::vector<int> &id_arr){
    multinomial *ans=new multinomial(0);
    bool found=false;
    for(int j=first_index;j<lhs.size();++j){
       if(curr_deg+lhs_degree[j]<=max_deg && lhs[j]->cmp_poly(id_arr)){
          found=true;
          if(j==2 && first_index==0){
            int x=1;
          }
          ans=&(*ans+(*lhs[j])*gen_pol(curr_deg+lhs_degree[j],j,id_arr));
          if(!ans->is_proper(0)){
             int x=1;
          }
       }
    }
    
        std::string ref=s->v->add_var(true,"additive_template");
        s->add((*(s->v))[ref],true);//add a ineq here::
        //s->add((*(s->v))[ref]*-1+100,true);
        ans=&(*ans+(*(s->v))[ref]);
    
    if(!ans->is_proper()){

    }
    return *ans;
}

void handelman_core::solve(int type,int silent){//heavy operation::
    if(type==2){
        //just use plain_solve::
        plain_solve(type,silent);
        return;
    }
    make_deg_arr();
    multinomial* to_solve;
    for(auto&rhs: rhs_arr){
        //repetitive computation::
        std::vector<int> rhs_id_arr=rhs->ret_id_arr();
        to_solve=&gen_pol(0,0,rhs_id_arr);
        to_solve=&(*to_solve+((*rhs) * -1));
        //print_vector(to_solve->print());
        for(auto& a:to_solve->reduce()){
            //print_vector(a->print());
            //std::cout<<"\n\n======================================\n\n";
            s->add(*a);
        }

        to_solve->custom_delete(true);
        delete to_solve;
    }
    if(type==0){
      s->solve(silent,"z3");
    } 
    else if(type==1){
      s->inc_solve(silent);
    }
    //do nothing if type==3::
}

void handelman_core::plain_solve(int type, int silent){
    std::vector<z3::expr> e=s->get_expr_vector();
    s->empty_mod_constraints();//clears any other constraints we have added::
    
    multinomial m(0);
    z3::expr lhs_term_impli=s->print_sym(m);
    for(int i=0;i<lhs.size();i++){
        if(i==0){
            lhs_term_impli=s->print_sym(*(lhs[i])) >= 0;
        }
        else{
            lhs_term_impli=(lhs_term_impli && s->print_sym(*(lhs[i])) >= 0);
        }
    }


    for(int i=0;i<rhs_arr.size();++i){
        multinomial* rhs= rhs_arr[i];

        z3::expr rhs_term_impli=(s->print_sym(*(rhs)) >= 0);
        rhs_term_impli=z3::implies(lhs_term_impli,rhs_term_impli);
        for(int i=0;i<e.size();i++){
            if(!(((*(s->v))[i]).prim_key->is_template())){
              rhs_term_impli=z3::forall(e[i],rhs_term_impli);
            }
        }
        
        //std::cout<<rhs_term_impli<<"\n";
        s->add(rhs_term_impli);        
    }

    if(type==0 || type==1){
       s->solve(silent,"z3");
    }
    //do not do anything here if type==3::
}

handelman_core::~handelman_core(){

}

void handelman_core::set_max_degree(int adeg){
    
        if(adeg==0){
            for(auto& rhs: rhs_arr){
                max_deg=std::max((int)rhs->get_degree(false),max_deg);//do not count templates here::
            }
        }
        else{
            max_deg=adeg;
        }
    
}