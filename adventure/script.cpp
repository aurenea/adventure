#include "script.h"

Script::Script(Script* ns) {
    next_script = ns;
}

int Script::activate(ALLEGRO_EVENT e) {
    if (next_script != NULL) {
        return next_script->activate(e);
    }
}

ConditionalScript::ConditionalScript(Script* ns, Script* fs, bool (*cf)(ALLEGRO_EVENT e)) : Script(ns) {
    failure_script = fs;
    condition_function = cf;
}

int ConditionalScript::activate(ALLEGRO_EVENT e) {
    if (condition_function(e)) {
        return Script::activate(e);
    } else if (failure_script != NULL) {
        return failure_script->activate(e);
    }
}

int ScriptLord::add(Script* s) {
    if (length == array_length) {
        array_length += (int)(2*powf(array_length+1, 0.25));
        Script** store = new Script*[array_length];
        for (int i = 0; i < array_length; i++) {
            if (i < length) { store[i] = vassals[i]; }
            else { store[i] = NULL; }
        }
        delete [] vassals;
        vassals = store;
    }
    vassals[length] = s;
    return length++;
}

Script* ScriptLord::discard(Script* s) {
    bool found = false;
    for (int i = 0; i < length; i++) {
        if (vassals[i] == s) {
            found = true;
            vassals[i] = NULL;
        } else if (found) {
            vassals[i-1] = vassals[i];
            vassals[i] = NULL;
        }
    }
    length--;
    return s;
}

int ScriptLord::activate(ALLEGRO_EVENT e) {
    for (int i = 0; i < length; i++) {
        vassals[i]->activate(e);
    }
    return 1;
}
