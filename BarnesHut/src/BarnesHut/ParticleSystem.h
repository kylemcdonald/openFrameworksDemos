#pragma once

#include "Tree.h"
#include "Particle.h"

class ParticleSystem {
protected:
	bool exact;
	float gravitationalConstant;
	float timeStep;
	float friction;

	void zeroForces() {
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			particles[i].zeroForce();
		}
	}
	vector<Particle> particles;
	void approximateSolve() {
		Tree tree;
		tree.build(particles);
		//tree.draw();
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			Particle& cur = particles[i];
			cur.zeroForce();
			tree.sumForces(cur);
			cur.applyForce(gravitationalConstant, friction);
		}
	}
	void exactSolve() {
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			Particle& a = particles[i];
			for(int j = 0; j < n; j++) {
				if(i != j) {
					Particle& b = particles[j];
					float xd = b.x - a.x;
					float yd = b.y - a.y;
					float r = sqrtf(xd * xd + yd * yd);
					
					// bounce if too close
					if(r < a.mass + b.mass) {
					//	r = -r;
					}
					
					xd /= r;
					yd /= r;
					float r2 = r * r;
					
					a.xf += xd * (b.mass / r2);
					a.yf += yd * (b.mass / r2);
				}
			}
		}
	}
	void updatePositions() {
		int n = particles.size();
		for(int i = 0; i < n; i++) {
			particles[i].updatePosition(timeStep, gravitationalConstant);
		}
	}
public:
	ParticleSystem() {
		this->exact = true;
		this->gravitationalConstant = 1;
		this->timeStep = 1;
		this->friction = 1;
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
	void setFriction(float friction) {
		this->friction = friction;
	}
	void add(Particle& p) {
		particles.push_back(p);
	}
	void clear() {
		particles.clear();
	}
	void update() {
		zeroForces();
		if(exact) {
			exactSolve();
		} else {
			approximateSolve();
		}
		updatePositions();
	}
	void draw() {
		for(int i = 0; i < particles.size(); i++)
			particles[i].draw();
	}
};
