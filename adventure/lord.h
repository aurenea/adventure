#ifndef LORD_H
#define LORD_H

template <class T, class P>
class Lord {
protected:
    unsigned int capacity;
    unsigned int last;
    T* vassals;

public:
    Lord(int ic = 8) {
        capacity = ic;
        last = 0;
        vassals = new T[capacity];
    }

    virtual bool add(T) = 0;
    virtual bool add(T, P) = 0;
    virtual T discard(T) = 0;
    virtual T get(P) = 0;

};

#endif // LORD_H
