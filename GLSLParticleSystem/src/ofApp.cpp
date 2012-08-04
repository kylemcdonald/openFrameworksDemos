#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	ofSetBackgroundAuto(false);
	ofBackground(128);
	updateShader.setup("updateShader");
	drawShader.setup("drawShader");
	
	side = 512;
	
	ofFbo::Settings settings;
	settings.width = side;
	settings.height = side;
	settings.maxFilter = GL_NEAREST;
	settings.minFilter = GL_NEAREST;
	settings.internalformat = GL_RGBA32F;
	ping.allocate(settings);
	pong.allocate(settings);
	
	img.allocate(side, side, OF_IMAGE_COLOR_ALPHA);
	mesh.setMode(OF_PRIMITIVE_POINTS);
	for(int y = 0; y < side; y++) {
		for(int x = 0; x < side; x++) {
			img.setColor(x, y, ofFloatColor(ofRandom(1), ofRandom(1), .5, .5));
			mesh.addVertex(ofVec2f(x, y));
		}
	}
	img.update();
	
	randomize();
}
void ofApp::randomize() {
	ping.begin();
	img.draw(0, 0);
	ping.end();
	pong.begin();
	img.draw(0, 0);
	pong.end();
}

void ofApp::update() {
	bool usePing = ofGetFrameNum() % 2 == 0;
	ofFbo& toFbo = usePing ? ping : pong;
	ofFbo& fromFbo = usePing ? pong : ping;
	toFbo.begin();
	updateShader.begin();
	ofSetMinMagFilters(GL_NEAREST, GL_NEAREST);
	updateShader.setUniform2f("mouse", (float) mouseX / ofGetWidth(), (float) mouseY / ofGetHeight());
	updateShader.setUniformTexture("tex", fromFbo, 0);
	fromFbo.draw(0, 0);
	updateShader.end();
	toFbo.end();
}

void ofApp::draw() {	
	ofPushMatrix();
	ofScale(ofGetWidth(), ofGetHeight());
	ofNoFill();
	
	drawShader.begin();
	ofSetMinMagFilters(GL_NEAREST, GL_NEAREST);
	bool usePing = ofGetFrameNum() % 2 == 0;	
	drawShader.setUniformTexture("tex", usePing ? ping : pong, 0);
	mesh.draw();
	drawShader.end();
	ofPopMatrix();
}

void ofApp::keyPressed(int key) {
	if(key == ' ') {
		randomize();
	}
}