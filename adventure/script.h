#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include <vector>

enum class ScriptTypes : unsigned int {
    MAX = 15,
    END = 0,
    IF = 1,
    // +1   *condition
    // +2   *true script
    // +3   *false script
    // +4   *next script
    AND = 2,
    // +1   *first condition
    // +2   *second condition
    OR = 3,
    // +1   *first condition
    // +2   *second condition
    NOT = 4,
    // +1   *condition
    EQUAL = 5,
    // +1   *first value
    // +2   *second value
    GREATER_THAN = 6,
    // +1   *first value
    // +2   *second value
    GREATER_EQUAL = 7,
    // +1   *first value
    // +2   *second value
    LESS_THAN = 8,
    // +1   *first value
    // +2   *second value
    LESS_EQUAL = 9,
    // +1   *first value
    // +2   *second value
    WHILE = 10,
    // +1   *condition
    // +2   *loop script
    // +3   *next script
    FOR = 11,
    // +1   *declaration
    // +2   *condition
    // +3   *update
    // +4   *loop script
    // +5   *next script
    VALUE = 12,
    // +1   value
    DECLARE = 13,
    // +1   key
    // +2   next script
    ASSIGN = 14
    // +1   var to assign to
    // +2   value to assign
    // +3   next script
};

struct ScriptVariable {
    unsigned int scope;
    unsigned int key;
    unsigned int value;
};

class Script {
private:
    std::vector<unsigned int> chain;

    std::vector<ScriptVariable> vars;
    unsigned int scope;

    unsigned int var_get(unsigned int);
    void var_set(unsigned int, unsigned int);

    unsigned int execute(unsigned int);
    void decrement_scope();
public:
    Script();

    void set_value(int, unsigned int);
    int append(unsigned int);

    void execute();
};

#endif // SCRIPT_H
