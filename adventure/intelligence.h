#ifndef INTELLIGENCE_H
#define INTELLIGENCE_H

enum class AIMode : int {
    MANUAL = 0,         // under manual control of player               PC
    ATTACK_SAME = 1,    // attacks same target as PC                    ally
    ATTACK_WEAK = 2,    // attacks weakest threat
    ATTACK_STRONG = 3,  // attacks strongest threat
    ATTACK_CLOSEST = 4, // attacks closest threat
    PROTECT = 5,        // attack anyone who attacks targeted ally
    TARGET = 6,         // target a specific enemy                      enemy
    DISTANCE = 7,       // attacks from distance, if possible
    PACIFIST = 8,       // don't attack, stay at distance
    FLEE = 9            // don't attack, run away
};

class Agent {
private:
    AIMode mode;
public:
    AIMode get_mode() { return mode; }
    void set_mode(AIMode m) { mode = m; }
};

#endif // INTELLIGENCE_H
