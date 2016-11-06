#ifndef DATA_H
#define DATA_H

#include <unordered_map>
#include <string>
//#include "form.h"

/*
enum class UnderlyingClassData {
    MENU,
    WIDGET,
    ITEM,
    CONTAINER,
    ENTITY,
    ATTIRED
};
*/

class Parametrized {
private:
    Parametrized* inherit;
    std::unordered_map<std::string, void*> params;
public:
    Parametrized(Parametrized* p) : inherit(p) {}
    void add_param(std::string, void*);
    void set_param(std::string, void*);
    virtual void* get_param(std::string);
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
    Article(Parametrized* p) : Parametrized(p) {}
    void* get_param(std::string);
};

#endif // DATA_H
