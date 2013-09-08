#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	cam.initGrabber(1280, 720);
	delayTime = 1;
	frameCount = 0;
	lastTime = 0;
}

void ofApp::update() {
	cam.update();
	float curTime = ofGetElapsedTimef();
	if(curTime - lastTime > delayTime) {
		img = cam.getPixelsRef();
		img.update();
		img.saveImage(ofToString(frameCount, 8, '0') + ".jpg");
		frameCount++;
		lastTime = curTime;
	}	
}

void ofApp::draw() {
	if(img.isAllocated()) {
		img.draw(0, 0);
	}
}