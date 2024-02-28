#include "approximation.hpp"


// Function to log a message to a file
std::string to_string_smt_(double x){
        if(x>=0){
            return std::to_string((int)x);
        }
        else{
            return "( - "+std::to_string((int)(-1*x))+" )";
        }
}

void log_(std::ofstream& logFile, const std::string& message) {
    // Get current timestamp
    time_t rawTime;
    struct tm* timeInfo;
    char buffer[80];
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    std::string timestamp(buffer);

    // Write log entry to the file
    logFile << "[" << timestamp << "] " << message;
}

void fragmentation_intersection::run_iters(int iter_limit,double vol_limit,std::string FileName,trie_node* trie){
    //direclty runs the algorithm on this ll::
    int fnd=0;
    std::vector<std::pair<double,double>> bounds;
    if(frags->HEAD!=NULL){
        bounds= frags->HEAD->box().coords_();
    }
     for(int itr=0;itr<iter_limit;++itr){
        std::string to_log="";
        to_log+=("---->itr: "+std::to_string(itr)+"::::: found: "+std::to_string(fnd)+"::::::::");
        
        if(curr_box==NULL){
            break;
        }
        
        if(trie){
            std::vector<int> path= trie_node::get_path(curr_box->box().coords_(),bounds,trie->n_dir_());
            if(!trie->find(path,0)){
              curr_box=frags->remove(curr_box);
              
              log_(LOG_FILE,"Not in original boxes\n");
              continue;
            }
        }
        to_log+=curr_box->box().print_dirs(1);
        log_(LOG_FILE,to_log);
        int found=2;
        //2-> this box is in the intersection::
        //1-> cannot discard this box::
        //0-> discard this box::
        double mx_len=-1;
        int idx=-1;
        
        try{
            for(int idx_solver=0;idx_solver<s_pos.size();++idx_solver){
                if(curr_box->box().curr_state_vec_()[idx_solver]>0){
                    continue;
                }
                if(curr_box->box().substitute(s_pos[idx_solver],id_map,3)){
                    curr_box->box().curr_state_vec_()[idx_solver]=1;
                }
                else if(curr_box->box().substitute(s_neg[idx_solver],id_map,3)){
                    curr_box->box().curr_state_vec_()[idx_solver]=-1;
                    curr_box=frags->remove(curr_box);
                    found=0;
                    break;
                }
                else{
                    //incolusive::
                    if(found==2){
                        found=1;
                    }
                }
            }
        }
        catch(const std::string& errorMessage){
            if(curr_box->box().norm()<vol_limit){
                curr_box->box().recompute_confidence();
                curr_box=curr_box->next;
                fnd++;
            }
            else{
                for(int dir=0;dir<n_dir;++dir){
                
                    if(mx_len<curr_box->box().len(dir)){
                        idx=dir;
                        mx_len=curr_box->box().len(dir);
                    }
                }
                            
                bounding_box *b= &curr_box->box().divide(idx,0,false);
                b->init_curr_state_vec(curr_box->box().curr_state_vec_());
                curr_box=frags->emplace_node(curr_box);
                frags->add_node(*b);

            }
            std::cout<<errorMessage<<"\n";

            std::string filename="../tests/"+FileName;
            std::ofstream file(filename);

            // Check if the file is opened successfully
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                exit(0);
            }

            // Close the file, which will truncate it and remove its contents
            file.close();
            frags->dump_trace(filename,curr_box);
            LOG_FILE.close();
            exit(0);
        }

        if(found==2){
            curr_box->box().confidence_()=1;
            curr_box=curr_box->next;
            fnd++;
        }
        else if(found==1){
            //now split::
            if(curr_box->box().norm()<vol_limit){
                curr_box->box().recompute_confidence();
                curr_box=curr_box->next;
                fnd++;
            }
            else{
                for(int dir=0;dir<n_dir;++dir){
                
                    if(mx_len<curr_box->box().len(dir)){
                        idx=dir;
                        mx_len=curr_box->box().len(dir);
                    }
                }
                            
                bounding_box *b= &curr_box->box().divide(idx,0,false);
                b->init_curr_state_vec(curr_box->box().curr_state_vec_());
                curr_box=frags->emplace_node(curr_box);
                frags->add_node(*b);
            }
        }
        

        if(curr_box==NULL){
            break;
        }
     }

     frags->recompute_confidence();
     //look for 0 confidence boxes now::
     curr_box=NULL;
     
    //we will make smt calls here::
     
     //all iters are run::  go until tail and drop ones that are not needed::
    //  while(curr_box!=NULL){
    //     if(curr_box->box().substitute(s,id_map)){
    //         std::cout<<"fnd::"<<++fnd<<"\n";
    //         curr_box=curr_box->next;
    //     }
    //     else if(curr_box->box().substitute(s_neg,id_map)){
    //         curr_box=frags->remove(curr_box);
    //     }
    //     else{
    //         std::cout<<"fnd::"<<++fnd<<"\n";
    //         curr_box=curr_box->next;
    //     }
    //  }
}



