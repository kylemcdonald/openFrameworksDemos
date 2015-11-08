#include "ofApp.h"

void ofApp::setup(){
	ofSetVerticalSync(true);
	camera.initGrabber(camWidth, camHeight);
	remapImage.loadImage("remap.exr");
	remap.setup(camWidth, camHeight);
	remap.setSource(camera.getTextureReference());
	remap.setRemap(remapImage.getTextureReference());
}

void ofApp::update(){
	camera.update();
	if(camera.isFrameNew()) {
		remap.update();
	}
}

void ofApp::draw(){	
	ofSetColor(255);
	remap.getTexture().draw(0, 0);
	ofSetColor(0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}
