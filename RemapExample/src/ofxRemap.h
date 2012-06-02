#pragma once

#include "ofMain.h"

class ofxRemap {
private:
	ofTexture* source;
	ofTexture* remap;
	ofFbo result;
	
	ofShader remapShader;
	
public:
	void setup(int width, int height) {
		result.allocate(width, height, GL_RGBA); // doesn't need a depth buffer
		remapShader.load("", "remap.frag");
	}
	void setSource(ofTexture& source) {
		this->source = &source;
	}
	void setRemap(ofTexture& remap) {
		this->remap = &remap;
	}
	void update() {
		ofPushMatrix();
		result.begin();
		remapShader.begin();
		remapShader.setUniformTexture("source", *source, 0);
		remapShader.setUniformTexture("remap", *remap, 1);
		source->draw(0, 0);
		remapShader.end();
		result.end();
		ofPopMatrix();
	}
	ofTexture& getTexture() {
		return result.getTextureReference();
	}
};