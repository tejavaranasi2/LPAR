//includes necessary files for solver::
#ifndef SOLVER_H
#define SOLVER_H
#include "vars.hpp"
#include "expr.hpp"
#include "z3++.h"
#include <set>
#include <yices.h>
#include <utility>
#include <thread>
#include <chrono>
#include <future>
//#include <mlpack/core.hpp>
#include <cmath>
#include <boost/thread.hpp>
#include <gurobi_c++.h>
//#include <mlpack/methods/optimization/sgd/sgd.hpp>

std::vector<int> get_vars(z3::expr e,z3::context &c);



class solver{
    std::vector<multinomial*> eq;
    
    std::vector<z3::expr> e;
    std::vector<term_t> e_yices;
    std::vector<multinomial*> ineq;
    std::vector<z3::expr> mod_constraints;

    z3::context *c;
    z3::config* cfg;

    boost::mutex data_mutex;  // Mutex to protect shared data
    public:
    vars *v;

    ~solver();
    solver(vars* av){
       v=av;
       cfg=new z3::config;
       (*cfg).set("auto_config", true);
       c=new z3::context(*cfg);
       
       int n=v->sz();
       for(int i=0;i<n;i++){
                    const char* myCString = ("var_"+std::to_string(i)).c_str();
                    z3::expr t=(*c).real_const(myCString);
                    e.push_back(t);
       }



       //create an empty gurobi env::
        
    }
    std::vector<z3::expr> get_expr_vector(){
        return e;
    }

    void empty_mod_constraints(int n=-1){
        //empties any forall constraints we have::
        if(n==-1){
         mod_constraints.clear(); //reduces the size to zero::
        }
        else{
            mod_constraints.erase(mod_constraints.begin()+n,mod_constraints.end());
        }
    }
    void empty_constraints(){
        //empties any forall constraints we have::
        for(auto& elem:eq){
            delete elem;
        }
        for(auto& elem:ineq){
            delete elem;
        }
        eq.clear();
        ineq.clear();
    }
    void solve(int silent,std::string solver_type="z3"){
        //create expr variables::
        
        data_mutex.lock();
        int n=v->sz();
        
        for(int i=e.size();i<n;i++){
            const char* myCString = ("var_"+std::to_string(i)).c_str();
            z3::expr t=(*c).real_const(myCString);
            e.push_back(t);
        }
        data_mutex.unlock();

        if(solver_type=="z3"){

        
        
            z3::solver s(*c);

            
            //add equations to solver::
            for(auto &a: eq){
                print_vector(a->print());
                z3::expr e_=print_sym(*a);
                s.add(e_==0);
            }
            if(silent){
            std::cout<<"INEQ===========\n============\n";
            }
            for(auto &a: ineq){
                //print_vector(a->print());
                z3::expr e_=print_sym(*a);
                //std::cout<<e_;
                s.add(e_>=0);
            }

            for(auto &a: mod_constraints){
                s.add(a);
            }
            auto result = s.check();
            if (result == z3::sat) {
                if(silent){
                std::cout << "Satisfiable\n";
                }
                // Access the value of the real variable in the model
                auto model = s.get_model();
                for(auto& a: v->mp){
                    if(silent){
                    std::cout<<a.first<<"="<<model.eval(e[a.second])<<"\n";
                    }
                }
            } else if (result == z3::unsat) {
                if(silent){
                std::cout << "Not satisfiable\n";
                }
            } else {
                if(silent){
                std::cout << "Solver result: " << result << "\n";
                }
            }
        }
        else if(solver_type=="yices"){
        
            
            

            yices_init();
            ctx_config_t* config= yices_new_config();
            yices_set_config(config, "solver-type", "mcsat");
            yices_default_config_for_logic(config,"QF_NIA");

            context_t *ctx=yices_new_context(config);


            

            
            for(int i=0;i<n;i++){
                const char* myCString = ("var_"+std::to_string(i)).c_str();
                term_t t = yices_new_uninterpreted_term(yices_real_type());
                yices_set_term_name(t,myCString);
                e_yices.push_back(t);
            }
            //add equations to solver::
            for(auto &a: eq){
                print_vector(a->print());
                term_t e_=print_sym_yices(*a);
                // std::cout<<"yices Term:\n------------------------------";
                // yices_pp_term(stdout, e_, 80, 8, 0);
                // std::cout<<"===============\n";
                yices_assert_formula(ctx,yices_arith_eq_atom(e_,yices_zero()));
            }
            if(silent){
            std::cout<<"INEQ===========\n============\n";
            }
            for(auto &a: ineq){
                print_vector(a->print());
                term_t e_=print_sym_yices(*a);
                //std::cout<<e_;
                yices_assert_formula(ctx,yices_arith_geq_atom(e_,yices_zero()));
            }
            auto result = yices_check_context(ctx, NULL);
            if (result == STATUS_SAT) {
                if(silent){
                std::cout << "Satisfiable\n";
                }
                // Access the value of the real variable in the model
                auto model = yices_get_model(ctx,1);
                for(auto& a: v->mp){
                    if(silent){
                        double value;
                        if (yices_get_double_value(model, e_yices[a.second], &value) == 0) {
                           std::cout << a.first << "=" << value << "\n";
                        }
                    }
                }
            } else if (result == STATUS_UNSAT) {
                if(silent){
                std::cout << "Not satisfiable\n";
                }
            } else {
                if(silent){
                std::cout << "Solver result: " << result << "\n";
                }
            }

            yices_free_context(ctx);
            yices_exit();
        }
        else if(solver_type=="pgd"){
            handelman::expr lgr;
            lgr=0;

            for(auto &a: eq){
                std::string ref=v->add_var(true,"additive_template_lgr");
                lgr=lgr+(*a)*((*v)[ref]);
            }
            if(silent){
                std::cout<<"INEQ===========\n============\n";
            }
            std::vector<int> proj_id_arr;//projection variables::
            for(auto &a: ineq){
                std::string ref=v->add_var(true,"additive_template_lgr");
                lgr=lgr+(*a)*((*v)[ref]);
                proj_id_arr.push_back((*v)[ref].prim_key->get_id());
            }

            // mlpack::optimization::PGD pgd;
            // pgd.MaxIterations() = 1000;  // Set the maximum number of iterations.
            // pgd.StepSize() = 0.01;      // Set the step size.
            // pgd.Projection() = [](arma::mat& coords) {
            //     for (size_t i = 0; i < proj_id_arr.size(); ++i)
            //     {
            //         coords[i] = std::max(0.0, coords[proj_id_arr[i]]);
            //     }
            // };

            // // Initial point.
            // arma::mat initialPoint = arma::randu<arma::mat>( v->sz(),1);  // Random initial point.

            // // Optimize using your custom function 'g'.
            // arma::mat coordinates;
            // double result = pgd.Optimize([](const arma::mat& coords) {
            //     // Lambda function as the objective.
            //     std::vector<double> coordVector(coords.begin(), coords.end());
            //     double objectiveResult = lgr.substitute(coordVector);
            //     return objectiveResult;
            // }, initialPoint, coordinates);

            // std::cout << "Optimal coordinates: " << coordinates << std::endl;
        }
        
        


    }

