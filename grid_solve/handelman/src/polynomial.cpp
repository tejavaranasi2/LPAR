#include "multinomial.hpp"

//we also define the function that returns a polynomial from symbol::
int created_polys=0;
int deleted_polys=0;
multinomial& multinomial::operator+(const double& m){
    //adding double to polynomial::
    if(!save){
        if(prim_key==NULL){
            *(double*)mp[0]=*(double*)mp[0]+m;
        }
        else{
            if(mp.find(0)==mp.end()){
                mp[0]=new multinomial();
            }
            mp[0]=&(*(multinomial*)mp[0]+m);
            
            //zero simplification::
            if(mp.size()==1 && ((multinomial*)mp[0])->is_zero()){
                prim_key=NULL;
                ((multinomial*)mp[0])->custom_delete(true);
                delete ((multinomial*)mp[0]);
                mp[0]=new double(0);
            }
        }
        return *this;
    }
    else{
        multinomial* ans=new multinomial();
        ans->prim_key=prim_key;
        for(auto& a:mp){
            if(a.first==0){
                if(prim_key==NULL){
                    *(double*)ans->mp[0]=*(double*)a.second+m;
                }
                else{
                     ans->mp[a.first]=&(*((multinomial*)a.second)+m);
                }
            }
            else{
                multinomial* temp=new multinomial();
                *temp=*(multinomial*)a.second;
                ans->mp[a.first]=temp;
                
            }
        }
        ans->simplify();
        return *ans;
    }
}
multinomial& multinomial::operator=(multinomial& rhs){
    //copy in the case of save is a heavy operation(try and avoid)::
    if(rhs.save){
        if(save){
            custom_delete(true);
        }
        else{
            custom_delete(false);
        }
        prim_key=rhs.prim_key;
        if(prim_key==NULL){
           double* temp=new double(*(double*)rhs.mp[0]);
           mp[0]=temp;
        }
        else{
            for(auto& a:rhs.mp){
                multinomial* temp=new multinomial();
                *temp=*(multinomial*)a.second;
                mp[a.first]=temp;
            }
        }
    }
    else{
      mp=rhs.mp;
      prim_key=rhs.prim_key;
      save=rhs.save;

      delete &rhs;//call delete operator to ensure no memory leaks::
    }
    return *this;
}

