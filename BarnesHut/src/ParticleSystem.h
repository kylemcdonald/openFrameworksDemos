#pragma once

#include "Tree.h"
#include "Particle.h"

#define USE_GCD

class ParticleSystem {
protected:
	bool exact;
	float gravitationalConstant;
	float timeStep;
    float friction;
    int iterations;
    float centering;

	void zeroForces() {
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			particles[i].zeroForce();
		}
	}
	vector<Particle> particles;
	void sumGravityApproximate() {        
        // this takes about 2400us
		Tree tree;
        tree.build(particles);
        
//        ofPushStyle();
//        ofNoFill();
//        ofSetColor(255);
//        tree.draw();
//        ofPopStyle();
        
        int n = particles.size();
#ifdef USE_GCD
        dispatch_apply(n, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^(size_t i) {
#else
        for(int i = 0; i < n; i++) {
#endif
            tree.sumForces(particles[i]);
        }
#ifdef USE_GCD
        );
#endif;
	}
	void sumGravityExact() {
		int n = particles.size();
#ifdef USE_GCD
        dispatch_apply(n, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^(size_t i) {
#else
        for(int i = 0; i < n; i++) {
#endif
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
		}
#ifdef USE_GCD
        );
#endif
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
    void updatePositions(float dt) {
        for(Particle& cur : particles) {
			cur.updatePosition(dt, friction);
		}
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
            applyCentering();
            updatePositions(dt);
        }
	}
    void draw() {
        int i = 0;
        for(Particle& particle : particles) {
            ofSetColor(ofColor::fromHsb(((i++)%25)*10, 255, 255));
			particle.draw();
        }
	}
};
