#pragma once

#include "Particle.h"

#define maxParticles 4

class Tree : public Body {
public:
	bool empty;
	int nParticles;
	shared_ptr<Particle> particles[maxParticles];
	bool hasChildren;
	shared_ptr<Tree> nw, ne, sw, se;
    float minX, minY, midX, midY, maxX, maxY, side, diag;

	Tree() :
		empty(true),
		nParticles(0),
		hasChildren(false),
		minX(0), minY(0),
		midX(0), midY(0),
		maxX(0), maxY(0) {
	}
	Tree(float _minX, float _minY, float _maxX, float _maxY) :
		empty(true),
		nParticles(0),
		hasChildren(false),
		minX(_minX), minY(_minY),
		maxX(_maxX), maxY(_maxY) {
		setMid();
	}
	void setMid() {
		midX = (minX + maxX) / 2;
		midY = (minY + maxY) / 2;
		side = maxX - minX;
        diag = sqrt(2 * side * side);
	}
	void findBoundaries(vector<shared_ptr<Particle> >& all) {
		minX = all[0]->x;
		minY = all[0]->y;
		maxX = minX;
		maxY = minY;
		int n = all.size();
		for(int i = 0; i < n; i++) {
			if(all[i]->x < minX)
				minX = all[i]->x;
			if(all[i]->y < minY)
				minY = all[i]->y;
			if(all[i]->x > maxX)
				maxX = all[i]->x;
			if(all[i]->y > maxY)
				maxY = all[i]->y;
		}
		setMid();
	}
	void squareBoundaries() {
		float centerX = (minX + maxX) / 2;
		float centerY = (minY + maxY) / 2;
		float diffX = maxX - minX;
		float diffY = maxY - minY;
		float halfSide = max(diffX, diffY) / 2;
		minX = centerX - halfSide;
		maxX = centerX + halfSide;
		minY = centerY - halfSide;
		maxY = centerY + halfSide;
		setMid();
	}
	void add(shared_ptr<Particle>& cur) {
		if(hasChildren) {
			if(cur->x < midX) {
				if(cur->y < midY) {
					nw->add(cur);
				} else {
					sw->add(cur);
				}
			} else {
				if(cur->y < midY) {
					ne->add(cur);
				} else {
					se->add(cur);
				}
			}
		} else {
			if(nParticles < maxParticles) {
				particles[nParticles] = cur;
				nParticles++;
				empty = false;
			} else {
				nw = shared_ptr<Tree>(new Tree(minX, minY, midX, midY));
				ne = shared_ptr<Tree>(new Tree(midX, minY, maxX, midY));
				sw = shared_ptr<Tree>(new Tree(minX, midY, midX, maxY));
				se = shared_ptr<Tree>(new Tree(midX, midY, maxX, maxY));
				hasChildren = true;
				for(int i = 0; i < nParticles; i++)
					add(particles[i]);
				nParticles = 0;
				add(cur);
			}
		}
	}
	void addAll(vector<shared_ptr<Particle> >& all) {
		int n = all.size();
		for(int i = 0; i < n; i++)
			add(all[i]);
	}
	void findCenterOfMass() {
		if(hasChildren) {
			nw->findCenterOfMass();
			ne->findCenterOfMass();
			sw->findCenterOfMass();
			se->findCenterOfMass();
			mass +=
				nw->mass +
				ne->mass +
				sw->mass +
				se->mass;
			x +=
				nw->x * nw->mass +
				ne->x * ne->mass +
				sw->x * sw->mass +
				se->x * se->mass;
			x /= mass;
			y +=
				nw->y * nw->mass +
				ne->y * ne->mass +
				sw->y * sw->mass +
				se->y * se->mass;
			y /= mass;
		} else {
			if(nParticles) {
				for(int i = 0; i < nParticles; i++) {
					shared_ptr<Particle>& cur = particles[i];
					x += cur->x * cur->mass;
					y += cur->y * cur->mass;
					mass += cur->mass;
				}
				x /= mass;
                y /= mass;
            }
		}
	}
	void build(vector<shared_ptr<Particle> >& particles) {
		findBoundaries(particles);
		squareBoundaries();
		addAll(particles);
		findCenterOfMass();
	}
	void sumForces(shared_ptr<Particle>& cur) const {
		if(!empty) {
            ofVec2f d = *this - *cur;
            if(d.x > +side || d.x < -side ||
               d.y > +side || d.y < -side) {
                // far away, approximate
                float r = d.length();
                d *= mass / (r * r * r);
                cur->force += d;
            } else if(nParticles) {
                // close, sum particles
                for(int i = 0; i < nParticles; i++) {
                    const shared_ptr<Particle>& target = particles[i];
                    // exact calculations
                    if(target != cur) {
                        d = *target - *cur;
                        float r = d.length();
                        r = MAX(r, MAX(target->radius, cur->radius));
                        d *= target->mass / (r * r * r);
                        cur->force += d;
                    }
//                    ofDrawLine(cur.x, cur.y, this->x, this->y);
                }
            } else {
                // too close, go deeper
                nw->sumForces(cur);
                ne->sumForces(cur);
                sw->sumForces(cur);
                se->sumForces(cur);
            }
		}
	}
	void draw() const {
		ofDrawRectangle(minX, minY, maxX - minX, maxY - minY);
		if(hasChildren) {
			nw->draw();
			ne->draw();
			sw->draw();
			se->draw();
		}
	}
};
