#include "symbol.hpp"

uint32_t NUM_SYMBOLS=0;



bool symbol::operator==(symbol& s){
    //overloading the equality operator
        return (id==s.id);
}
bool symbol::operator>(symbol& s){
    //overload > and return true if our symbol has more preference::
    return (id>s.id);
}
bool symbol::operator<(symbol& s){
    //overload > and return true if our symbol has more preference::
    return (id<s.id);
}


symbol::symbol(bool ais_template){
        if(NUM_SYMBOLS>=SYM_LIMIT){
            throw "[ERROR](symbol.h) Maximum symbol limit reached";
        }
       is_templ=ais_template;
       id=NUM_SYMBOLS;
       NUM_SYMBOLS+=1;
}