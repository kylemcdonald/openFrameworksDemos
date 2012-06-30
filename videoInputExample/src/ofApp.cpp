#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	cam.initGrabber(800, 600);
}

void ofApp::update() {
	if(cam.update()) {
	}
}

void ofApp::draw() {
	cam.draw(0, 0);
}