#pragma once

#include "Tree.h"
#include "Particle.h"

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
    void applyCentering() {
        for(Particle& cur : particles) {
            float length = cur.length();
            cur.force += centering * -cur;
        }
    }
	vector<Particle> particles;
	void approximateSolve() {
		Tree tree;
		tree.build(particles);
        ofPushStyle();
        ofNoFill();
        ofSetColor(255);
		tree.draw();
        ofPopStyle();
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			Particle& cur = particles[i];
			tree.sumForces(cur);
		}
	}
	void exactSolve() {
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			Particle& a = particles[i];
			for(int j = 0; j < n; j++) {
				if(i != j) {
					Particle& b = particles[j];
                    ofVec2f d = b - a;
                    float r = d.length();
                    float r2 = r * r;
                    d /= r;
                    a.force += d * (b.mass / r2);
				}
            }
            a.force *= a.mass * gravitationalConstant;
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
                exactSolve();
            } else {
                approximateSolve();
            }
            applyCentering();
            updatePositions(dt);
        }
	}
	void draw() {
		for(int i = 0; i < particles.size(); i++)
			particles[i].draw();
	}
};