    void inc_solve(int silent){
        std::vector<z3::expr> cons_fixed;
        std::vector<z3::expr> cons_variable;
        std::vector<bool> is_free;
        int N_free=0,N_tot=0;//N_tot is the total number of new constraints introduced::
        
        int n=v->sz();
        
        for(int i=e.size();i<n;i++){
            const char* myCString = ("var_"+std::to_string(i)).c_str();
            z3::expr t=(*c).real_const(myCString);
            e.push_back(t);
        }

        for(int i=0;i<n;i++){
            //const char* myCString = ("var_"+std::to_string(i)).c_str();
            z3::expr t=e[i];
            //e.push_back(t);
            if(v->is_templ(i)){
                N_tot+=1;
                cons_variable.push_back(t==(*c).real_val("0"));
                is_free.push_back(false);
            }
        }
     
        //add equations to solver::
        for(auto &a: eq){
            //print_vector(a->print());
            z3::expr e_=print_sym(*a);
            cons_fixed.push_back(e_==(*c).real_val("0"));
        }
        
        for(auto &a: ineq){
            //print_vector(a->print());
            z3::expr e_=print_sym(*a);
            cons_fixed.push_back(e_>=(*c).real_val("0"));
            //std::cout<<cons_fixed[cons_fixed.size()-1]<<"\n";
        }

        while(N_free<N_tot){
            
            z3::solver s(*c);
            
            z3::params p(*c);
            p.set("unsat_core", true);
            s.set(p);

            for(int i=0;i<cons_fixed.size();i++){
                //std::cout<<cons_fixed[i]<<"\n=====================\n";
                s.add(cons_fixed[i],("e"+std::to_string(i)).c_str());
            }

            for(int i=0;i<is_free.size();i++){
                if(!is_free[i]){
                    s.add(cons_variable[i],("p"+std::to_string(i)).c_str());
                }
            }

            auto result = s.check();
            if (result == z3::sat) {
                if(silent){
                  std::cout << "Satisfiable\n";
                }
                // Access the value of the real variable in the model
                auto model = s.get_model();
                for(auto& a: v->mp){
                    if(silent){
                      std::cout<<a.first<<"="<<model.eval(e[a.second])<<"\n";
                    }
                }
                break;
            } else if (result == z3::unsat) {
                z3::expr_vector core = s.unsat_core();
                
                 
                for (unsigned i = 0; i < core.size(); ++i) {
                    std::string name=core[i].decl().name().str();
                    if(name[0]=='p'){
                        name=name.substr(1);
                        if(silent){
                          std::cout<<"unsat_core "<<stoi(name)<<"\n";
                        }
                        if(!is_free[stoi(name)]){
                                is_free[stoi(name)]=true;
                                N_free+=1;
                        }
                    }
                    else{
                        name=name.substr(1);
                        for(auto& var: get_vars(cons_fixed[stoi(name)],*c)){
                            if(silent){
                              std::cout<<"unsat_core "<<var-n+N_tot<<"\n";
                            }
                            if(!is_free[var-n+N_tot]){
                                is_free[var-n+N_tot]=true;
                                N_free+=1;
                                continue;
                            }
                            
                        
                        }
                    }
                }

            } else {
                if(silent){
                  std::cout << "Solver result: " << result << "\n";
                }
            }
        }

    }

