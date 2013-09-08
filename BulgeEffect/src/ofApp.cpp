#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	shader.load("bulge.vs", "bulge.fs");
	//img.loadImage("gradient-grid.png");
	img.initGrabber(640, 480);
}

void ofApp::update() {
	img.update();
}

void ofApp::draw() {
	shader.begin();
	shader.setUniform2f("center", mouseX, mouseY);
	shader.setUniform1f("radius", 256);
	shader.setUniform1f("shape", 1.5);
	shader.setUniformTexture("tex", img.getTextureReference(), 0);
	img.draw(0, 0);
	shader.end();
}