#include "script.h"

void Script::execute() {
    scope = 0;
    vars.clear();
    // try catch block
    execute(0);
    // end try catch
}

unsigned int Script::execute(unsigned int index) {
    while (chain[index] != (unsigned int)ScriptTypes::END) {
        if (chain[index] >= (unsigned int)ScriptTypes::MAX) {
            return var_get(chain[index]-(unsigned int)ScriptTypes::MAX);
        } else {
            switch ((ScriptTypes)chain[index]) {
                case ScriptTypes::IF:
                    if (execute(chain[index+1]) > 0) {
                        scope++;
                        execute(chain[index+2]);
                    } else {
                        scope++;
                        execute(chain[index+3]);
                    }
                    decrement_scope();
                    index = chain[index+4];
                    break;
                case ScriptTypes::AND:
                    return (unsigned int)(execute(chain[index+1]) > 0 && execute(chain[index+2]) > 0);
                case ScriptTypes::OR:
                    return (unsigned int)(execute(chain[index+1]) > 0 || execute(chain[index+2]) > 0);
                case ScriptTypes::NOT:
                    return (unsigned int)(execute(chain[index+1]) == 0);
                case ScriptTypes::EQUAL:
                    return (unsigned int)(execute(chain[index+1]) == execute(chain[index+2]));
                case ScriptTypes::GREATER_THAN:
                    return (unsigned int)(execute(chain[index+1]) > execute(chain[index+2]));
                case ScriptTypes::GREATER_EQUAL:
                    return (unsigned int)(execute(chain[index+1]) >= execute(chain[index+2]));
                case ScriptTypes::LESS_THAN:
                    return (unsigned int)(execute(chain[index+1]) < execute(chain[index+2]));
                case ScriptTypes::LESS_EQUAL:
                    return (unsigned int)(execute(chain[index+1]) <= execute(chain[index+2]));
                case ScriptTypes::WHILE:
                    scope++;
                    while (execute(chain[index+1]) > 0) {
                        execute(chain[index+2]);
                    }
                    decrement_scope();
                    index = chain[index+3];
                    break;
                case ScriptTypes::FOR:
                    scope++;
                    execute(chain[index+1]);
                    while (execute(chain[index+2]) > 0) {
                        execute(chain[index+4]);
                        execute(chain[index+3]);
                    }
                    decrement_scope();
                    index = chain[index+5];
                    break;
                case ScriptTypes::VALUE:
                    return chain[index+1];
                case ScriptTypes::DECLARE:
                    vars.push_back({scope, chain[index+1]});
                    index += 2;
                    break;
                case ScriptTypes::ASSIGN:
                    var_set(chain[index+1], execute(chain[index+2]));
                    index += 3;
                    break;
                default:
                    // uh idk something fucked up
                    break;
            }
        }
    }
    return 0;
}

void Script::decrement_scope() {
    scope--;
    while (vars.back().scope > scope) {
        vars.pop_back();
    }
}

unsigned int Script::var_get(unsigned int key) {
    for (std::vector<ScriptVariable>::reverse_iterator iter = vars.rbegin(); iter < vars.rend(); ++iter) {
        if (key == (*iter).key) {
            return (*iter).value;
        }
    }
    throw std::string("Tried to access undefined variable ").append(key).append(".");
}

void Script::var_set(unsigned int key, unsigned int value) {
    for (std::vector<ScriptVariable>::reverse_iterator iter = vars.rbegin(); iter < vars.rend(); ++iter) {
        if (key == (*iter).key) {
            (*iter).value = value;
            return;
        }
    }
    throw std::string("Tried to access undefined variable ").append(key).append(".");
}

void Script::set_value(int index, unsigned int value) {
    chain[index] = value;
}

int Script::append(unsigned int value) {
    chain.push_back(value);
    return chain.size()-1;
}
