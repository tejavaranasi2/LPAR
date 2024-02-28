#include "solver.hpp"

std::vector<int> get_vars(z3::expr e,z3::context &c){
    //returns indices of vars::
    std::vector<int> ans;
    
    
    //std::cout<<e<<"\n";
    if(e.num_args()==0){
        try{
            std::string name=e.decl().name().str();
            name=name.substr(4);
            // if(std::stoi(name)<5){
            //     std::cout<<e<<"\n";
            //     int x=1;
            // }
            ans.push_back(std::stoi(name));
        }
        catch(...){
            //do nothing::
        }
    }
    else{
        unsigned num_args=e.num_args();
        for(int i=0;i<num_args;i++){
            for(auto& elem: get_vars(e.arg(i),c)){
                ans.push_back(elem);
            }
        }

    }

    //std::cout<<e<<"\n";



    return ans;
}

solver::~solver(){
    //delete all the created instances::
}