multinomial& multinomial::operator+(multinomial& m){
   multinomial* ans;
   if(prim_key==NULL){
      ans=&(m+*(double*)mp[0]);
      if(!save){
          custom_delete(true);
          delete this;//renders this object= invalid after here::
      }

      return *ans;
   }
   else if(m.prim_key==NULL){
      ans=&(*this+*(double*)m.mp[0]);
      if(!m.save){
        m.custom_delete(true);
        delete &m;
      }

      return *ans;
   }
   else{
      ans=new multinomial();
      if(*prim_key>*m.prim_key){
         ans->prim_key=m.prim_key;
         for(auto& a:m.mp){
            if(a.first!=0){
                ans->mp[a.first]=new multinomial();
                *(multinomial*)ans->mp[a.first]=*(multinomial*)a.second;
            }
            else{
                ans->mp[a.first]=&((*(multinomial*)a.second+*this));
                //do not use this after this point::
            }
         }
         if(!m.save){
            delete &m;
        }
         
         return *ans;
      }
      else if(*prim_key<*m.prim_key){
         ans->prim_key=prim_key;
         for(auto& a:mp){
            if(a.first!=0){
                ans->mp[a.first]=new multinomial();
                *(multinomial*)ans->mp[a.first]=*(multinomial*)a.second;
            }
            else{
                ans->mp[a.first]=&((*(multinomial*)a.second+m));
            }
         }
         if(!save){
            delete this;
         }

         return *ans;
      }
      else{
         ans->prim_key=prim_key;
         for(auto& a:mp){
            if(m.mp.find(a.first)==m.mp.end()){
                ans->mp[a.first]=new multinomial();
                *(multinomial*)ans->mp[a.first]=*(multinomial*)a.second;
            }
            else{
                ans->mp[a.first]=new multinomial();
                *(multinomial*)ans->mp[a.first]=*(multinomial*)a.second+*(multinomial*)m.mp[a.first];
            }
         }
         for(auto& a:m.mp){
            if(mp.find(a.first)==mp.end()){
                ans->mp[a.first]=new multinomial();
                *(multinomial*)ans->mp[a.first]=*(multinomial*)a.second;
            }
         }
         
         if(this==&m){
               if(!save){
                    delete this;
               }
            }
        else{
            if(!save){
                delete this;
            }
            if(!m.save){
                delete &m;
            }
        }
        ans->simplify();

         return *ans;
      }
   }
   return *ans;//code should not ideally reach this point::
}
multinomial* multinomial::simplify(){
    //do not simplify save::
    if(save){
        return this;
    }
    if(prim_key==NULL){
        return this;
    }
    for(auto it=mp.cbegin();it!=mp.cend();){
           if(((multinomial*)it->second)->is_zero()){
             ((multinomial*)it->second)->custom_delete(true);
             delete ((multinomial*)it->second);
             it=mp.erase(it);//can find a better way to do this::
           }
           else{
            ++it;
           }
    }
         //make it zero polynomial::
    if((mp).size()==0){
        prim_key=NULL;
        mp[0]=new double(0);
    }
    if(mp.find(0)==mp.end()){
        //we always have a zero degree::(for convenience)::
        mp[0]=new multinomial(0);
    }
    return this;
}
bool multinomial::is_zero(){
    if(prim_key==NULL){
        if(*(double*)mp[0]==0){
            return true;
        }
    }
    return false;
}
multinomial& multinomial::operator*(multinomial& m){
    int sz=mp.size();
    bool is_m_save=m.save;//we will finally set these values::
    bool is_save=save;
    int sz_m=m.mp.size();
    multinomial* ans=new multinomial();//[MEM_LEAK]
    ans->mp.erase(0);
    //base cases::
    if(prim_key==NULL){
        ans=&(m*(*(double*)mp[0]));
        if(!save){
            custom_delete(true);
            delete this;
        }
        return *ans;
    }
    else if(m.prim_key==NULL){
        ans=&(*this*(*(double*)m.mp[0]));
        if(!is_m_save){
            m.custom_delete(true);
            delete &m;
        }
        return *ans;
    }
    else{
        if(*prim_key>*m.prim_key){
           ans->prim_key=m.prim_key;
           if(sz_m>1 && !is_save){
             set_save(true);
           }
           for(auto &a:m.mp){
             ans->mp[a.first]=&(*(multinomial*)a.second*(*this));
             sz_m-=1;
             if(sz_m==1){
                set_save(is_save);//(set_save does nothing if already there::)
             }
           }
           ans->simplify();
           return *ans;
        }
        else if(*prim_key<*m.prim_key){
           ans->prim_key=prim_key;
           if(sz>1 && !is_m_save){
             m.set_save(true);
           }
           for(auto &a:mp){
             ans->mp[a.first]=&(*(multinomial*)a.second*(m));
             sz-=1;
             if(sz==1){
                m.set_save(is_m_save);//(set_save does nothing if already there::)
             }
           }
           ans->simplify();
           return *ans;
        }
        else{
            //difficult case::
            ans->prim_key=prim_key;
            if(sz>1 && !is_m_save){
                m.set_save(true);
            }
            for(auto &a:mp){
                int sz_m_temp=sz_m;
                if(sz_m_temp>1 && !is_save){
                    ((multinomial*)a.second)->set_save(true);
                }
                for(auto &b:m.mp){
                    if(ans->mp.find(b.first+a.first)==ans->mp.end()){
                        ans->mp[a.first+b.first]=&((*(multinomial*)a.second) * (*(multinomial*)b.second));
                    }
                    else{
                        ans->mp[a.first+b.first]=&(*(multinomial*)(ans->mp[a.first+b.first])+(*(multinomial*)a.second) * (*(multinomial*)b.second));
                    }
                    sz_m_temp-=1;
                    if(sz_m_temp==1){
                        ((multinomial*)a.second)->set_save(is_save);
                    }
                }
                sz-=1;
                if(sz==1){
                    m.set_save(is_m_save);
                }
            }
            if(this==&m){
               if(!is_save){
                    delete this;
               }
            }
            else{
                if(!is_save){
                    delete this;
                }
                if(!is_m_save){
                    delete &m;
                }
            }

            ans->simplify();

            return *ans;

        }
    }
}  

