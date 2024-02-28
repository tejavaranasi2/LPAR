#include "vars.hpp"
#include "expr.hpp"
#include "solver.hpp"
#include "handelman_core.hpp"
#include <ctime>
#include "approximation.hpp"
#include "parser.hpp"
using namespace handelman;

std::ofstream LOG_FILE;
int main(int argv, char* argc[]){
    vars v(4); //(x,y)
    // std::string c[3];
    // c[0]=v.add_var(3,false,"c1");
    // c[1]=v.add_var(3,false,"c2");
    // c[2]=v.add_var(3,false,"c3");
    
    // //templates::
    // std::string t[4];
    // t[0]=v.add_var(6,true,"t1");
    // t[1]=v.add_var(6,true,"t2");
    // t[2]=v.add_var(6,true,"t3");

   

    
   //  int ctr_i= std::stoi(argc[4]);
   int ctr_i=1;
    LOG_FILE=std::ofstream("../logs/output_tc10_prog_sys_parallel10-"+std::to_string(ctr_i)+".log");
    double ctr=ctr_i/2;
    std::vector<std::vector<double> > circles={{10,10,10,10,15},{20,20,20,20,15},{30,30,30,30,30}};
    std::vector<std::vector<double> > coeff={{1,1,1,1},{1,1,1,1},{1,1,1,1}};
    std::vector<solver*> sls_pos,sls_neg;
    std::string t;
    t=v.add_var(8,true,"t4");
    ll* ans=NULL;
    
    std::map<std::pair<int,int>, int> id_map;
    std::vector<std::pair<double,double>> coords;

    id_map[std::pair<int,int>(coords.size(),0)]=v(t,0).prim_key->get_id();
    id_map[std::pair<int,int>(coords.size(),1)]=v(t,4).prim_key->get_id();
    coords.push_back(std::pair<double,double>(0,40));

    
    id_map[std::pair<int,int>(coords.size(),0)]=v(t,1).prim_key->get_id();
    id_map[std::pair<int,int>(coords.size(),1)]=v(t,5).prim_key->get_id();
    coords.push_back(std::pair<double,double>(0,40));
    
    id_map[std::pair<int,int>(coords.size(),0)]=v(t,2).prim_key->get_id();
    id_map[std::pair<int,int>(coords.size(),1)]=v(t,6).prim_key->get_id();
    coords.push_back(std::pair<double,double>(0,40));

    id_map[std::pair<int,int>(coords.size(),0)]=v(t,3).prim_key->get_id();
    id_map[std::pair<int,int>(coords.size(),1)]=v(t,7).prim_key->get_id();
    coords.push_back(std::pair<double,double>(0,40));


    std::vector<expr> e_check;
    for(int i=0;i<2;i++){

        handelman_core e1_(v);
        handelman_core e2_(v);
        
         
        

        //template conditions::

        // for(int i=0;i<3;i++){
        //     for(int j=0;j<3;j++){
        //         e_.add_lhs(v(c[i],j)+v(t[i],j)*-1);
        //         e_.add_lhs(v(t[i],j+3)+v(c[i],j)*-1);

        //         coords.push_back(std::pair<double,double>(0,100));
        //         id_map[std::pair<int,int>(coords.size(),0)]=v(t[i],j).prim_key->get_id();
        //         id_map[std::pair<int,int>(coords.size(),1)]=v(t[i],j+3).prim_key->get_id();
        //     }
        // }
        

        e1_.add_lhs(v[0]+v(t,0)*-1);
        e1_.add_lhs(v(t,4)+v[0]*-1);

        e1_.add_lhs(v[1]+v(t,1)*-1);
        e1_.add_lhs(v(t,5)+v[1]*-1);
        
        e1_.add_lhs(v[2]+v(t,2)*-1);
        e1_.add_lhs(v(t,6)+v[2]*-1);

        e1_.add_lhs(v[3]+v(t,3)*-1);
        e1_.add_lhs(v(t,7)+v[3]*-1);
        
        
       
        
        e2_.add_lhs(v[0]+v(t,0)*-1);
        e2_.add_lhs(v(t,4)+v[0]*-1);

        e2_.add_lhs(v[1]+v(t,1)*-1);
        e2_.add_lhs(v(t,5)+v[1]*-1);
        
        e2_.add_lhs(v[2]+v(t,2)*-1);
        e2_.add_lhs(v(t,6)+v[2]*-1);

        e2_.add_lhs(v[3]+v(t,3)*-1);
        e2_.add_lhs(v(t,7)+v[3]*-1);


        
       
       

        
        
        //add_rhs_equations::
        expr e_c[2];
        
       

        e_c[0]=(((v[0]*-1+circles[i][0])^2)*(coeff[i][0])+((v[1]*-1+circles[i][1])^2)*(coeff[i][1])+((v[2]*-1+circles[i][2])^2)*(coeff[i][2])+((v[3]*-1+circles[i][3])^2)*(coeff[i][3]))*(-1)+(circles[i][4]*circles[i][4]);

        e_c[0].set_save(true);
        e1_.add_rhs(e_c[0]);
        
        e_c[1]=e_c[0]*(-1);

        e2_.add_rhs(e_c[1]);
        
        e_check.push_back(e_c[0]);

        e_check[i].set_save(true);

        e1_.set_max_degree(2);
        e1_.solve(3,0);

        e2_.set_max_degree(2);
        e2_.solve(3,0);
        
        sls_pos.push_back(e1_.solver_());
        sls_neg.push_back(e2_.solver_());
    }
    //can parallelize them::
    fragmentation_intersection* f;
    std::string flag_(argc[3]);
    int flag=std::stoi(flag_);
    std::string dump_file(argc[2]);
    std::string _file(argc[1]);
    if(flag==1){
       
       f= new fragmentation_intersection(coords,id_map,sls_pos,sls_neg,dump_file);
    }
    else if(flag==0 || flag==2){
       f= new fragmentation_intersection(coords,id_map,sls_pos,sls_neg); 
    }
   //  else{
   //     f= new fragmentation_intersection(coords,id_map,sls_pos,sls_neg,_file,true);
   //  }
    
    ll* new_l;
    if(flag==2){
      new_l= new ll();
      new_l->read_dump_trace(_file,coords,sls_pos.size(),true);
    }
    f->run_threads(80000,0.7,dump_file,NULL,20,true);
    f->recompute_confidence(e_check);//makes SMT calls::
    if(flag==2){
      f->frags_()=operation(new_l,f->frags_(),0);
    }
    f->frags_()->print("/results_prog_sys/results10-"+(std::string)argc[4]+".csv");

    trie_node* HEAD= trie_node::make_trie(f->frags_(),4,coords);
    HEAD->write_json("/home/poqer/Desktop/tests/tc_10.json",coords);
    // for(int i=0;i<3;i++){
    //     fragmentation f(coords,id_map,sls_pos[i],sls_neg[i]);
   
    //     f.run_iters(3000,0.7);

    //     if(ans==NULL){
    //         ans=f.frags_();
    //     }
    //     else{
    //         ans=operation(ans,f.frags_(),0);
    //     }
    // }
    // color_code(ans,sls_pos,sls_neg,id_map);,
    // ans->print("results4.csv");
    sl_parser::make_test_case(e_check,coords,v,3,"test10",f->frags_());

    // ll* cleaned_ll=trie_node::clean_ll(f->frags_(),2,coords);
    
    // cleaned_ll->dump_trace(_file,f->curr_box_(),2);
    // cleaned_ll->print("/results_z3/results6_elim-"+(std::string)argc[4]+".csv");
     

    LOG_FILE.close();
    
    return 111;//special code for my test case::

    
}