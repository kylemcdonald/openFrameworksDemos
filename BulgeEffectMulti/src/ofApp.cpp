#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	cam.initGrabber(1280, 720);
	bulgeEffect.setup(1280, 720);
}

void ofApp::update() {
	cam.update();
}

void ofApp::draw() {
	if(ofGetMousePressed()) {
		bulgeEffect.addBulge(ofGetWidth() / 2 - mouseX, ofGetHeight() / 2, mouseY);
		bulgeEffect.addBulge(ofGetWidth() / 2 + mouseX, ofGetHeight() / 2, mouseY);
	} else {
		ofSeedRandom(0);
		for(int i = 0; i < 64; i++) {
			float x = ofMap(sin(ofGetElapsedTimef() * ofRandom(3)), -1, 1, 0, ofGetWidth());
			float y = ofMap(sin(ofGetElapsedTimef() * ofRandom(3)), -1, 1, 0, ofGetHeight());
			bulgeEffect.addBulge(x, y, ofRandom(64, 512));
		}
	}
	
	bulgeEffect.draw(cam.getTextureReference(), 0, 0);
	
	ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()), 10, 20);
}
