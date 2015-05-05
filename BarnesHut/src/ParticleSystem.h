#pragma once

#include "Tree.h"
#include "Particle.h"
#include "Spring.h"

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

bool memoryLocation(const shared_ptr<Particle>& a, const shared_ptr<Particle>& b) {
    return ((size_t) a.get()) < ((size_t) b.get());
}

void sortByLocation(vector<shared_ptr<Particle> >& data) {
    ofSort(data, memoryLocation);
}

template <class T>
void checkMemoryDistance(vector<shared_ptr<T> >& data) {
    shared_ptr<T> prev;
    size_t minDiff = 0;
    size_t maxDiff = 0;
    float sumDiff = 0;
    for(shared_ptr<T>& cur : data) {
        if(prev) {
            size_t a = (size_t) prev.get();
            size_t b = (size_t) cur.get();
            size_t absDiff = 0;
            if(a > b) {
                absDiff = a - b;
            } else {
                absDiff = b - a;
            }
            if(minDiff == 0 || absDiff < minDiff) {
                minDiff = absDiff;
            }
            if(maxDiff == 0 || absDiff > maxDiff) {
                maxDiff = absDiff;
            }
            sumDiff += absDiff;
        }
        prev = cur;
    }
    float avgDiff = sumDiff / data.size();
    
    cout << (int) avgDiff << endl;
//    << ", "
//        << minDiff << ", "
//        << maxDiff << endl;
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
		for(int i = 0; i < n; i++) {
			particles[i]->zeroForce();
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
			shared_ptr<Particle>& a = particles[i];
			for(int j = 0; j < n; j++) {
				if(i != j) {
					shared_ptr<Particle> b = particles[j];
                    ofVec2f d = *b - *a;
                    float r = d.length();
                    r = MAX(r, (a->radius + b->radius));
                    float mor3 = b->mass / (r * r * r);
                    a->force += d * mor3;
				}
            }
        } LOOP_END();
    }
    void applyGravity() {
        for(auto cur : particles) {
            // could probably square the constant...
            cur->force *= cur->mass * gravitationalConstant;
        }
    }
    void applyCentering() {
        for(auto cur : particles) {
            float length = cur->length();
            cur->force += cur->mass * centering * -(*cur);
        }
    }
    void applySprings() {
        for(auto spring : springs) {
            spring->update();
        }
//        int n = springs.size();
//        LOOP_BEGIN(i, n) {
//            springs[i]->update();
//        } LOOP_END();
    }
    void updatePositions(float dt) {
        for(auto particle : particles) {
            particle->updatePosition(dt, friction);
        }
//        int n = particles.size();
//        LOOP_BEGIN(i, n) {
//			particles[i]->updatePosition(dt, friction);
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
	void add(const Particle& particle) {
        add(shared_ptr<Particle>(new Particle(particle)));
    }
    void add(shared_ptr<Particle> particle) {
		particles.push_back(particle);
        sortByLocation(particles);
        checkMemoryDistance(particles);
    }
    void addSpring(shared_ptr<Particle> a, shared_ptr<Particle> b, float stiffness, float distance = 0) {
        addSpring(shared_ptr<Spring>(new Spring(a, b, stiffness, distance)));
    }
    void addSpring(shared_ptr<Spring> spring) {
       springs.push_back(spring);
    }
    shared_ptr<Particle> getParticle(int i) {
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
        for(auto particle : particles) {
            ofSetColor(ofColor::fromHsb(((i++)%25)*10, 255, 255));
			particle->draw();
        }
        ofSetLineWidth(4);
        ofSetColor(255, 128);
        for(auto spring : springs) {
            spring->draw();
        }
        ofPopStyle();
	}
};
