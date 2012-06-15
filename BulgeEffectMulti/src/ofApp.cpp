#include "ofApp.h"

void ofApp::buildBulge(int bulgeSize) {
	this->bulgeSize = bulgeSize;
	ofShader generateBulgeShader;
	generateBulgeShader.load("", "generateBulge.fs");
	ofFbo::Settings settings;
	settings.width = bulgeSize, settings.height = bulgeSize;
	settings.internalformat = GL_RGB32F;
	bulge.allocate(settings);
	bulge.begin();
	generateBulgeShader.begin();
	generateBulgeShader.setUniform1f("scale", bulgeSize / 2);
	generateBulgeShader.setUniform1f("shape", 1.5);
	ofRect(0, 0, bulgeSize, bulgeSize);
	generateBulgeShader.end();
	bulge.end();
	
	blendBulgeShader.load("", "blendBulge.fs");
	settings.width = ofGetWidth(), settings.height = ofGetHeight();
	blendBulge.allocate(settings);
	
	applyBulgeShader.load("", "applyBulge.fs");
}

void ofApp::setup() {
	ofSetVerticalSync(true);
	cam.initGrabber(1280, 720);
	buildBulge(512);
}

void ofApp::update() {
	cam.update();
}

void ofApp::drawBulge(float x, float y, float side) {
	blendBulge.begin();
	blendBulgeShader.setUniform1f("scale", side / bulgeSize);
	bulge.draw(x - side / 2, y - side / 2, side, side);
	blendBulge.end();
}

void ofApp::draw() {
	blendBulgeShader.begin();
	blendBulgeShader.setUniformTexture("base", blendBulge.getTextureReference(), 1);
	blendBulge.begin();
	ofClear(128, 128, 0);	
	blendBulgeShader.setUniformTexture("tex", bulge.getTextureReference(), 2);
	blendBulge.end();
	
	if(ofGetMousePressed()) {
		ofSeedRandom(0);
		for(int i = 0; i < 64; i++) {
			float x = ofMap(sin(ofGetElapsedTimef() * ofRandom(3)), -1, 1, 0, ofGetWidth());
			float y = ofMap(sin(ofGetElapsedTimef() * ofRandom(3)), -1, 1, 0, ofGetHeight());
			drawBulge(x, y, bulgeSize / ofRandom(1, 4));
		}
	} else {
		drawBulge(ofGetWidth() / 2, ofGetHeight() / 2, bulgeSize);
		drawBulge(mouseX, mouseY, bulgeSize / 4);
	}
	
	blendBulgeShader.end();
	
	applyBulgeShader.begin();
	applyBulgeShader.setUniformTexture("tex", cam.getTextureReference(), 1);
	applyBulgeShader.setUniformTexture("bulge", blendBulge.getTextureReference(), 2);
	cam.draw(0, 0);
	applyBulgeShader.end();
	
	ofDrawBitmapStringHighlight(ofToString((int) ofGetFrameRate()), 10, 20);
}