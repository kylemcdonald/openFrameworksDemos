#pragma once

#include "ofMain.h"

class Force;
class Particle {
public:
	Particle(float x, float y);
	void addForce(ofPtr<Force> force);
	void update(float timeStep);
	const ofVec2f& getPosition() const;
	const ofVec2f& getVelocity() const;
	void setMaxVelocity(float maxVelocity);
	void draw() const;
protected:
	ofVec2f position, velocity, force;
	vector<ofPtr<Force> > forces;
	float maxVelocity;
};
