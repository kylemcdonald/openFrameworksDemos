#pragma once

#include "ofMain.h"
#include "Particle.h"

class Particle;
class Force {
public:
	virtual ofVec2f getForce(Particle& particle) const = 0;
	virtual void draw() const = 0;
};

class ForceLine : public Force {
public:
	ForceLine(ofVec2f begin, ofVec2f end, float radius, float magnitude, float minRadius = 1);
	ofVec2f getForce(Particle& particle) const;
	void draw() const;
protected:
	ofVec2f begin, end;
	float radius, magnitude, minRadius;
};

class ForceCircle : public Force {
public:
	ForceCircle(float x, float y, float radius, float magnitude, float minRadius = 1);
	ofVec2f getForce(Particle& particle) const;
	void draw() const;
protected:
	ofVec2f center;
	float radius, magnitude, minRadius;
};

class ForceFriction : public Force {
public:
	ForceFriction(float magnitude);
	ofVec2f getForce(Particle& particle) const;
	void draw() const {}
protected:
	float magnitude;
};

class FieldDonut : public Force {
public:
	FieldDonut(float x, float y, float radius, float magnitude, float minRadius = 1);
	ofVec2f getForce(Particle& particle) const;
	void draw() const;
protected:
	ofVec2f center;
	float radius, magnitude, minRadius;
};

class FieldLine : public Force {
public:
	FieldLine(ofVec2f begin, ofVec2f end, float magnitude, float minRadius = 1);
	ofVec2f getForce(Particle& particle) const;
	void draw() const;
protected:
	ofVec2f begin, end;
	float magnitude, minRadius;
};