    void add(multinomial& m,bool is_ineq=false){
        //add
        bool is_save=m.save;
        m.set_save(true);
        multinomial *t=new multinomial((double)0,true);
        *t=m;
        m.set_save(is_save);
        if(is_ineq){
            ineq.push_back(t);
        }
        else{
            eq.push_back(t);
        }
    }
    void add(z3::expr e){
        mod_constraints.push_back(e);
    }

    void solve_plain(std::vector<z3::expr> v){
        //------> works in a very different manner::
        //variables are already provided here::
        //solve only mod_constraints::
        



    }
    void add(handelman::expr& e,bool is_ineq=false){
        bool is_save=e.m->save;
        e.m->set_save(true);
        multinomial *t=new multinomial((double)0,true);
        *t=*e.m;
        e.m->set_save(is_save);
        if(is_ineq){
            ineq.push_back(t);
        }
        else{
            eq.push_back(t);
        }
    }

    z3::expr print_sym(multinomial& m,std::map<int,double> *sub_map=NULL){
        z3::expr ans=(*c).real_val(0);
   
        if(m.prim_key==NULL){
            
                std::string val=std::to_string(*((double*)m.mp[0]));
                const char* myCString = val.c_str();
                //std::cout<<"expr:"<<myCString<<"\n";
                ans=(*c).real_val(myCString);
            

        }
        else{
            for(auto &a:m.mp){
                    
                
                z3::expr temp=print_sym(*(multinomial*)(a.second),sub_map);
                if(a.first==0){
                    ans=ans+temp;
                }
                else{
                    //probably not a good practice
                    if(sub_map==NULL || (*sub_map).find(m.prim_key->get_id())==(*sub_map).end()){
                        z3::expr pw_e=z3::pw(e[m.prim_key->get_id()],a.first);
                        ans=ans+pw_e*temp;
                    }
                    else{
                        std::string val=std::to_string(pow((*sub_map)[m.prim_key->get_id()],a.first));
                        const char* myCString = val.c_str();

                        z3::expr mul_fac=(*c).real_val(myCString);
                        ans=ans+temp*(mul_fac);
                    }
                }
            }
        }
        return ans;
    }