void fragmentation_intersection::run_threads(int iter_limit,double vol_limit,std::string FileName,trie_node* trie,int n_threads,bool int_solve){
    std::vector<std::pair<double,double>> bounds;
    if(frags->HEAD!=NULL){
        bounds= frags->HEAD->box().coords_();
    }

    std::vector<std::thread> threads;
    n_threads_active=n_threads;
    for(int i=0;i<n_threads;++i){
        if(!int_solve){
         threads.emplace_back(std::bind(&fragmentation_intersection::run_thread, this, bounds,iter_limit,vol_limit,FileName,trie));
        }
        else{
         threads.emplace_back(std::bind(&fragmentation_intersection::run_thread_int, this, bounds,iter_limit,vol_limit,FileName,trie));
        }
    }
    
    for (std::thread& thread : threads) {
        thread.join();
    }

  

    //frags->recompute_confidence();


    //curr_box=NULL;

}
void fragmentation_intersection::run_thread(std::vector<std::pair<double,double>> &bounds,int iter_limit,double vol_limit,std::string FileName,trie_node* trie){
      
      
      while(true){
          std::string to_log="";
          boost::unique_lock<boost::mutex> lock(data_mutex);
          while(curr_box==NULL && n_threads_active>0){
             n_threads_active-=1;
             if(n_threads_active==0){
                break;
             }
             wait_queue.wait(lock);

             n_threads_active+=1;//this thread got the lock smh::
          }



          if(curr_box==NULL || n_threads_active==0){
             wait_queue.notify_all();
             data_mutex.unlock();
             
             break;
          }

          ll_node* to_p=curr_box;
          to_p->box().curr_state_()=1;//we are processing this node::
          curr_box=curr_box->next;
          data_mutex.unlock();

          //now process to_p;

          

        if(trie){
            std::vector<int> path= trie_node::get_path(to_p->box().coords_(),bounds,trie->n_dir_());
            if(!trie->find(path,0)){
              frags->remove(to_p);

              //if curr_box is null, we need to set the new curr_box when we add one::
              
              log_(LOG_FILE,"Not in original boxes\n");
              continue;
            }
        }
        to_log+=to_p->box().print_dirs(1);
        log_(LOG_FILE,to_log);
        int found=2;
        //2-> this box is in the intersection::
        //1-> cannot discard this box::
        //0-> discard this box::
        double mx_len=-1;
        int idx=-1;
        
        try{
            for(int idx_solver=0;idx_solver<s_pos.size();++idx_solver){
                if(to_p->box().curr_state_vec_()[idx_solver]>0){
                    continue;
                }
                if(to_p->box().substitute(s_pos[idx_solver],id_map,0)){
                    to_p->box().curr_state_vec_()[idx_solver]=1;
                }
                else if(to_p->box().substitute(s_neg[idx_solver],id_map,0)){
                    to_p->box().curr_state_vec_()[idx_solver]=-1;
                    frags->remove(to_p);
                    found=0;
                    break;
                }
                else{
                    //incolusive::
                    if(found==2){
                        found=1;
                    }
                }
            }
        }
        catch(const std::string& errorMessage){
            if(to_p->box().norm()<vol_limit){
                to_p->box().recompute_confidence();
            }
            else{
                for(int dir=0;dir<n_dir;++dir){
                
                    if(mx_len<to_p->box().len(dir)){
                        idx=dir;
                        mx_len=to_p->box().len(dir);
                    }
                }
                            
                bounding_box *b= &to_p->box().divide(idx,0,false);
                b->init_curr_state_vec(to_p->box().curr_state_vec_());

                frags->emplace_node(to_p);
                frags->add_node(*b);

            }
            std::cout<<errorMessage<<"\n";

            std::string filename="../tests/"+FileName;
            std::ofstream file(filename);

            // Check if the file is opened successfully
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                exit(0);
            }

            // Close the file, which will truncate it and remove its contents
            file.close();
            frags->dump_trace(filename,curr_box);
            LOG_FILE.close();
            exit(0);
        }

        if(found==2){
            to_p->box().confidence_()=1;
        }
        else if(found==1){
            //now split::
            if(to_p->box().norm()<vol_limit){
                to_p->box().recompute_confidence();
            }
            else{
                for(int dir=0;dir<n_dir;++dir){
                
                    if(mx_len<to_p->box().len(dir)){
                        idx=dir;
                        mx_len=to_p->box().len(dir);
                    }
                }
                            
                bounding_box *b= &to_p->box().divide(idx,0,false);
                b->init_curr_state_vec(to_p->box().curr_state_vec_());

                data_mutex.lock();
                ll_node* new_node=frags->emplace_node(to_p);
                if(curr_box==NULL){
                    
                    curr_box=to_p;//this node still needs processing::
                    
                    wait_queue.notify_all();
                }
                frags->add_node(*b);
                data_mutex.unlock();
                
            }
        }

        to_p->box().curr_state_()=2;


      }



}

