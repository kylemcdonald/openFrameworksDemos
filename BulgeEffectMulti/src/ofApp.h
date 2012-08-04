#pragma once

#include "ofMain.h"
#include "BulgeEffect.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	BulgeEffect bulgeEffect;
	ofVideoGrabber cam;
};