    GRBLinExpr print_sym_GRB(multinomial& m,std::vector<GRBVar>& e_GRB,std::map<int,double> *sub_map=NULL){
        //we are promised that these expressions are linear::
        GRBLinExpr ans(0);
   
        if(m.prim_key==NULL){
            
        
                ans=GRBLinExpr(*((double*)m.mp[0]));
            

        }
        else{
            for(auto &a:m.mp){
                    
                
                
                if(a.first==0){
                    GRBLinExpr temp=print_sym_GRB(*(multinomial*)(a.second),e_GRB,sub_map);
                    //ans=ans+temp;
                    ans=ans+temp;
                }
                else{
                    //probably not a good practice
                    if((sub_map==NULL || (*sub_map).find(m.prim_key->get_id())==(*sub_map).end()) && a.first>0){
                        GRBLinExpr pw_e(e_GRB[m.prim_key->get_id()]);
                        double temp=eval_sym_GRB(*(multinomial*)(a.second),sub_map);
                        if(a.first>1 && temp!=0){
                            std::cout<<"GRB: Higher degree term detected"<<"\n";
                        }
                        //ans=ans+pw_e*temp;
                        pw_e*=temp;

                        ans=ans+pw_e;
                    }
                    else if(a.first==0){
                        GRBLinExpr temp=print_sym_GRB(*(multinomial*)(a.second),e_GRB,sub_map);
                        ans=ans+temp;
                        // ans.multAdd(1,temp);
                    }
                    else{
                        GRBLinExpr temp=print_sym_GRB(*(multinomial*)(a.second),e_GRB,sub_map);
                        double val=(pow((*sub_map)[m.prim_key->get_id()],a.first));
                        
                        temp*=val;
                        ans=ans+temp;
                    }
                }
            }
        }
        return ans;
    }

    double eval_sym_GRB(multinomial& m,std::map<int,double> *sub_map=NULL){
        //we are promised that these expressions are linear::

        double ans=0;
   
        if(m.prim_key==NULL){
            
        
                ans=*((double*)m.mp[0]);
            

        }
        else{
            for(auto &a:m.mp){
                    
                
                double temp=eval_sym_GRB(*(multinomial*)(a.second),sub_map);
                if(a.first==0){
                    
                    ans=ans+temp;
                }
                else{
                    //probably not a good practice
                    if(sub_map==NULL || (*sub_map).find(m.prim_key->get_id())==(*sub_map).end()){
                        
                        if(a.first>0 && temp!=0){
                            std::cout<<"GRB: Higher degree term detected[eval]"<<"\n";
                        }
                        ans=ans+temp;
                    }
                    else{
                       
                        double val=(pow((*sub_map)[m.prim_key->get_id()],a.first));
                        
                        ans=ans+temp*(val);
                    }
                }
            }
        }
        return ans;
    }

