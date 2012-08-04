#pragma once

#include "ofMain.h"
#include "Force.h"
#include "Particle.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	vector<ofPtr<Force> > forces;
	vector<Particle> particles;
};
