#pragma once

#include "ofMain.h"
#include "ofAppGlutWindow.h"

class PApp : public ofBaseApp {
public:
	float width, height;
	
	PApp() {
		width = ofGetWidth();
		height = ofGetHeight();
		ofEnableAlphaBlending();
	}
	
	int framecount;
	void update() {
		framecount = ofGetFrameNum();
	}
	
	int x, y, button;
	virtual void mousePressed() {
	}
	virtual void mouseReleased() {
	}
	virtual void mouseDragged() {
	}
	virtual void mouseMoved() {
	}
	void mousePressed(int x, int y, int button) {
		this->x = x;
		this->y = y;
		this->button = button;
		mousePressed();
	}
	void mouseReleased(int x, int y, int button) {
		this->x = x;
		this->y = y;
		this->button = button;
		mouseReleased();
	}
	void mouseDragged(int x, int y, int button) {
		this->x = x;
		this->y = y;
		this->button = button;
		mouseDragged();
	}
	void mouseMoved(int x, int y) {
		this->x = x;
		this->y = y;
		mouseMoved();
	}
	
	int key;
	virtual void keyPressed() {
	}
	virtual void keyReleased() {
	}
	void keyPressed(int key) {
		this->key = key;
		keyPressed();
	}
	void keyReleased(int key) {
		this->key = key;
		keyReleased();
	}
	
	void background(ofColor color) {
		ofBackground(color);
	}
	void background(float r, float g, float b) {
		ofBackground(r, g, b);
	}
	void pushMatrix() {
		ofPushMatrix();
	}
	void popMatrix() {
		ofPopMatrix();
	}
	void translate(float x, float y, float z = 0) {
		ofTranslate(x, y, z);
	}
	void stroke(ofColor color) {
		ofSetColor(color);
	}
	void stroke(ofColor color, float a) {
		ofSetColor(color, a);
	}
	void stroke(float r, float g, float b, float a = 255) {
		ofSetColor(r, g, b, a);
	}
	void fill(ofColor color) {
		ofSetColor(color);
	}
	void fill(ofColor color, float a) {
		ofSetColor(color, a);
	}
	void fill(float r, float g, float b, float a = 255) {
		ofSetColor(r, g, b, a);
	}
	float random(float max) {
		return ofRandom(max);
	}
	float random(float min, float max) {
		return ofRandom(min, max);
	}
	float norm(float x, float min, float max) {
		return ofNormalize(x, min, max);
	}
	float map(float x, float inMin, float inMax, float outMin, float outMax) {
		return ofMap(x, inMin, inMax, outMin, outMax);
	}
	float lerp(float min, float max, float amt) {
		return ofLerp(min, max, amt);
	}
	float noise(float x) {
		return ofNoise(x);
	}
	float noise(float x, float y) {
		return ofNoise(x, y);
	}
	float noise(float x, float y, float z) {
		return ofNoise(x, y, z);
	}
	void ellipse(float x, float y, float width, float height) {
		ofEllipse(x, y, width, height);
	}
	void line(float x1, float y1, float x2, float y2) {
		ofLine(x1, y1, x2, y2);
	}
};