    term_t print_sym_yices(multinomial& m){
        //term_t ans=yices_zero();
        std::vector<term_t> ans;
        ans.push_back(yices_zero());

        // std::cout<<"yices Term:\n------------------------------";
        // yices_pp_term(stdout, ans[ans.size()-1], 80, 8, 0);
        // std::cout<<"===============\n";


        // std::cout<<"yices Term:\n------------------------------";
        // yices_pp_term(stdout, yices_zero(), 80, 8, 0);
        // std::cout<<"===============\n";

        // term_t temp_=yices_zero();
        // std::cout<<"yices Term:\n------------------------------";
        // yices_pp_term(stdout, temp_ ,80, 8, 0);
        // std::cout<<"===============\n";

        if(m.prim_key==NULL){
            

                int64_t numerator = static_cast<int64_t>(((double)(*(double*)m.mp[0])) * 100);  // Scale by 100 for two decimal places
                int64_t denominator = 100;

                // Create a rational term representing x
                term_t term = yices_rational64(numerator, denominator);
                ans.push_back(term);

        }
        else{
            for(auto &a:m.mp){
                    
                
                term_t temp=print_sym_yices(*(multinomial*)(a.second));
                // std::cout<<"yices Term:\n------------------------------";
                // yices_pp_term(stdout, temp, 80, 8, 0);
                // std::cout<<"===============\n";
                if(a.first==0){
                    term_t temp_ans=yices_add(ans[ans.size()-1],temp);
                    ans.push_back(temp_ans);
                }
                else{
                    term_t pw_e=yices_power(e_yices[m.prim_key->get_id()],a.first);
                    ans.push_back(yices_add(ans[ans.size()-1],yices_mul(pw_e,temp)));
                }
            }
        }

        //std::cout<<ans<<"\n";
        // std::cout<<"yices Term:\n------------------------------";
        // yices_pp_term(stdout, ans[ans.size()-1], 80, 8, 0);
        // std::cout<<"===============\n";
        return ans[ans.size()-1];
    }
bool smt_query(std::vector<handelman::expr>& e_check,std::vector<std::pair<double,double>> &coords ){
    z3::solver s(*c);
    z3::params p(*c);
    //p.set(":arith.auto_config_simplex",true);
    s.set(p);

    for(int i=0;i<coords.size();i++){
         z3::expr e1 = e[i]-(*c).real_val(std::to_string(coords[i].first).c_str());
         z3::expr e2= -1*e[i] + (*c).real_val(std::to_string(coords[i].second).c_str());
         
         s.add(e1>=0);
         s.add(e2>=0);


    }

    for(handelman::expr& poly: e_check){
         z3::expr e1= print_sym(*(poly.m));
         s.add(e1>=0);

    }

    auto result=s.check();

    if (result == z3::sat) {
            return true;
    } else if (result == z3::unsat) {
        //std::cout<<"unsat"<<"\n";
        return false;
    } else {
        //throw std::string("Timeout"); //throw and exception if we are unsure of the result::
        return false;
    }
}

bool solve(int silent,std::map<int, double> &sub_map,std::string solver_type="z3"){
        //create expr variables::
        
        int n=v->sz();
        
        if(solver_type=="z3"){
            data_mutex.lock();
            
        
            for(int i=e.size();i<n;i++){
                const char* myCString = ("var_"+std::to_string(i)).c_str();
                z3::expr t=(*c).real_const(myCString);
                e.push_back(t);
            }
            data_mutex.unlock();
        
            z3::solver s(*c);
            z3::params p(*c);
            //p.set(":arith.auto_config_simplex",true);
            s.set(p);
            
            //add equations to solver::
            for(auto &a: eq){
                //print_vector(a->print());
                z3::expr e_=print_sym(*a,&sub_map);
                //std::cout<<e_<<"\n";
                s.add(e_==0);
            }
            
            for(auto &a: ineq){
                //print_vector(a->print());
                z3::expr e_=print_sym(*a,&sub_map);
                //std::cout<<e_<<"\n";
                s.add(e_>=0);
            }

            for(auto &a: mod_constraints){
                
                Z3_ast from[sub_map.size()] ;
                Z3_ast to[sub_map.size()] ;

                int ctr=0;
                for(auto& elem: sub_map){
                    from[ctr]=e[elem.first];
                    z3::expr sub= (*c).real_val(std::to_string(elem.second).c_str());
                    to[ctr]=sub;
                    ctr+=1;
                }
                z3::expr new_f(*c);
                new_f = z3::to_expr(*c, Z3_substitute(*c, a, sub_map.size(), from, to));
                //std::cout<<new_f<<"\n";
                s.add(new_f);

            }
            

            //std::cout << s.to_smt2() << "\n";
            


            unsigned timeout_ms = 10;  // 1000 milliseconds = 1 second
            z3::check_result result;
            
            bool done=false;
            // Start a separate thread for solving
            std::thread solverThread([&]() {
                result = s.check();
                // if (result == z3::sat) {
                //     return true;
                // } else if (result == z3::unsat) {
                //     return false;
                // } else {
                //     return false;
                // }
                // std::lock_guard<std::mutex> lock(mutex);
                done=true;

            });

            // Sleep for the specified timeout duration
            int n_timeout=0;
            while(!done){
                std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));//look every 10 milliseconds
                n_timeout+=1;

                if(n_timeout>=1000){ //5 seconds as limit for any lp solver
                    break;
                }
            }

            // Terminate the solver thread if it's still running
            if (solverThread.joinable()) {
                // std::lock_guard<std::mutex> lock(mutex);

                if(!done){
                    solverThread.detach();
                    std::cout << "Timeout reached" << std::endl;
                    result=z3::check_result::unknown;
                }
                else{
                    solverThread.join();
                }
            }
            
          

            // z3::params p(*c);
            // //p.set("timeout",10u);
            // s.set(p);

