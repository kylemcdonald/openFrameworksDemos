#pragma once

#include "ofMain.h"

class Lut {
private:
	ofTexture* source;
	ofFloatImage lut;
	ofFbo result;
	
	ofShader lutShader;
	
public:
	void setup(int width, int height, string filename) {
		result.allocate(width, height, GL_RGB); // doesn't need a depth buffer
		lutShader.load("", "lut.frag");
		lut.loadImage(filename);
	}
	void setSource(ofTexture& source) {
		this->source = &source;
	}
	void setGamma(float gamma) {
		setGamma(ofFloatColor(gamma, gamma, gamma));
	}
	void setGamma(ofFloatColor gamma) {
		int n = 256;
		for(int i = 0; i < n; i++) {
			float x = (float) i / (n - 1.);
			ofFloatColor cur(
				powf(x, 1. / gamma.r),
				powf(x, 1. / gamma.g),
				powf(x, 1. / gamma.b));
			lut.setColor(i, 0, cur);
		}
		lut.update();
	}
	void update() {
		ofPushMatrix();
		result.begin();
		lutShader.begin();
		lutShader.setUniformTexture("source", *source, 0);
		lutShader.setUniformTexture("lut", lut.getTextureReference(), 1);
		source->draw(0, 0);
		lutShader.end();
		result.end();
		ofPopMatrix();
	}
	ofTexture& getTexture() {
		return result.getTextureReference();
	}
};