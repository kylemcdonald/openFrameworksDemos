#pragma once

#include "Tree.h"
#include "Particle.h"

#define USE_GCD

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

class ParticleSystem {
protected:
	bool exact;
	float gravitationalConstant;
	float timeStep;
    float friction;
    int iterations;
    float centering;
    
    vector<Particle> particles;
    vector<Spring> springs;
    
	void zeroForces() {
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			particles[i].zeroForce();
		}
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
			Particle& a = particles[i];
			for(int j = 0; j < n; j++) {
				if(i != j) {
					Particle& b = particles[j];
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
        for(Particle& cur : particles) {
            // could probably square the constant...
            cur.force *= cur.mass * gravitationalConstant;
        }
    }
    void applyCentering() {
        for(Particle& cur : particles) {
            float length = cur.length();
            cur.force += cur.mass * centering * -cur;
        }
    }
    void applySprings() {
        for(Spring& spring : springs) {
            spring.update();
        }
//        int n = springs.size();
//        LOOP_BEGIN(i, n) {
//            springs[i].update();
//        } LOOP_END();
    }
    void updatePositions(float dt) {
        for(Particle& particle : particles) {
            particle.updatePosition(dt, friction);
        }
//        int n = particles.size();
//        LOOP_BEGIN(i, n) {
//			particles[i].updatePosition(dt, friction);
//        } LOOP_END();
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
	void add(Particle& p) {
		particles.push_back(p);
    }
    void addSpring(Particle* a, Particle* b, float stiffness, float distance = 0) {
       Spring spring(a, b, stiffness, distance);
       springs.push_back(spring);
    }
    Particle& getParticle(int i) {
        return particles[i];
    }
    unsigned int size() {
        return particles.size();
    }
	void clear() {
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
    void draw() {
        ofPushStyle();
        int i = 0;
        for(Particle& particle : particles) {
            ofSetColor(ofColor::fromHsb(((i++)%25)*10, 255, 255));
			particle.draw();
        }
        ofSetLineWidth(4);
        ofSetColor(255, 128);
        for(Spring& spring : springs) {
            spring.draw();
        }
        ofPopStyle();
	}
};