            // result= s.check();
            // // std::cout<<"size of:"<<sizeof(*c)<<"\n";
            if (result == z3::sat) {
                    return true;
            } else if (result == z3::unsat) {
                std::cout<<"unsat"<<"\n";
                return false;
            } else {
                throw std::string("Timeout"); //throw and exception if we are unsure of the result::
                return false;
            }
            
           
           

            
        }
        else if(solver_type=="GRB"){
            //try{
            std::vector<GRBVar> e_GRB;

            GRBEnv env= GRBEnv();
            env.set(GRB_IntParam_OutputFlag, 0);
            GRBModel model= GRBModel(env);
           
            //model.getEnv().set(GRB_IntParam_OutputFlag, 0); 
            
            
            //data_mutex.lock();
            for(int i=e_GRB.size();i<n;i++){
                std::string myString = ("var_"+std::to_string(i));
                e_GRB.push_back(model.addVar(0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, myString));
               
            }
            //data_mutex.unlock();
            
            
            
            for(auto &a: ineq){
                //print_vector(a->print());
                GRBLinExpr e_=print_sym_GRB(*a,e_GRB,&sub_map);
                //std::cout<<e_<<"\n";
                model.addConstr(e_>=0);
            }

            //add equations to solver::
            for(auto &a: eq){
                //print_vector(a->print());
                GRBLinExpr e_=print_sym_GRB(*a,e_GRB,&sub_map);
                
                model.addConstr(e_>=0);
                model.addConstr(e_<=0);

            }
            
            

           
            

            //std::cout << s.to_smt2() << "\n";
            


            GRBLinExpr obj=0;
            model.setObjective(obj,GRB_MAXIMIZE);
            model.optimize();

            if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
                std::cout << "Feasible solution found." << std::endl;
                return true;
            } else {
                std::cout << "No feasible solution found." << std::endl;
                return false;
            }
            //}

            // catch (GRBException& e) {
            //     std::cerr << "Error code = " << e.getErrorCode() << std::endl;
            //     std::cerr << e.getMessage() << std::endl;
            //     exit(1);
            // } catch (...) {
            //     std::cerr << "Exception during optimization" << std::endl;
            //     exit(1);
            // }
        }
        else if(solver_type=="yices"){
        
            
            

            yices_init();
            ctx_config_t* config= yices_new_config();
            yices_set_config(config, "solver-type", "mcsat");
            yices_default_config_for_logic(config,"QF_NIA");

            context_t *ctx=yices_new_context(config);


            

            
            for(int i=0;i<n;i++){
                const char* myCString = ("var_"+std::to_string(i)).c_str();
                term_t t = yices_new_uninterpreted_term(yices_real_type());
                yices_set_term_name(t,myCString);
                e_yices.push_back(t);
            }
            //add equations to solver::
            for(auto &a: eq){
                //print_vector(a->print());
                term_t e_=print_sym_yices(*a);
                // std::cout<<"yices Term:\n------------------------------";
                // yices_pp_term(stdout, e_, 80, 8, 0);
                // std::cout<<"===============\n";
                yices_assert_formula(ctx,yices_arith_eq_atom(e_,yices_zero()));
            }
            if(silent){
            std::cout<<"INEQ===========\n============\n";
            }
            for(auto &a: ineq){
                //print_vector(a->print());
                term_t e_=print_sym_yices(*a);
                //std::cout<<e_;
                yices_assert_formula(ctx,yices_arith_geq_atom(e_,yices_zero()));
            }
            auto result = yices_check_context(ctx, NULL);
            if (result == STATUS_SAT) {
                return true;
            } else if (result == STATUS_UNSAT) {
                return false;
            } else {
                return false;
            }

            yices_free_context(ctx);
            yices_exit();
        }
        else if(solver_type=="pgd"){
            //not complete implementation::
            handelman::expr lgr;
            lgr=0;

            for(auto &a: eq){
                std::string ref=v->add_var(true,"additive_template_lgr");
                lgr=lgr+(*a)*((*v)[ref]);
            }
            if(silent){
                std::cout<<"INEQ===========\n============\n";
            }
            std::vector<int> proj_id_arr;//projection variables::
            for(auto &a: ineq){
                std::string ref=v->add_var(true,"additive_template_lgr");
                lgr=lgr+(*a)*((*v)[ref]);
                proj_id_arr.push_back((*v)[ref].prim_key->get_id());
            }

            // mlpack::optimization::PGD pgd;
            // pgd.MaxIterations() = 1000;  // Set the maximum number of iterations.
            // pgd.StepSize() = 0.01;      // Set the step size.
            // pgd.Projection() = [](arma::mat& coords) {
            //     for (size_t i = 0; i < proj_id_arr.size(); ++i)
            //     {
            //         coords[i] = std::max(0.0, coords[proj_id_arr[i]]);
            //     }
            // };

            // // Initial point.
            // arma::mat initialPoint = arma::randu<arma::mat>( v->sz(),1);  // Random initial point.

            // // Optimize using your custom function 'g'.
            // arma::mat coordinates;
            // double result = pgd.Optimize([](const arma::mat& coords) {
            //     // Lambda function as the objective.
            //     std::vector<double> coordVector(coords.begin(), coords.end());
            //     double objectiveResult = lgr.substitute(coordVector);
            //     return objectiveResult;
            // }, initialPoint, coordinates);

            // std::cout << "Optimal coordinates: " << coordinates << std::endl;
        }
        
        return false;


    }
};



#endif;