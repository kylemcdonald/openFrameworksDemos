#include "ofApp.h"

void ofApp::setup(){
	ofSetVerticalSync(true);
	// this number describes how many bins are used
	// on my machine, 2 is the ideal number (2^2 = 4x4 pixel bins)
	// if this number is too high, binning is not effective
	// because the screen is not subdivided enough. if
	// it's too low, the bins take up so much memory as to
	// become inefficient.
	int binPower = 5;

	padding = 128;
	particleSystem.setup(ofGetWidth() + padding * 2, ofGetHeight() + padding * 2, binPower);

	kBinnedParticles = 800;
	for(int i = 0; i < kBinnedParticles; i++) {
		float x = ofRandom(0, ofGetWidth()) + padding;
		float y = ofRandom(0, ofGetHeight()) + padding;
		BinnedParticle particle(x, y, 0, 0);
		particleSystem.add(particle);
	}

	timeStep = 100;
	isMousePressed = false;
	slowMotion = true;
	particleNeighborhood = 64;
	particleRepulsion = .5;
	centerAttraction = .01;
	drawBalls = true;
}

void ofApp::update(){
}

void ofApp::draw(){
	ofBackground(0);
	
	particleSystem.setTimeStep(timeStep);

	ofSetColor(255);
	
	// do this once per frame
	particleSystem.setupForces();
	
	ofPushMatrix();
	ofTranslate(-padding, -padding);
	
	// apply per-particle forces
	if(!drawBalls) {
		ofSetLineWidth(8);
		glBegin(GL_LINES); // need GL_LINES if you want to draw inter-particle forces
	}
	for(int i = 0; i < particleSystem.size(); i++) {
		BinnedParticle& cur = particleSystem[i];
		// global force on other particles
		particleSystem.addRepulsionForce(cur, particleNeighborhood, particleRepulsion);
		// forces on this particle
		cur.bounceOffWalls(0, 0, particleSystem.getWidth(), particleSystem.getHeight());
		cur.addDampingForce();
	}
	if(!drawBalls) {
		glEnd();
	}
	
	// single-pass global forces
	particleSystem.addAttractionForce(particleSystem.getWidth() / 2, particleSystem.getHeight() / 2, particleSystem.getWidth() * 100, centerAttraction);
	if(isMousePressed) {
		particleSystem.addRepulsionForce(mouseX + padding, mouseY + padding, 200, 1);
	}
	particleSystem.update(ofGetLastFrameTime());
	
	// draw all the particles
	if(drawBalls) {
		for(int i = 0; i < particleSystem.size(); i++) {
			ofCircle(particleSystem[i].x, particleSystem[i].y, particleNeighborhood * .3);
		}
	}
	
	ofPopMatrix();

	ofSetColor(255);
	ofDrawBitmapString(ofToString(kBinnedParticles) + " particles", 32, 32);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 32, 52);
}

void ofApp::keyPressed(int key){
	if(key == 'p') {
		ofSaveScreen(ofToString(ofGetMinutes()) + "_" + ofToString(ofGetFrameNum()) + ".png");
	}
	if(key == 's') {
		slowMotion = !slowMotion;
		if(slowMotion)
			timeStep = 10;
		else
			timeStep = 100;
	}
	if(key == 'b') {
		drawBalls = !drawBalls;
	}
}

void ofApp::mousePressed(int x, int y, int button){
	isMousePressed = true;
}

void ofApp::mouseReleased(int x, int y, int button){
	isMousePressed = false;
}
