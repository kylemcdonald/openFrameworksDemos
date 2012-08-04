#pragma once

#include "Body.h"
#include "ofGraphics.h"

class Particle : public Body {
public:
	float xv, yv;
	float xf, yf;
	float radius;
	Particle(float _x, float _y, float _mass,
		float _xv = 0, float _yv = 0, float radius = 1) :
		Body(_x, _y, _mass),
		xv(_xv), yv(_yv),
		radius(radius) {
	}
	void updatePosition(float timeStep, float gravitationalConstant = 0) {		
		// f = ma, a = f / m, a = G * m * f / m, a = G * f
		float xa = xf * gravitationalConstant;
		float ya = yf * gravitationalConstant;
		xv += xa * timeStep;
		yv += ya * timeStep;
		x += xv * timeStep;
		y += yv * timeStep;
	}
	void zeroForce() {
		xf = 0;
		yf = 0;
	}
	void applyForce(float gravitationalConstant, float friction) {
		xf *= gravitationalConstant * mass;
		yf *= gravitationalConstant * mass;
		xv += xf;
		yv += yf;
		xv *= friction;
		yv *= friction;
	}
	void draw() {
		//ofRect(x, y, mass, mass);
		float s = 100. / 1.471e+11;
		ofCircle(s * x, s * y, 2);
	}
};