multinomial& multinomial::operator*(const double& c){
    if(!save){
        if(c==0){
        //make it a zero polynomial(in any other case(poly cant be zero))
            prim_key=NULL;
            mp.clear();
            //should delete the older contenets?
            mp[0]=new double(0);
        }
        else{
            if(prim_key!=NULL){
                for(auto &a:mp){
                    a.second=&((*(multinomial*)a.second)*c);
                }
            }
            else{
                *(double*)mp[0]=(*(double*)mp[0])*c;
            }
        }
        return *this;
    }
    else{
        multinomial* ans=new multinomial();
        ans->prim_key=prim_key;
        for(auto& a:mp){
                if(prim_key==NULL){
                    *(double*)ans->mp[0]=(*(double*)a.second)*c;
                }
                else{
                    ans->mp[a.first]=&(*((multinomial*)a.second)*c);
                }
        }
        ans->simplify();
        return *ans;
    }
}



vec_str multinomial::print(){
    vec_str ans;
   
    if(prim_key==NULL){
        if(!(*((double*)mp[0])==0)){
            ans.push_back(std::to_string(*((double*)mp[0])));
        }

    }
    else{
        for(auto &a:mp){
           std::string sym="(s_"+std::to_string(prim_key->get_id())+"^"+std::to_string(a.first)+")";
           if(a.first==0){
            sym="";
           }
           vec_str temp=((multinomial*)(a.second))->print();
           for(int i=0;i<temp.sz;i++){
                std::string s=temp[i];
                if(s=="$"){
                    break;
                }
                ans.push_back(s+sym);
           }
        }
    }
    ans.push_back("$");
    return ans;
}

std::string multinomial::smt_print(std::string var_dep,bool fun_names,int idx)
{
    std::string ans="";

    if(prim_key==NULL){

        if(*((double*)mp[0]) >= 0){
          ans+=std::to_string(*((double*)mp[0]));
        }
        else{
          ans+="( - ";
          ans+=std::to_string(-1* (*((double*)mp[0])));
          ans+=")";
        }
        
    }
    else{
        if(mp.size()>1){
            ans+="(+ ";
        }
        for(auto &a:mp){
           std::string sym="(s_"+std::to_string(prim_key->get_id())+"^"+std::to_string(a.first)+")";
           std::string sym_temp="";
           if(fun_names){
              if(prim_key->get_id()>=idx){
                 sym_temp = "( f"+std::to_string(prim_key->get_id())+" "+var_dep+")";
              }
              else{
                 sym_temp = "x"+std::to_string(prim_key->get_id());
              }
           }
           else{
                sym_temp="x"+std::to_string(prim_key->get_id());
           }
           if(a.first==1){
              sym=sym_temp;
           }
           else if(a.first==0){
            sym="1";
           }
           else{
               sym="( * ";
               for(int i=0;i<a.first;++i){
                 sym+=sym_temp;
                 sym+=" ";
               }
               sym+=")";
           }
           std::string temp=((multinomial*)(a.second))->smt_print(var_dep,fun_names,idx);

           ans+="(* "+sym+" "+temp+") ";
           
        }

        if(mp.size()>1){
            ans+=")";
        }
    }

    return ans;
}

multinomial& multinomial::operator^(const uint16_t& pow){
   //do a simple binary exponentiation (slow)
   
   multinomial *temp=&((*this)*1);
   uint16_t cpow=pow;
   if(pow==1){
     return *temp;
   }
   else if(pow==2){
     multinomial* ans;
     temp->set_save(true);
     ans=&((*temp)*(*temp));
     temp->set_save(false);

     temp->custom_delete(true);
     delete temp;
     
     

     return *ans;
   }
   else{
        multinomial* ans=new multinomial(1);
    
        while(cpow>0){
            if(cpow%2==1){
                temp->set_save(true);
                ans=&((*ans)*(*temp));
                temp->set_save(false);
            }
            cpow=(cpow-cpow%2)/2;
            if(cpow==0){
                break;
            }
            temp=&(*temp^2);
        }

       
        return *ans;
        
   }
   
  //return NULL
   
}

