#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofShader bulgeShader, blendBulgeShader;
	ofImage img;
	
	void buildBulge(int bulgeSize);
	int bulgeSize;
	ofFbo bulge, blendBulge;
};
