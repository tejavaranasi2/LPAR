#ifndef PARSER_H
#define PARSER_H


#include "expr.hpp"
#include "vars.hpp"

namespace POQER{
    class pars{
        std::string file_name;
        std::vector<handelman::expr> eq;//equalities::
        std::vector<handelman::expr> ineq;//inequalities:: 

        public:

        pars(std::string afile_name){
            file_name=afile_name;

            //parse the file to create and return the constraints::
        }

        

    };



};

namespace sl_parser{
    std::string to_string_smt(double x){
        if(x>=0){
            return std::to_string((int)x);
        }
        else{
            return "( - "+std::to_string((int)(-1*x))+" )";
        }
    }
    std::string to_string_smt(int x){
        if(x>=0){
            return std::to_string(x);
        }
        else{
            return "( - "+std::to_string(-1*x)+" )";
        }
    }
    void make_test_case(std::vector<handelman::expr> &e_check,std::vector<std::pair<double,double>> &coords,vars &v,int idx,std::string file_name,ll* frags=NULL){
         std::ofstream fp("../../../tests/G"+file_name+".sl");

         fp<<"(set-logic NIA)\n";
         
         std::string var_dep="";
         std::string arg_dep="";
         for(int i=0;i<idx;i++){
            var_dep+="(x"+sl_parser::to_string_smt(i)+" Int) ";
            arg_dep+="x"+sl_parser::to_string_smt(i)+" ";
            fp<<(std::string)"(declare-var "+(std::string)"x"+sl_parser::to_string_smt(i)+" Int)\n";
            //fp<<"(assume (and (<= "+sl_parser::to_string_smt(coords[i].first)+" x"+sl_parser::to_string_smt(i)+") (<= "+"x"+sl_parser::to_string_smt(i)+" "+sl_parser::to_string_smt(coords[i].second)+") ) )\n";
         }

         fp<<"(assume "+frags->smt_code(idx)+" )\n";
         for(int i=idx;i<v.sz();++i){
            if(!v.is_templ(i)){
                fp<<(std::string)"(declare-var "+(std::string)"x"+sl_parser::to_string_smt(i)+" Int)\n";
                fp<<"(assume (and (<= "+sl_parser::to_string_smt(coords[i].first)+" x"+sl_parser::to_string_smt(i)+") (<= "+"x"+sl_parser::to_string_smt(i)+" "+sl_parser::to_string_smt(coords[i].second)+") ) )\n";
            }
         }
         for(int i=idx;i<v.sz();++i){
            if(!v.is_templ(i)){
                fp<<"(synth-fun f"+sl_parser::to_string_smt(i)+" ("+var_dep+") Int)\n";
            }
         }


         for(auto& e: e_check){
            fp<<"(constraint (<= 0 "+e.smt_print(arg_dep,true,idx)+" ) )\n";
         }

         for(int i=idx;i<v.sz();++i){
            if(!v.is_templ(i)){
                fp<<"(constraint (and (<= "+sl_parser::to_string_smt(coords[i].first)+" ( f"+sl_parser::to_string_smt(i)+" "+arg_dep+") ) (<= "+"( f"+sl_parser::to_string_smt(i)+" "+arg_dep+") "+sl_parser::to_string_smt(coords[i].second)+") ) ) \n";
            }
         }

         //constraint of max or min::

         fp<<"(constraint (=> (and ";
         
         for(auto& e: e_check){
            fp<<"(<= 0 "+e.smt_print(var_dep,false,idx)+" ) ";
         }

         fp<<") (or (<= 1 0) ";
         
         std::string tr_expr="";
         for(int i=idx;i<v.sz();++i){
            if(!v.is_templ(i)){
                fp<<"(and "+tr_expr+ "(<= x"+sl_parser::to_string_smt(idx)+" ( f"+sl_parser::to_string_smt(i)+" "+arg_dep+") ) )\n";

                tr_expr+=("(= x"+sl_parser::to_string_smt(idx)+" ( f"+sl_parser::to_string_smt(i)+" "+arg_dep+")");
            }
         }

         fp<<") ) )\n";

         fp<<"(check-synth)\n";
    }
}





#endif;