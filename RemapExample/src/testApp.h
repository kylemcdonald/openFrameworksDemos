#pragma once

#include "ofMain.h"
#include "ofxRemap.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	static const int camWidth = 1280;
	static const int camHeight = 720;
	
	ofVideoGrabber camera;
	
	ofFloatImage remapImage;
	ofxRemap remap;
};
