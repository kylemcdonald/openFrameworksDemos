#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	ofShader shader;
	ofImage img;
	
	void buildBulge(int bulgeSize);
	int bulgeSize;
	ofFbo bulge;
};
