#pragma once

#include "Body.h"
#include "ofGraphics.h"

class Particle : public Body {
public:
    ofVec2f prevVelocity;
    ofVec2f force, velocity;
    float radius;
    Particle(float _x, float _y, float _mass,
             float _xv = 0, float _yv = 0) :
    Body(_x, _y, _mass),
    velocity(_xv, _yv) {
        radius = sqrtf(mass);
    }
    void updatePosition(float dt, float friction = 1) {
        ofVec2f acceleration = force / mass;
        prevVelocity = velocity;
        velocity += dt * acceleration;
        velocity *= friction;
        *this += dt * prevVelocity.interpolate(velocity, .5);
    }
    void zeroForce() {
        force.set(0, 0);
    }
    void draw() {
        //ofRect(x, y, mass, mass);
        ofDrawCircle(x, y, radius);
    }
};