void multinomial::custom_delete(bool nested){
    //ensure that all polynomials are allocated via new::
    //debug message::
    //std::cout<<"Deleting polynomial:["<<nested<<"]\n"<<print_vector(print())<<"\n";
    save=false;
    //std::cout<<"Deleted1\n";
    if(nested){
        if(prim_key==NULL){
            delete((double*)mp[0]);
        }
        else{
            for(auto& a:mp){
                if(a.second!=NULL){
                    ((multinomial*)a.second)->custom_delete(nested);
                    delete ((multinomial*)a.second);
                }
            }
        }
    }
    //std::cout<<"Deleted2\n";
    
    prim_key=NULL;
    mp.clear();
}
multinomial::~multinomial(){
    //all polynomials are being deleted by us::
    deleted_polys+=1;
}
uint32_t multinomial::get_degree(bool count_template){
    uint32_t ans=0;
    if(prim_key==NULL){
        //do nothing here::
    }
    else{
        if(!count_template && prim_key->is_template()){
           return 0;
        }
        for(auto &a:mp){
           ans=std::max(ans,(uint32_t)a.first+((multinomial*)a.second)->get_degree());
        }
    }
    return ans;
}

void multinomial::set_save(bool asave){
    if(save==asave){
        return;//do nothing here::
    }
    save=asave;
    if(prim_key!=NULL){
        save=asave;
        for(auto &a:mp){
            ((multinomial*)a.second)->set_save(asave);
        }
    }
}

void print_vector(vec_str arr){
    for(int i=0;i<arr.sz;i++){
        std::string a=arr[i];
        if(a=="$"){
            break;
        }
        std::cout<<a<<" + ";
    }
    std::cout<<"\n";
    //std::cout<<std::endl;

}


std::vector<multinomial*> multinomial::reduce(){
    //reduces gives vector of multinomials that need to be zeri::
    std::vector<multinomial*> ans;
    if(prim_key==NULL){
        ans.push_back(this);
    }
    else{
        if(prim_key->is_template()){
            ans.push_back(this);
        }
        else{
            for(auto& a: mp){
                for(auto& sub_mult: ((multinomial*)a.second)->reduce()){
                    ans.push_back(sub_mult);
                }
            }
        }
    }
    return ans;
}

bool multinomial::is_proper(uint32_t id){
    //reduces gives vector of multinomials that need to be zeri::
    bool ans=true;
    
    if(prim_key==NULL){
        return true;
    }
    else{
        if(id==0){
             id=prim_key->get_id();
        }
        if(prim_key->get_id()<id){
            ans=false;
        }
        else{
            for(auto& a: mp){
                ans=ans && ((multinomial*)a.second)->is_proper(prim_key->get_id());
                if(!ans){
                    break;
                }
               
            }
        }
    }
    return ans;
}

std::vector<int> multinomial::ret_id_arr(){
    //reduces gives vector of multinomials that need to be zeri::
    std::vector<int> ans;
    if(prim_key==NULL){
        //do nothing here::
    }
    else{
        ans.push_back(prim_key->get_id());
        for(auto& a: mp){
            for(auto& sub_mult: ((multinomial*)a.second)->ret_id_arr()){
                ans.push_back(sub_mult);
            }
        }
        
    }
    return ans;
}


bool multinomial::cmp_poly(std::vector<int> &v){
    if(prim_key==NULL){
        //do nothing here::
        return false;
    }
    else{
        for(auto &v_id:v){
            if(v_id==prim_key->get_id()){
                return true;
            }
        }
        for(auto &a: mp){
            if(((multinomial*)a.second)->cmp_poly(v)){
                return true;
            }
        }
        
        
    }
    return false;
}

double multinomial::substitute(std::vector<double> &vals){
    double ans=0;
    if(prim_key==NULL){
        //do nothing here::
        return *((double*)mp[0]);
    }
    else{
        
        for(auto &a:mp){
           
           double mul_val=pow(vals[prim_key->get_id()],a.first);
           
           double temp=((multinomial*)(a.second))->substitute(vals);
           
           ans+=(mul_val*temp);
        }
        
        
        
    }
    return ans;
}