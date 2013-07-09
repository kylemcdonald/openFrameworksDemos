#pragma once

#include "ofMain.h"

class ofAutoShader : public ofShader {
public:
	void setup(string name) {
		this->name = name;
		ofAddListener(ofEvents().update, this, &ofAutoShader::update);
	}
	
	void update(ofEventArgs &args) {	
		bool needsReload = false;
			
		string fragName = name + ".fs";
		ofFile fragFile(fragName);
		if(fragFile.exists()) {
			Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
			if(fragTimestamp != lastFragTimestamp) {
				needsReload = true;
				lastFragTimestamp = fragTimestamp;
			}
		} else {
			fragName = "";
		}
		
		string vertName = name + ".vs";
		ofFile vertFile(vertName);
		if(vertFile.exists()) {
			Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
			if(vertTimestamp != lastVertTimestamp) {
				needsReload = true;
				lastVertTimestamp = vertTimestamp;
			}
		} else {
			vertName = "";
		}
		
		if(needsReload) {
			ofShader::load(vertName, fragName);
		}
	}
private:
	string name;
	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void randomize();
	void keyPressed(int key);
	
	ofAutoShader updateShader, drawShader;
	ofFloatImage img;
	ofVboMesh mesh;
	ofFbo ping, pong;
	int side;
};
