#ifndef OPTIONS_H
#define OPTIONS_H

class Options: public Parametrized {
protected:
    // global options
    static unsigned short int FRAME_RATE;

public:
    Options();
    virtual void default_options();

    int get_frame_rate();
    int get_key_binding(int);
};

#endif // OPTIONS_H
