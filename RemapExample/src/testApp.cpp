#include "testApp.h"

void testApp::setup(){
	ofSetVerticalSync(true);
	camera.initGrabber(camWidth, camHeight);
	remapImage.loadImage("remap.exr");
	remap.setup(camWidth, camHeight);
	remap.setSource(camera.getTextureReference());
	remap.setRemap(remapImage.getTextureReference());
}

void testApp::update(){
	camera.update();
	if(camera.isFrameNew()) {
		remap.update();
	}
}

void testApp::draw(){	
	ofSetColor(255);
	remap.getTexture().draw(0, 0);
	ofSetColor(0);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}