void fragmentation_intersection::run_thread_int(std::vector<std::pair<double,double>> &bounds,int iter_limit,double vol_limit,std::string FileName,trie_node* trie){
      
      
      while(true){
          std::string to_log="";
          boost::unique_lock<boost::mutex> lock(data_mutex);
          while(curr_box==NULL && n_threads_active>0){
             n_threads_active-=1;
             if(n_threads_active==0){
                break;
             }
             wait_queue.wait(lock);

             n_threads_active+=1;//this thread got the lock smh::
          }



          if(curr_box==NULL || n_threads_active==0){
             wait_queue.notify_all();
             lock.unlock();
             
             break;
          }

          ll_node* to_p=curr_box;
          to_p->box().curr_state_()=1;//we are processing this node::
          curr_box=curr_box->next;
          lock.unlock();

          //now process to_p;

          

        if(trie){
            std::vector<int> path= trie_node::get_path(to_p->box().coords_(),bounds,trie->n_dir_());
            if(!trie->find(path,0)){
              frags->remove(to_p);

              //if curr_box is null, we need to set the new curr_box when we add one::
              
              log_(LOG_FILE,"Not in original boxes\n");
              continue;
            }
        }

        if(to_p->box().not_valid()){
            frags->remove(to_p);
            continue;
        }
        to_log+=to_p->box().print_dirs(1);
        log_(LOG_FILE,to_log);
        int found=2;
        //2-> this box is in the intersection::
        //1-> cannot discard this box::
        //0-> discard this box::
        double mx_len=-1;
        int idx=-1;
        
        try{
            for(int idx_solver=0;idx_solver<s_pos.size();++idx_solver){
                if(to_p->box().curr_state_vec_()[idx_solver]>0){
                    continue;
                }
                if(to_p->box().substitute(s_pos[idx_solver],id_map,0)){
                    to_p->box().curr_state_vec_()[idx_solver]=1;
                }
                else if(to_p->box().substitute(s_neg[idx_solver],id_map,0)){
                    to_p->box().curr_state_vec_()[idx_solver]=-1;
                    frags->remove(to_p);
                    found=0;
                    break;
                }
                else{
                    //incolusive::
                    if(found==2){
                        found=1;
                    }
                }
            }
        }
        catch(const std::string& errorMessage){
            if(to_p->box().norm()<vol_limit){
                to_p->box().recompute_confidence();
            }
            else{
                for(int dir=0;dir<n_dir;++dir){
                
                    if(mx_len<to_p->box().len(dir)){
                        idx=dir;
                        mx_len=to_p->box().len(dir);
                    }
                }
                            
                bounding_box *b= &to_p->box().divide(idx,0,false);
                b->init_curr_state_vec(to_p->box().curr_state_vec_());
                 
                to_p->box().round(1);
                b->round(1);

                frags->emplace_node(to_p);
                frags->add_node(*b);

            }
            std::cout<<errorMessage<<"\n";

            std::string filename="../tests/"+FileName;
            std::ofstream file(filename);

            // Check if the file is opened successfully
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                exit(0);
            }

            // Close the file, which will truncate it and remove its contents
            file.close();
            frags->dump_trace(filename,curr_box);
            LOG_FILE.close();
            exit(0);
        }

        if(found==2){
            to_p->box().confidence_()=1;
        }
        else if(found==1){
            //now split::
            if(to_p->box().norm()<vol_limit){
                to_p->box().recompute_confidence();
            }
            else{
                for(int dir=0;dir<n_dir;++dir){
                
                    if(mx_len<to_p->box().len(dir)){
                        idx=dir;
                        mx_len=to_p->box().len(dir);
                    }
                }
                            
                bounding_box *b= &to_p->box().divide(idx,0,false);
                b->init_curr_state_vec(to_p->box().curr_state_vec_());

                lock.lock();

                to_p->box().round(1);
                b->round(1);

                ll_node* new_node=frags->emplace_node(to_p);
                if(curr_box==NULL){
                    
                    curr_box=to_p;//this node still needs processing::
                    
                    wait_queue.notify_all();
                }
                frags->add_node(*b);
                lock.unlock();
                
            }
        }

        to_p->box().curr_state_()=2;


      }



}

