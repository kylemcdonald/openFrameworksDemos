#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	void buildBulge(int bulgeSize);
	void drawBulge(float x, float y, float side);
	int bulgeSize;
	ofShader blendBulgeShader, applyBulgeShader;
	ofFbo bulge, blendBulge;
	
	ofVideoGrabber cam;
};
