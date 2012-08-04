#include "BulgeEffect.h"

void BulgeEffect::setup(int width, int height, int resolution) {
	this->resolution = resolution;
	ofShader generateBulgeShader;
	generateBulgeShader.load("", "generateBulge.fs");
	ofFbo::Settings settings;
	settings.width = resolution, settings.height = resolution;
	settings.internalformat = GL_RGB32F;
	bulge.allocate(settings);
	bulge.begin();
	generateBulgeShader.begin();
	generateBulgeShader.setUniform1f("scale", resolution / 2);
	generateBulgeShader.setUniform1f("shape", 1.5);
	ofRect(0, 0, resolution, resolution);
	generateBulgeShader.end();
	bulge.end();
	
	blendBulgeShader.load("", "blendBulge.fs");
	settings.width = width, settings.height = height;
	blendBulge.allocate(settings);
	
	applyBulgeShader.load("", "applyBulge.fs");

	needToClear = true;
}

void BulgeEffect::addBulge(float x, float y, float side) {
	if(needToClear) {
		clearBulges();
	}

	blendBulge.begin();
	blendBulgeShader.begin();
	blendBulgeShader.setUniform1f("scale", side / resolution);
	bulge.setAnchorPercent(.5, .5);
	bulge.draw(x, y, side, side);
	blendBulgeShader.end();
	blendBulge.end();
}

void BulgeEffect::clearBulges() {
	blendBulgeShader.begin();
	blendBulgeShader.setUniformTexture("base", blendBulge.getTextureReference(), 1);
	blendBulge.begin();
	ofClear(128, 128, 0);	
	blendBulgeShader.setUniformTexture("tex", bulge.getTextureReference(), 2);
	blendBulge.end();
	blendBulgeShader.end();
	needToClear = false;
}

void BulgeEffect::draw(ofTexture& tex, int x, int y) {
	applyBulgeShader.begin();
	applyBulgeShader.setUniformTexture("bulge", blendBulge.getTextureReference(), 1);
	applyBulgeShader.setUniformTexture("tex", tex, 2);
	tex.draw(x, y);
	applyBulgeShader.end();
	needToClear = true;
}