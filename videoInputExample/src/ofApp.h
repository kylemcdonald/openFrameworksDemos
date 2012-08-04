/*
 this is an example of using videoInput directly instead of
 going through ofDirectShowGrabber. the advantage is that
 videoInput doesn't rescale for non-native sizes.
*/

#pragma once

#include "ofMain.h"
#include "Camera.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	Camera cam;
};
