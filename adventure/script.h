#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <unordered_map>
#include "module.h"

enum class ScriptTrigger : int {
    INIT = 0,
    ON_LEFT_CLICK = 1,
    ON_RIGHT_CLICK = 2,
    ON_DRAG_RELEASE = 3
};

enum class ScriptType : unsigned int {
    END = 0,            // END of script
    // OBJECTS
    VALUE = 12,         // get constant value
        // +1   *value
    VARIABLE = 13,      // get value of variable
        // +1   key
    DECLARE = 14,       // declare a variable
        // +1   key
        // +2   *valuation
    ASSIGN = 15,        // assign value to variable
        // +1   key
        // +2   *valuation
    // CONTROL FLOW
    IF = 1,             // IF control flow
        // +1   *evaluation
        // +2   *true subscript
        // +3   *false subscript
        // +4   *next subscript
    WHILE = 10,         // WHILE control flow
        // +1   *evaluation
        // +2   *loop subscript
        // +3   *next subscript
    FOR = 11,           // FOR control flow
        // +1   declare key
        // +2   *declare valuation
        // +3   *evaluation
        // +4   *update subscript
        // +5   *loop subscript
        // +6   *next subscript
    // EVALUATIONS
    AND = 2,            // & bit operator
        // +1   *first valuation
        // +2   *second valuation
    OR = 3,             // | bit operator
        // +1   *first valuation
        // +2   *second valuation
    NOT = 4,            // ~ bit operator
        // +1   *valuation
    EQUAL = 5,          // == boolean operator
        // +1   *first valuation
        // +2   *second valuation
    GREATER_THAN = 6,   // > boolean operator
        // +1   *first valuation
        // +2   *second valuation
    GREATER_EQUAL = 7,  // >= boolean operator
        // +1   *first valuation
        // +2   *second valuation
    LESS_THAN = 8,      // < boolean operator
        // +1   *first valuation
        // +2   *second valuation
    LESS_EQUAL = 9,     // <= boolean operator
        // +1   *first valuation
        // +2   *second valuation
    // VALUATIONS
    CREATE = 16,        // create new Form
        // +1   key to FormData
    ADD = 17            // add Form to Lord
        // +1   (WHERE TO ADD)
        // +2   *valuation
};

struct ScriptVariable {
    unsigned int scope;
    unsigned int key;
    void* value;
};

class Script {
private:
    Module* mod;

    std::vector<unsigned int> chain;

    std::vector<void*> consts;
    std::vector<ScriptVariable> vars;
    unsigned int scope;

    //void* var_get(unsigned int);
    //void* var_set(unsigned int, void*);

    //void* execute(unsigned int);
    //void decrement_scope();
public:
    Script();

    //void set_value(unsigned int, unsigned int);
    //unsigned int append(ScriptType t) { return append((unsigned int)t); }
    //unsigned int append(unsigned int);
    //unsigned int last_index() { return (unsigned int)chain.size(); }

    void execute();
};

#endif // SCRIPT_H