void fragmentation::run_iters(int iter_limit,double vol_limit){
     int fnd=0;
     for(int itr=0;itr<iter_limit;++itr){
        std::cout<<"---->itr:"<<itr<<"::::: found:"<<fnd<<"::::::::";
        if(curr_box==NULL){
            break;
        }
        curr_box->box().print_dirs();
        bool found=false;
        double mx_len=-1;
        int idx=-1;
        if(curr_box->box().substitute(s,id_map)){
            curr_box=curr_box->next;
            curr_box->box().confidence_()=1;
            fnd++;
            found=true;
        }
        else if(curr_box->box().substitute(s_neg,id_map)){
            curr_box=frags->remove(curr_box);
        }
        else{
            //now split::
            if(curr_box->box().norm()<vol_limit){
                curr_box=curr_box->next;
                fnd++;
                continue;
            }
            for(int dir=0;dir<n_dir;++dir){
            // bounding_box *b1=&curr_box->box().divide(dir,0);
            // bounding_box *b2=&curr_box->box().divide(dir,1);
            // if((*b1).substitute(s,id_map)){
            //     fnd++;
            //     idx=dir;
            //     //frags.push_back(b2);

            //     frags->add_node(*b2);

            //     curr_box->box().modify(dir,0);
            //     curr_box=curr_box->next;
            //     found=true;
            //     break;
            // }
            // else if((*b2).substitute(s,id_map)){
            //     fnd++;
            //     idx=dir;
                
            //     frags->add_node(*b1);

            //     curr_box->box().modify(dir,1);
            //     curr_box=curr_box->next;
            //     found=true;

            //     break;
            // }
                if(mx_len<curr_box->box().len(dir)){
                    idx=dir;
                    mx_len=curr_box->box().len(dir);
                }
            }
            

            // if(!found && idx>=0){
            //     bounding_box *b= &curr_box->box().divide(idx,0,false);
            //     curr_box=frags->emplace_node(curr_box);
            //     frags->add_node(*b);
            // }
            
            bounding_box *b= &curr_box->box().divide(idx,0,false);
            curr_box=frags->emplace_node(curr_box);
            frags->add_node(*b);
            
        }

        if(curr_box==NULL){
            break;
        }
     }
     //all iters are run::  go until tail and drop ones that are not needed::
     while(curr_box!=NULL){
        if(curr_box->box().substitute(s,id_map)){
            std::cout<<"fnd::"<<++fnd<<"\n";
            curr_box=curr_box->next;
        }
        else if(curr_box->box().substitute(s_neg,id_map)){
            curr_box=frags->remove(curr_box);
        }
        else{
            std::cout<<"fnd::"<<++fnd<<"\n";
            curr_box=curr_box->next;
        }
     }

    
     //frags->print();
}

