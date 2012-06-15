#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	
	void buildBulge(int bulgeSize);
	void drawBulge(float x, float y);
	int bulgeSize;
	ofShader blendBulgeShader;
	ofFbo bulge, blendBulge;
};
