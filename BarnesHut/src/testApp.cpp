#include "testApp.h"

void testApp::setup(){
	ofSetBackgroundAuto(false);
	particleSystem.clear();
	kParticles = 10;
	float size = 256;
	
	particleSystem.setExact(true);
	/*
	float velocity = 1;
	for(int i = 0; i < kParticles; i++) {
		float x = ofRandom(-size, size);
		float y = ofRandom(-size, size);
		float theta = ofRandom(-PI, PI);
		float xv = cos(theta) * velocity;
		float yv = sin(theta) * velocity;
		float mass = ofRandom(.5, 1);
		Particle particle(x, y, mass, xv, yv);
		particleSystem.add(particle);
	}
	particleSystem.setGravitationalConstant(10);
	particleSystem.setTimeStep(.01);
	*/
	/*
	for(int i = 0; i < kParticles; i++)  {
		float x = ofMap(i, 0, kParticles, -size, size);
		float y = 0;
		float xv = 0;
		float yv = ofMap(i, 0, kParticles, -.1, .1);
		float mass = 2;// ofMap(i, 0 , kParticles, 1, 1);
		Particle particle(x, y, mass, xv, yv);
		particleSystem.add(particle);
	}
	particleSystem.setGravitationalConstant(.01);
	*/
	/*
	//Particle sun(0, 0, 1.9891e+30, 0, 0, 6.955e8);
	//particleSystem.add(sun);
	for(int i = 0; i < 10; i++) {
		float distance = ofRandom(0, 2.324e+11);
		float dtheta = ofRandom(-PI, PI);
		float x = cos(dtheta) * distance;
		float y = sin(dtheta) * distance;
		
		float mass = ofRandom(3.3022e+23, 1.9891e+28);
		
		float velocity = ofRandom(0, 2.7362e+4);
		float vtheta = ofRandom(-PI, PI);
		float xv = cos(vtheta) * velocity;
		float yv = sin(vtheta) * velocity;
		Particle planet(x, y, mass, xv, yv);
		particleSystem.add(planet);
	}
	particleSystem.setTimeStep(60 * 60);
	particleSystem.setGravitationalConstant(6.67384e-11);
	*/
	
	Particle sun(0, 0, 1.9891e+30, 0, 0, 6.955e8);
	Particle mercury(4.625e+10, 0, 3.3022e23, 0, 4.7362e+4, 2.4397e6);
	Particle venus(1.078e+11, 0, 4.869e24, 0, 3.5020e+4, 6.0519e6);
	Particle earth(1.506e+11, 0, 5.976e24, 0, 2.98e+4, 6.367e6);
	Particle mars(2.324e+11, 0, 6.4191e23, 0, 2.4077e+4, 3.386e6);
	particleSystem.add(sun);
	particleSystem.add(mercury);
	particleSystem.add(venus);
	particleSystem.add(earth);
	particleSystem.add(mars);
	particleSystem.setTimeStep(60 * 60);
	particleSystem.setGravitationalConstant(6.67384e-11);
	
	
	particleSystem.setFriction(1);

	ofBackground(0, 0, 0, 255);
}

void testApp::update(){
	particleSystem.update();
}

void testApp::draw(){
	ofPushMatrix();
	ofEnableAlphaBlending();
	ofSetColor(255, 1);
	ofFill();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	particleSystem.draw();
	ofDisableAlphaBlending();
	ofPopMatrix();

	ofDrawBitmapString(ofToString(kParticles) + " particles", 32, 32);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 32, 52);
}

void testApp::keyPressed(int key){
	if(key == 'p')
		ofSaveFrame();
	if(key == 'r')
		setup();
}

void testApp::mousePressed(int x, int y, int button){
	float mass = ofRandom(1, 20);
	Particle particle(x, y, mass);
	particleSystem.add(particle);
}
