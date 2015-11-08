#include "ofApp.h"

void ofApp::setup(){
	ofSetVerticalSync(true);
	camera.initGrabber(camWidth, camHeight);
	lut.setup(camWidth, camHeight, "blue-boost-lut.exr");
	lut.setSource(camera.getTextureReference());
}

void ofApp::update(){
	camera.update();
	if(camera.isFrameNew()) {
		lut.setGamma(
			ofFloatColor(
				ofMap(mouseX, 0, ofGetWidth(), 0, 3),
				ofMap(mouseY, 0, ofGetHeight(), 0, 3),
				1.));
		lut.update();
	}
}

void ofApp::draw(){	
	ofSetColor(255);
	if(ofGetMousePressed()) {
		camera.draw(0, 0);
	} else {
		lut.getTexture().draw(0, 0);
	}
	ofSetColor(0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}
