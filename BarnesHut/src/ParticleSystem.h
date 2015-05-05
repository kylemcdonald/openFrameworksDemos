#pragma once

#include "Tree.h"
#include "Particle.h"
#include "Spring.h"

#define MAX_PARTICLE_COUNT 10000
#define MAX_SPRING_COUNT 10000

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

template <class T>
void checkMemoryDistance(const vector<T>& data) {
    size_t prevLocation = 0;
    float sumDiff = 0;
    for(const T& cur : data) {
        size_t curLocation = (size_t) &cur;
        if(prevLocation) {
            size_t absDiff = 0;
            if(prevLocation > curLocation) {
                absDiff = prevLocation - curLocation;
            } else {
                absDiff = curLocation - prevLocation;
            }
            sumDiff += absDiff;
        }
        prevLocation = curLocation;
    }
    float avgDiff = sumDiff / data.size();
    cout << (int) avgDiff << endl;
}

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
					const Particle& b = particles[j];
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
        int n = springs.size();
        LOOP_BEGIN(i, n) {
            springs[i].update();
        } LOOP_END();
    }
    void updatePositions(float dt) {
        int n = particles.size();
        LOOP_BEGIN(i, n) {
			particles[i].updatePosition(dt, friction);
        } LOOP_END();
	}
public:
	ParticleSystem() {
		this->exact = true;
		this->gravitationalConstant = 1;
		this->timeStep = 1;
		this->friction = 1;
        this->iterations = 1;
        particles.reserve(MAX_PARTICLE_COUNT);
        springs.reserve(MAX_SPRING_COUNT);
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
	void add(const Particle& particle) {
		particles.push_back(particle);
    }
    void addSpring(Particle& a, Particle& b, float stiffness, float distance = 0) {
        springs.push_back(Spring(a, b, stiffness, distance));
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
        for(const Particle& particle : particles) {
            ofSetColor(ofColor::fromHsb(((i++)%25)*10, 255, 255));
			particle.draw();
        }
        ofSetLineWidth(4);
        ofSetColor(255, 128);
        for(const Spring& spring : springs) {
            spring.draw();
        }
        ofPopStyle();
	}
};
