#pragma once

#include "ofMain.h"
#include "DelayTimer.h"
#include "ofxQTKitVideoPlayer.h"
#include "ofxXmlSettings.h"

class Description {
public:
	string file, h1, h2; 
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	DelayTimer timer;
	ofxQTKitVideoPlayer video;
	
	ofTrueTypeFont h1, h2;
	vector<Description> descriptions;
	vector<int> playlist;
	int which;
	Description cur;
	bool skip;
};
