#include "script.h"

void Script::execute() {
    scope = 0;
    vars.clear();
    // try catch block
    //// execute(0);
    // end try catch
}
/*
void* Script::execute(unsigned int index) {
    while (chain[index] != (unsigned int)ScriptType::END) {
        switch ((ScriptType)chain[index]) {
            case ScriptType::DECLARE:
                vars.push_back({ scope, chain[index+1], valuate(chain[index+2]) });
                index += 3;
                break;
            case ScriptType::ASSIGN:
                set_var(chain[index+1], valuate(chain[index+2]));
                index += 3;
                break;
            case ScriptType::IF:
                if (evaluate(chain[index+1])%2 > 0) {
                    execute(chain[index+2]);
                } else {
                    execute(chain[index+3]);
                }
                index = chain[index+4];
                break;
            case ScriptType::WHILE:
                scope++;
                while (evaluate(chain[index+1])%2 > 0) {
                    execute(chain[index+2]);
                }
                decrement_scope();
                index = chain[index+3];
                break;
            case ScriptType::FOR:
                scope++;
                execute(chain[index+1]);
                while (evaluate(chain[index+2])%2 > 0) {
                    execute(chain[index+4]);
                    execute(chain[index+3]);
                }
                decrement_scope();
                index = chain[index+5];
                break;
            case ScriptType::ADD:
                break;
        }
    }
}
void* Script::valuate(unsigned int index) {
    switch ((ScriptType)chain[index]) {
        case ScriptType::VALUE:
            return consts[chain(index+1)];
        case ScriptType::VARIABLE:
            return var_get(chain[index+1]);
        case ScriptType::DECLARE:
            void* value = valuate(chain[index+2]);
            vars.push_back({ scope, chain[index+1], value });
            return value;
        case ScriptType::ASSIGN:
            return set_var(chain[index+1], valuate(chain[index+2]));
        case ScriptType::CREATE:
            return mod->create(chain[index+1]);
        case ScriptType::ADD:
            Form* value = dynamic_cast<Form*>(valuate(chain[index+2]));
            if (value != NULL) {
            }
            return value;
        default:
            return NULL;
    }
}
int Script::evaluate(unsigned int index) {
    switch ((ScriptType)chain[index]) {
        case ScriptType::VALUE:
            return (int)chain[index+1];
        case ScriptType::VARIABLE:
            void* value = var_get(chain[index+1]);
            if (int i = static_cast<int>(value)) {
                return i;
            } else {
                return (int)(value != NULL);
            }
        case ScriptType::DECLARE:
        case ScriptType::ASSIGN:
            void* value = valuate(index);
            return (int)(value != NULL);
        case ScriptType::AND:
            return (evaluate(chain[index+1]) & evaluate(chain[index+2]));
        case ScriptType::OR:
            return (evaluate(chain[index+1]) | evaluate(chain[index+2]));
        case ScriptType::NOT:
            return ~evaluate(chain[index+1]);
        case ScriptType::EQUAL:
            return (int)(evaluate(chain[index+1]) == evaluate(chain[index+2]));
        case ScriptType::GREATER_THAN:
            return (int)(evaluate(chain[index+1]) > evaluate(chain[index+2]));
        case ScriptType::GREATER_EQUAL:
            return (int)(evaluate(chain[index+1]) >= evaluate(chain[index+2]));
        case ScriptType::LESS_THAN:
            return (int)(evaluate(chain[index+1]) < evaluate(chain[index+2]));
        case ScriptType::LESS_EQUAL:
            return (int)(evaluate(chain[index+1]) <= evaluate(chain[index+2]));
        default:
            return 0;
    }
}
unsigned int Script::execute(unsigned int index) {
    while (chain[index] != (unsigned int)ScriptType::END) {
        if (chain[index] >= (unsigned int)ScriptType::MAX) {
            return var_get(chain[index]-(unsigned int)ScriptType::MAX);
        } else {
            switch ((ScriptType)chain[index]) {
                case ScriptType::IF:
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
                case ScriptType::AND:
                    return (unsigned int)(execute(chain[index+1]) > 0 && execute(chain[index+2]) > 0);
                case ScriptType::OR:
                    return (unsigned int)(execute(chain[index+1]) > 0 || execute(chain[index+2]) > 0);
                case ScriptType::NOT:
                    return (unsigned int)(execute(chain[index+1]) == 0);
                case ScriptType::EQUAL:
                    return (unsigned int)(execute(chain[index+1]) == execute(chain[index+2]));
                case ScriptType::GREATER_THAN:
                    return (unsigned int)(execute(chain[index+1]) > execute(chain[index+2]));
                case ScriptType::GREATER_EQUAL:
                    return (unsigned int)(execute(chain[index+1]) >= execute(chain[index+2]));
                case ScriptType::LESS_THAN:
                    return (unsigned int)(execute(chain[index+1]) < execute(chain[index+2]));
                case ScriptType::LESS_EQUAL:
                    return (unsigned int)(execute(chain[index+1]) <= execute(chain[index+2]));
                case ScriptType::WHILE:
                    scope++;
                    while (execute(chain[index+1]) > 0) {
                        execute(chain[index+2]);
                    }
                    decrement_scope();
                    index = chain[index+3];
                    break;
                case ScriptType::FOR:
                    scope++;
                    execute(chain[index+1]);
                    while (execute(chain[index+2]) > 0) {
                        execute(chain[index+4]);
                        execute(chain[index+3]);
                    }
                    decrement_scope();
                    index = chain[index+5];
                    break;
                case ScriptType::VALUE:
                    return chain[index+1];
                case ScriptType::DECLARE:
                    vars.push_back({scope, chain[index+1]});
                    index += 2;
                    break;
                case ScriptType::ASSIGN:
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
void* Script::var_get(unsigned int key) {
    for (std::vector<ScriptVariable>::reverse_iterator iter = vars.rbegin(); iter < vars.rend(); ++iter) {
        if (key == (*iter).key) {
            return (*iter).value;
        }
    }
    throw std::string("Tried to access undefined variable ").append(key).append(".");
}
void* Script::var_set(unsigned int key, void* value) {
    for (std::vector<ScriptVariable>::reverse_iterator iter = vars.rbegin(); iter < vars.rend(); ++iter) {
        if (key == (*iter).key) {
            (*iter).value = value;
            return value;
        }
    }
    throw std::string("Tried to access undefined variable ").append(key).append(".");
}
void Script::set_value(unsigned int index, unsigned int value) {
    chain[index] = value;
}
unsigned int Script::append(unsigned int value) {
    chain.push_back(value);
    return last_index()-1;
}
*/
