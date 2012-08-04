#pragma once

#include "BinnedParticleSystem.h"
#include "ofMain.h"

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	float timeStep;
	float particleNeighborhood, particleRepulsion;
	float centerAttraction;
	float padding;

	int kBinnedParticles;
	BinnedParticleSystem particleSystem;
	bool isMousePressed, slowMotion;
	
	bool drawBalls;
};
