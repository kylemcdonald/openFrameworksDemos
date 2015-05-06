#pragma once

class Body : public ofVec2f {
public:
    float mass;
    Body(float x = 0, float y = 0, float mass = 0) :
    ofVec2f(x, y),
    mass(mass) {
    }
};
