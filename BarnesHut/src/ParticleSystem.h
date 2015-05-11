#pragma once

#include "Tree.h"
#include "Particle.h"
#include "Spring.h"

//#define USE_GCD

#ifdef USE_GCD
#define LOOP_BEGIN(i, n) dispatch_apply(n, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^(size_t i)
#else
#define LOOP_BEGIN(i, n) for(int i = 0; i < n; i++)
#endif

#ifdef USE_GCD
#define LOOP_END() )
#else
#define LOOP_END()
#endif

class SpringParticleMatch {
protected:
    shared_ptr<Particle> particle;
public:
    SpringParticleMatch(shared_ptr<Particle>& particle)
    :particle(particle) { }
    bool operator()(const shared_ptr<Spring>& spring) {
        return spring->a == particle ||
        spring->b == particle;
    }
};

template <class T>
void eraseRemove(vector<T>& x, const T& y) {
    x.erase(remove(x.begin(), x.end(), y), x.end());
}

class ParticleSystem {
protected:
    bool exact;
    float gravitationalConstant;
    float timeStep;
    float friction;
    int iterations;
    float centering;
    
    vector<shared_ptr<Particle> > particles;
    vector<shared_ptr<Spring> > springs;
    
    void zeroForces() {
        int n = particles.size();
        LOOP_BEGIN(i, n) {
            particles[i]->zeroForce();
        } LOOP_END();
    }
    void sumGravityApproximate() {
        Tree tree;
        tree.build(particles);
        
        //        ofPushStyle();
        //        ofNoFill();
        //        ofSetColor(255);
        //        tree.draw();
        //        ofPopStyle();
        
        int n = particles.size();
        LOOP_BEGIN(i, n) {
            tree.sumForces(particles[i]);
        } LOOP_END();
    }
    void sumGravityExact() {
        int n = particles.size();
        LOOP_BEGIN(i, n) {
            Particle& a = *particles[i];
            for(int j = 0; j < n; j++) {
                if(i != j) {
                    const Particle& b = *particles[j];
                    ofVec2f d = b - a;
                    float r = d.length();
                    r = MAX(r, (a.radius + b.radius));
                    float mor3 = b.mass / (r * r * r);
                    a.force += d * mor3;
                }
            }
        } LOOP_END();
    }
    void applyGravity() {
        int n = particles.size();
        LOOP_BEGIN(i, n) {
            Particle& cur = *particles[i];
            cur.force *= cur.mass * gravitationalConstant;
        } LOOP_END();
    }
    void applyCentering() {
        int n = particles.size();
        LOOP_BEGIN(i, n) {
            Particle& cur = *particles[i];
            float length = cur.length();
            cur.force += cur.mass * centering * -cur;
        } LOOP_END();
    }
    void applySprings() {
        int n = springs.size();
        LOOP_BEGIN(i, n) {
            springs[i]->update();
        } LOOP_END();
    }
    void updatePositions(float dt) {
        int n = particles.size();
        LOOP_BEGIN(i, n) {
            particles[i]->updatePosition(dt, friction);
        } LOOP_END();
    }
public:
    ParticleSystem() {
        this->exact = true;
        this->gravitationalConstant = 1;
        this->timeStep = 1;
        this->friction = 1;
        this->iterations = 1;
    }
    void setExact(bool exact) {
        this->exact = exact;
    }
    void setGravitationalConstant(float gravitationalConstant) {
        this->gravitationalConstant = gravitationalConstant;
    }
    void setTimeStep(float timeStep) {
        this->timeStep = timeStep;
    }
    void setIterations(int iterations) {
        this->iterations = iterations;
    }
    void setFriction(float friction) {
        this->friction = friction;
    }
    void setCentering(float centering) {
        this->centering = centering;
    }
    shared_ptr<Particle>& getParticle(int i) {
        return particles[i];
    }
    shared_ptr<Particle> addParticle(const Particle& particle) {
        particles.push_back(shared_ptr<Particle>(new Particle(particle)));
        return particles.back();
    }
    void removeParticle(int i) {
        if(!particles.empty()) {
            shared_ptr<Particle>& cur = particles[i];
            ofRemove(springs, SpringParticleMatch(cur));
            particles.erase(particles.begin() + i);
        }
    }
    void removeParticle(shared_ptr<Particle>& particle) {
        eraseRemove(particles, particle);
    }
    unsigned int getParticleCount() {
        return particles.size();
    }
    shared_ptr<Spring>& getSpring(int i) {
        return springs[i];
    }
    void addSpring(int i, int j, float stiffness, float distance = 0) {
        if(i < particles.size() && j < particles.size()) {
            addSpring(particles[i], particles[j], stiffness, distance);
        }
    }
    void addSpring(shared_ptr<Particle>& a, shared_ptr<Particle>& b, float stiffness, float distance = 0) {
        if(a != b) {
            springs.push_back(shared_ptr<Spring>(new Spring(a, b, stiffness, distance)));
        }
    }
    void removeSpring(int i) {
        if(!springs.empty()) {
            springs.erase(springs.begin() + i);
        }
    }
    void removeSpring(shared_ptr<Spring>& spring) {
        (springs, spring);
    }
    unsigned int getSpringCount() {
        return springs.size();
    }
    void clear() {
        springs.clear();
        particles.clear();
    }
    void update() {
        float dt = timeStep / iterations;
        for(int i = 0; i < iterations; i++) {
            zeroForces();
            if(exact) {
                sumGravityExact();
            } else {
                sumGravityApproximate();
            }
            applyGravity();
            if(centering != 0) {
                applyCentering();
            }
            applySprings();
            updatePositions(dt);
        }
    }
    void draw() const {
        ofPushStyle();
        int i = 0;
        for(const shared_ptr<Particle>& particle : particles) {
            ofSetColor(ofColor::fromHsb(((i++)%25)*10, 255, 255));
            particle->draw();
        }
        ofSetColor(255, 64);
        for(const shared_ptr<Spring>& spring : springs) {
            spring->draw();
        }
        ofPopStyle();
    }
};
