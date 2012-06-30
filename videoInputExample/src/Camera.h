#pragma once

#include "ofMain.h"

class Camera {
protected:
	int deviceId, width, height;
	ofImage img;

public:
	Camera();
	~Camera();

	void setDeviceId(int deviceId);
	bool initGrabber(int width, int height);
	bool update();
	int getWidth() const;
	int getHeight() const;
	ofPixels& getPixelsRef();
	void draw(int x, int y);
};
