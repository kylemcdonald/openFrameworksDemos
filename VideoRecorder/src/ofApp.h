#pragma once

#include "ofMain.h"
#include "ofxQTKitVideoGrabber.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	void startPreview(); void stopPreview();
	void startRecord(); void stopRecord();
	void startPlayback(); void stopPlayback();
	
	ofxQTKitVideoGrabber grabber;
	ofVideoPlayer player;
	
	int camWidth, camHeight, count;
	
	enum Mode {PREVIEW, RECORD, PLAYBACK};
	Mode mode;
	string stringify(Mode mode) {
		switch(mode) {
			case PREVIEW: return "Preview";
			case RECORD: return "Record";
			case PLAYBACK: return "Playback";
		}
	}
};
