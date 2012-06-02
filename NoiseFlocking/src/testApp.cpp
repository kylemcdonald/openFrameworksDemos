#include "testApp.h"

void testApp::setup() {
	//ofSetBackgroundAuto(false);
	//ofSetVerticalSync(true);
	Particle::setup(50000, 100);	
	ofBackground(0);
}

void testApp::update() {
	if(ofGetMousePressed()) {
		Particle::addForce(ofVec2f(mouseX - ofGetWidth() / 2, mouseY - ofGetHeight() / 2), 100);
	}
	Particle::updateAll(60 * ofGetLastFrameTime());
}

void testApp::draw() {	
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
	ofSetColor(255);
	//glPointSize(2);
	//glEnable(GL_POINT_SMOOTH);
	Particle::drawAll();
	ofPopMatrix();
	
	ofSetColor(0);
	ofRect(0, 0, 100, 100);
	ofSetColor(255);
	//ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void testApp::keyPressed(int key) {
	if(key == 'f')
		ofToggleFullscreen();
}
