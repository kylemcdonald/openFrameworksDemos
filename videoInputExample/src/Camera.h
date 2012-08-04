#pragma once

#include "ofMain.h"

class Camera : public ofBaseHasPixels {
protected:
	int deviceId, width, height;
	ofImage img;
	bool horizontalFlip;
	bool ready;
	unsigned long previous;

public:
	Camera();
	~Camera();

	void setDeviceId(int deviceId);
	void setHorizontalFlip(bool horizontalFlip);
	bool initGrabber(int width, int height);
	bool update();
	int getWidth() const;
	int getHeight() const;
	bool getReady() const;
	unsigned char* getPixels();
	ofPixels& getPixelsRef();
	ofTexture& getTextureReference();
	void draw(int x, int y);
};
