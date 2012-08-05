#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);

	camWidth = 640;
	camHeight = 360;
	count = 0;
	
	startPreview();
}  

void ofApp::update() {
	if(mode == PREVIEW || mode == RECORD) {
		grabber.update();
	} else {
		player.update();
	}
}

void ofApp::draw() {
	ofBackground(0);
	ofSetColor(255);
	if(mode == PREVIEW || mode == RECORD) {
		grabber.draw(0, 0);
	} else {
		player.draw(0, camHeight);
	}
	ofDrawBitmapString(stringify(mode), 10, 20);
}

void ofApp::startPreview() {
	mode = PREVIEW;
	grabber.listVideoCodecs();
	grabber.initGrabber(camWidth, camHeight, true);
}

void ofApp::stopPreview() {
}

void ofApp::startRecord() {
	mode = RECORD;
	grabber.initRecording();
	grabber.setVideoCodec("QTCompressionOptionsSD480SizeMPEG4Video");
	grabber.startRecording(ofToDataPath(ofToString(count) + ".mov"));
}

void ofApp::stopRecord() {
	grabber.stopRecording();
	grabber.close();
	count++;
}

void ofApp::startPlayback() {
	mode = PLAYBACK;
	player.loadMovie(ofToString(count - 1) + ".mov");
	player.setLoopState(OF_LOOP_NONE);
	player.play();
}

void ofApp::stopPlayback() {
	player.close();
}

void ofApp::keyPressed(int key) {
	if(key == ' ') {
		switch(mode) {
			case PREVIEW: stopPreview(); startRecord(); break;
			case RECORD: stopRecord(); startPlayback(); break;
			case PLAYBACK: stopPlayback(); startPreview(); break;
		}
	}
}