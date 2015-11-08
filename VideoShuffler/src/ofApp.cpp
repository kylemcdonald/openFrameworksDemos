#include "ofApp.h"

ofColor cyanPrint = ofColor::fromHex(0x00abec), magentaPrint = ofColor::fromHex(0xec008c), yellowPrint = ofColor::fromHex(0xffee00);

void centerFitInside(float width, float height) {
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	float scale;
	if(width / ofGetWidth() > height / ofGetHeight()) {
		scale = ofGetWidth() / width;
	} else {
		scale = ofGetHeight() / height;
	}
	ofScale(scale, scale);
	ofTranslate(-width / 2, -height / 2);
}

ofRectangle addPadding(ofRectangle rect, float padding) {
	return ofRectangle(rect.x - padding, rect.y - padding, rect.width + 2 * padding, rect.height + 2 * padding);
}

void ofApp::setup() {
	ofSetVerticalSync(true);
	
	ofxXmlSettings xml;
	
	xml.loadFile("settings.xml");
	timer.setPeriod(xml.getValue("period", 30));
	h1.loadFont("GravuConBol", xml.getValue("h1Size", 36), true);
	h2.loadFont("GravuConLig", xml.getValue("h2Size", 28), true);
	
	xml.loadFile("videos.xml");
	int n = xml.getNumTags("video");
	for(int i = 0; i < n; i++) {
		xml.pushTag("video", i);
		Description cur;
		cur.file = xml.getValue("file", "");
		cur.h1 = xml.getValue("h1", "");
		cur.h2 = xml.getValue("h2", "");
		descriptions.push_back(cur);
		xml.popTag();
		
		playlist.push_back(i);
	}
	
	which = 0;
	
	skip = true;
	
	ofHideCursor();
}

void ofApp::update() {
	if(timer.tick() || skip) {
		cur = descriptions[playlist[which]];
		cout << "loading " << cur.file << endl;
		video.loadMovie("videos/" + cur.file, OFXQTVIDEOPLAYER_MODE_TEXTURE_ONLY);
		video.play();
		float duration = video.getDuration();
		float time = ofClamp(ofRandom(0, duration - timer.getPeriod()), 0, duration - timer.getPeriod());
		video.setPosition(time / duration);
		which++;
		if(which == playlist.size()) {
			ofRandomize(playlist);
			which = 0;
		}
		skip = false;
	}
	
	video.update();
}
void ofApp::draw() {
	ofBackground(0);
	
	ofPushMatrix();
	centerFitInside(video.getWidth(), video.getHeight());
	ofSetColor(255);
	video.draw(0, 0);
	ofPopMatrix();
	
	ofSetColor(255);
	
	float padding = 8, margin = 16;
	
	ofRectangle titleBox = addPadding(h1.getStringBoundingBox(cur.h1, 0, 0), padding);
	ofRectangle authorBox = addPadding(h2.getStringBoundingBox(cur.h2, 0, 0), padding);
	
	ofFill();
	
	ofTranslate(margin, margin + titleBox.height);
	ofSetColor(cyanPrint);
	ofRect(titleBox);
	ofSetColor(255);
	h1.drawString(cur.h1, 0, 0);
	
	ofTranslate(0, titleBox.height + padding);
	ofSetColor(magentaPrint);
	ofRect(authorBox);
	ofSetColor(255);
	h2.drawString(cur.h2, 0, 0);
}

void ofApp::keyPressed(int key) {
	if(key == 'f') {
		ofToggleFullscreen();
	}
	if(key == ' ') {
		skip = true;
	}
}
