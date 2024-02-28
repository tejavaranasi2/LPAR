#ifndef SYMBOL_H
#define SYMBOL_H
#include <inttypes.h>
#include <string>
#define SYM_LIMIT (UINT32_MAX) //limit on total symbols::


extern uint32_t NUM_SYMBOLS;//variable for number of symbols::
class multinomial;
class symbol{
    uint32_t id;
    bool is_templ;
    
    public:
    
    uint32_t get_id(){
        return id;
    }
    ~symbol(){
        NUM_SYMBOLS-=1;//we want to keep track of total symbols::
    }
    symbol(bool ais_template);
    bool is_template(){
        return is_templ;//returns if the symbol is a template or not::
    }
    bool operator==(symbol& s);
    bool operator>(symbol& s);
    bool operator<(symbol& s);
    
   
   

};
#endif