#ifndef RUN_H
#define RUN_H

enum class RunResult : int {
    NEXT = 2,           // event not yet resolved, keep searching for resolution
    CONTINUE = 1,       // event resolved, move on
    STOP = 0,           // stop running program and exit
    ERROR = -1          // error raised, shut down program
};

#endif // RUN_H
