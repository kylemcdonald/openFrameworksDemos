#include "ofApp.h"

void ofApp::buildBulge(int bulgeSize) {
	this->bulgeSize = bulgeSize;
	ofFbo::Settings settings;
	settings.width = bulgeSize, settings.height = bulgeSize;
	settings.internalformat = GL_RGB32F;
	bulge.allocate(settings);
	bulge.begin();
	shader.begin();
	shader.setUniform1f("scale", bulgeSize / 2);
	shader.setUniform1f("shape", 1.5);
	ofRect(0, 0, bulgeSize, bulgeSize);
	shader.end();
	bulge.end();
}

void ofApp::setup() {
	ofSetVerticalSync(true);
	shader.load("bulge.vs", "bulge.fs");
	buildBulge(512);
}

void ofApp::update() {
}

void ofApp::draw() {
	bulge.draw(0, 0);
}