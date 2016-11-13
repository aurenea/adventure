#ifndef DATA_H
#define DATA_H

#include <memory>
#include <unordered_map>
#include <vector>

struct Param {};

template <class T>
struct TypedParam: public Param {
    T data;
    TypedParam(T t) : data(t) {}
};


class Parametrized {
private:
    std::unordered_map< unsigned int, std::shared_ptr<Param> > params;
public:
    Parametrized();

    template <class T> void add_param(unsigned int, T);
    template <class T> void set_param(unsigned int, T);
    bool check_param(unsigned int);
    template <class T> T get_param(unsigned int);
};

/*
class ParamData {
private:
    ParamData* inherited;
    std::unordered_map<std::string, void*> params;
protected:
    UnderlyingClassData underlying;
public:
    void add_param(std::string, void*);
    void set_param(std::string, void*);
    void* get_param(std::string);
    virtual void* create();
};
*/

/*
class FormData: public TypeData {
public:
    void execute(std::string);
    Form* create();
};
*/

class Article: public Parametrized {
public:
    Article() {}
    Article(Parametrized*);

    template <class T> T get_param_chain(std::vector<unsigned int>*);
};

#endif // DATA_H
