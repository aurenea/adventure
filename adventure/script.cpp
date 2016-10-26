#include "script.h"

struct ScriptVariable {
    std::string key;
    int value;
};

class ScriptVariableStorage {
private:
    static std::vector<ScriptVariable> vars;

    ScriptVariableStorage() {}

public:
    static int get_value(std::string);
    static void set_value(std::string, int);
    static void increment(std::string);
    static void push(std::string, int);
    static void pop();
};

std::vector<ScriptVariable> ScriptVariableStorage::vars;

int ScriptVariableStorage::get_value(std::string key) {
    for (std::vector<ScriptVariable>::reverse_iterator iter = vars.rbegin(); iter != vars.rend(); ++iter) {
        if (iter->key.compare(key) == 0) {
            return iter->value;
        }
    }
    return -99999;
}

void ScriptVariableStorage::set_value(std::string key, int value) {
    for (std::vector<ScriptVariable>::reverse_iterator iter = vars.rbegin(); iter != vars.rend(); ++iter) {
        if (iter->key.compare(key) == 0) {
            iter->value = value;
        }
    }
}

void ScriptVariableStorage::increment(std::string key) {
    for (std::vector<ScriptVariable>::reverse_iterator iter = vars.rbegin(); iter != vars.rend(); ++iter) {
        if (iter->key.compare(key) == 0) {
            iter->value++;
        }
    }
}

void ScriptVariableStorage::push(std::string key, int value) {
    vars.push_back({key, value});
}

void ScriptVariableStorage::pop() {
    vars.pop_back();
}


void Script::execute() {
    chain->execute();
}

ConditionalScript::ConditionalScript(Script* c, BooleanScript* b, Script* t, Script* f) : Script(c) {
    evaluation = b;
    t_chain = t;
    f_chain = f;
}

void ConditionalScript::execute() {
    if (evaluation->evaluate()) {
        t_chain->execute();
    } else {
        f_chain->execute();
    }
    Script::execute();
}

LoopScript::LoopScript(Script* c, Script* l) : Script(c) {
    loop = l;
}

void LoopScript::execute() {
    loop->execute();
}

RangeScript::RangeScript(Script* c, Script* l, std::string k, int s, int e) : LoopScript(c, l) {
    key = k;
    first = s;
    last = e;
}

void RangeScript::execute() {
    ScriptVariableStorage::push(key, first);
    while (ScriptVariableStorage::get_value(key) < last) {
        LoopScript::execute();
        ScriptVariableStorage::increment(key);
    }
    ScriptVariableStorage::pop();
    Script::execute();
}
