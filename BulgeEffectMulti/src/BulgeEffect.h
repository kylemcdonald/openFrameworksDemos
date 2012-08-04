#pragma once

#include "ofMain.h"

class BulgeEffect {
public:
	void setup(int width, int height, int resolution = 512);
	void addBulge(float x, float y, float side);
	void draw(ofTexture& tex, int x, int y);

private:
	void clearBulges();
	bool needToClear;

	int resolution;
	ofShader blendBulgeShader, applyBulgeShader;
	ofFbo bulge, blendBulge;
};
