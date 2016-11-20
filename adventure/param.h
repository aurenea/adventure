#ifndef PARAM_H
#define PARAM_H

#include <memory>
#include <vector>
#include <unordered_map>


enum class ParamSpecific: unsigned int {
    THIS = 0,
    INHERIT = 1
};


struct Param {
    virtual ~Param() {}
};

template <class T>
struct TypedParam: public Param {
    T data;
    TypedParam(T t) { data = t; }
};


class Parametrized {
private:
    Parametrized* inherit;
    std::unordered_map<unsigned int, std::shared_ptr<Param> > params;
public:
    Parametrized();
    virtual void add_param(unsigned int, std::shared_ptr<Param>);
    template <class T> void set_param(unsigned int, T);
    template <class T> T get_param(unsigned int);
    bool check_param(unsigned int);
};

class Article: public Parametrized {
public:
    Article() {}
    Article(Parametrized*);
    template <class T> T get_param_chain(std::vector<unsigned int>*);
};

#endif // PARAM_H
