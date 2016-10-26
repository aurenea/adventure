#include <string>
#include <vector>

class Script {
private:
    Script* chain;
public:
    Script(Script* s) : chain(s) {}

    virtual void execute();
};

class BooleanScript: public Script {
public:
    BooleanScript() : Script(NULL) {}

    virtual bool evaluate() = 0;
};

class NegationScript: public BooleanScript {
private:
    BooleanScript* script;
public:
    NegationScript(BooleanScript* s) : script(s) {}

    bool evaluate() { return !script->evaluate(); }
};

class UnionScript: public BooleanScript {
private:
    BooleanScript* script1;
    BooleanScript* script2;
public:
    UnionScript(BooleanScript* s1, BooleanScript* s2) : script1(s1), script2(s2) {}

    bool evaluate() { return (script1->evaluate() || script2->evaluate()); }
};

class IntersectScript: public BooleanScript {
private:
    BooleanScript* script1;
    BooleanScript* script2;
public:
    IntersectScript(BooleanScript* s1, BooleanScript* s2) : script1(s1), script2(s2) {}

    bool evaluate() { return (script1->evaluate() && script2->evaluate()); }
};

class ConditionalScript: public Script {
private:
    BooleanScript* evaluation;
    Script* t_chain;
    Script* f_chain;
public:
    ConditionalScript(Script*, BooleanScript*, Script*, Script*);

    void execute();
};

class LoopScript: public Script {
private:
    Script* loop;
public:
    LoopScript(Script*, Script*);

    virtual void execute();
};

class RangeScript: public LoopScript {
private:
    std::string key;
    int first;
    int last;
public:
    RangeScript(Script*, Script*, std::string, int, int);

    void execute();
};
