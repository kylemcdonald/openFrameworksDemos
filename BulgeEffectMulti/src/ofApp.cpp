#include "ofApp.h"

void ofApp::buildBulge(int bulgeSize) {
	this->bulgeSize = bulgeSize;
	bulgeShader.load("", "bulge.fs");
	ofFbo::Settings settings;
	settings.width = bulgeSize, settings.height = bulgeSize;
	settings.internalformat = GL_RGB32F;
	bulge.allocate(settings);
	bulge.begin();
	bulgeShader.begin();
	bulgeShader.setUniform1f("scale", bulgeSize / 2);
	bulgeShader.setUniform1f("shape", 1.5);
	ofRect(0, 0, bulgeSize, bulgeSize);
	bulgeShader.end();
	bulge.end();
	
	blendBulgeShader.load("", "blendBulge.fs");
	settings.width = ofGetWidth(), settings.height = ofGetHeight();
	blendBulge.allocate(settings);
}

void ofApp::setup() {
	ofSetVerticalSync(true);
	
	buildBulge(512);
}

void ofApp::update() {
}

void ofApp::drawBulge(float x, float y) {
	blendBulge.begin();
	blendBulgeShader.begin();
	bulge.draw(x, y);
	blendBulgeShader.end();
	blendBulge.end();
}

void ofApp::draw() {
	blendBulge.begin();
	ofClear(128, 128, 0);	
	blendBulgeShader.begin();
	blendBulgeShader.setUniformTexture("base", blendBulge.getTextureReference(), 1);
	blendBulgeShader.setUniformTexture("tex", bulge.getTextureReference(), 2);
	blendBulgeShader.end();
	blendBulge.end();
	
	drawBulge((ofGetWidth() - bulgeSize) / 2, (ofGetHeight() - bulgeSize) / 2);
	drawBulge(mouseX - bulgeSize / 2, mouseY - bulgeSize / 2);	
	
	blendBulge.draw(0, 0);
}