void fragmentation::plot(){

}

ll* operation(ll* l1,ll*l2,int type){
    ll* ans=new ll();
    ll_node* curr1=l1->HEAD;

    while(curr1!=NULL){
        ll_node* curr2=l2->HEAD;
        while(curr2!=NULL){
            bounding_box* ans_=curr1->box().intersect(curr2->box());
            if(ans_){
                if(curr1->box().confidence_()==0 && curr2->box().confidence_()==0){
                  ans_->confidence_()=0;
                }
                else{
                  ans_->confidence_()=1;
                }
                ans->add_node(*ans_);
            }
            curr2=curr2->next;
        }
        curr1=curr1->next;
    }

    return ans;

}

bounding_box& bounding_box::divide(int dir,int type,bool retain){
    //type=0 is left half and type==1 is right
    std::vector<std::pair<double,double> > c(coords);
    double temp=(c[dir].second+c[dir].first)*0.5;
    if(type==0){
        
        c[dir].second=temp;
        if(!retain){
            coords[dir].first=temp;
        }

    }
    else{
        c[dir].first=temp;
        if(!retain){
            coords[dir].second=temp;
        }
    }
    bounding_box* n_box= new bounding_box(c);

    return *n_box;
}

void bounding_box::modify(int dir,int type){
    double temp=(coords[dir].second+coords[dir].first)*0.5;

    if(type==0){
        coords[dir].second=temp;

    }
    else{
        
        coords[dir].first=temp;
        
    }
}


bool bounding_box::substitute(solver *s,std::map<std::pair<int,int>, int> &id_map,int type_){
  //substitute--> and check if it can be satisfied::
  std::map<int,double> m;
  for(int i=0;i<coords.size();++i){
    int id_1=id_map[std::make_pair(i,0)];
    int id_2=id_map[std::make_pair(i,1)];

    m[id_1]=coords[i].first;
    m[id_2]=coords[i].second;


  }
  if(type_==0){
     return s->solve(0,m,"GRB");
  }
  else if(type_==1){
     return s->solve(0,m,"z3");
  }
  else{
     //erase the contents first::
     //type_==3
     s->empty_constraints();
     vars* v= s->v;
     for(auto& elem: m){
         s->add((*v)[elem.first]+(-1*elem.second));
     }
      return s->solve(0,m,"z3");
  }
  


}

bool bounding_box::make_smt_query(solver* s,std::vector<handelman::expr>e_check){
  return s->smt_query(e_check,coords);
}



void color_code(ll* list,std::vector<solver*> &eq_pos,std::vector<solver*> &eq_neg,std::map<std::pair<int,int>, int> &id_map){
     ll_node* curr=list->HEAD;
     while(curr!=NULL){
         int fail=0;
         for(int i=0;i<eq_pos.size();i++){
            if(!curr->box().substitute(eq_pos[i],id_map)){
                if(curr->box().substitute(eq_neg[i],id_map)){
                    curr->box().confidence_()=-1;
                    break;
                }
                fail+=1;
            }

        

            if(fail>1){
                curr->box().confidence_()=0;
                break;
            }
         }
         if(fail<=1 && curr->box().confidence_()>=0){
           curr->box().confidence_()=1;
         }

         curr=curr->next;
     }
}
