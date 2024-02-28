#include "expr.hpp"
using namespace handelman;

multinomial& expr::operator=(expr& e){
   if(in_mem){
       m->custom_delete(true);
       delete m;
       //delete even if save if on::
   }
   m=new multinomial();
   in_mem=true;

   *m=*e.m;
   e.rem_mem_flag();//hanldes if e must be saved::

   return *m;
}
multinomial& expr::operator=(multinomial& e){
   if(in_mem){
       m->custom_delete(true);
       delete m;
       //delete even if save if on::
   }
   m=new multinomial();
   in_mem=true;

   *m=e;
   return *m;
}
multinomial& expr::operator=(const double& e){
   if(in_mem){
       m->custom_delete(true);
       delete m;
       //delete even if save if on::
   }
    m=new multinomial(e);
    in_mem=true;
    return *m;
}

multinomial& expr::operator+(expr& e){
   if(!m->save){
     in_mem=false;
   }
   if(!e.m->save){
      e.in_mem=false;
   }
   return (*m+*e.m);
}
multinomial& expr::operator+(multinomial& e){
   if(!m->save){
    in_mem=false;
   }
   return ((*m)+(e));
}
multinomial& expr::operator+(const double& e){
   if(!m->save){
    in_mem=false;
   }
   return ((*m)+(e));
}

multinomial& expr::operator*(expr& e){
   if(!m->save){
    in_mem=false;
   }
   if(!e.m->save){
      e.in_mem=false;
   }
   return ((*m)*(*e.m));
}
multinomial& expr::operator*(multinomial& e){
   if(!m->save){
    in_mem=false;
   }
   return ((*m)*(e));
}
multinomial& expr::operator*(const double& e){
   if(!m->save){
    in_mem=false;
   }
   return ((*m)*(e));
}

multinomial& expr::operator-(expr& e){
   if(!m->save){
     in_mem=false;
   }
   if(!e.m->save){
      e.in_mem=false;
   }
   return (*m+((*e.m)*-1));
}
multinomial& expr::operator-(multinomial& e){
   if(!m->save){
     in_mem=false;
   }
   return (*m+((e)*-1));
}
multinomial& expr::operator-(const double& e){
  if(!m->save){
     in_mem=false;
   }
   return (*m+((e)*-1));
}

multinomial& expr::operator^(const uint16_t& pow){
   if(!m->save){
    in_mem=false;
   }
   return ((*m)^(pow));
}