#pragma once

#include "Force.h"
#include "Particle.h"

void drawArrow(ofVec2f begin, ofVec2f end) {
	ofLine(begin, end);
	ofVec2f side = begin - end;
	side.normalize();
	side *= 4;
	side.rotate(45);
	ofLine(end, end + side);
	side.rotate(-90);
	ofLine(end, end + side);
}

ForceLine::ForceLine(ofVec2f begin, ofVec2f end, float radius, float magnitude, float minRadius)
	:begin(begin)
	,end(end)
	,radius(radius)
	,magnitude(magnitude)
	,minRadius(minRadius) {
}

ofVec2f ForceLine::getForce(Particle& particle) const {
	ofVec2f line = end - begin;
	const ofVec2f& p = particle.getPosition();
	float u = ((p.x - begin.x) * (end.x - begin.x) + (p.y - begin.y) * (end.y - begin.y)) / line.lengthSquared();
	if(u < 0 || u > 1) {
		return ofVec2f(0);
	}
	ofVec2f inter = begin.getInterpolated(end, u);
	ofVec2f diff = p - inter;
	float length = diff.length();
	if(length < minRadius || length > radius) {
		return ofVec2f(0);
	} else {
		diff.normalize();
		diff *= magnitude / length;
		return diff;
	}
}

void ForceLine::draw() const {
	ofLine(begin, end);
	ofVec2f side = end - begin;
	side.rotate(90);
	side.normalize();
	side *= radius;
	ofPushStyle();
	ofSetColor(128);
	for(int i = 0; i < 8; i++) {
		ofVec2f cur = begin.getInterpolated(end, ofMap(i, 0, 7, 0, 1));
		if(magnitude > 0) {
			drawArrow(cur, cur + side);
			drawArrow(cur, cur - side);
		} else {
			drawArrow(cur + side, cur);
			drawArrow(cur - side, cur);
		}
	}
	ofPopStyle();
}

ForceCircle::ForceCircle(float x, float y, float radius, float magnitude, float minRadius)
	:center(ofVec2f(x, y))
	,radius(radius)
	,magnitude(magnitude)
	,minRadius(minRadius) {
}

ofVec2f ForceCircle::getForce(Particle& particle) const {
	ofVec2f diff = particle.getPosition() - center;
	float length = diff.length();
	if(length < minRadius || length > radius) {
		return ofVec2f(0);
	} else {
		diff.normalize();
		diff *= magnitude / length;
		return diff;
	}
}

void ForceCircle::draw() const {
	ofPushMatrix();
	ofTranslate(center);
	ofPushStyle();
	ofNoFill();
	ofCircle(0, 0, radius);
	ofSetColor(128);
	for(int i = 0; i < 8; i++) {
		ofPushMatrix();
		ofRotate(ofMap(i, 0, 8, 0, 360));
		drawArrow(ofVec2f(radius, 0), ofVec2f(radius + (magnitude > 0 ? +8 : -8), 0));
		ofPopMatrix();
	}
	ofPopStyle();
	ofPopMatrix();
}

ForceFriction::ForceFriction(float magnitude)
	:magnitude(magnitude) {
}

ofVec2f ForceFriction::getForce(Particle& particle) const {
	return magnitude * particle.getVelocity();
}

FieldDonut::FieldDonut(float x, float y, float radius, float magnitude, float minRadius)
	:center(ofVec2f(x, y))
	,radius(radius)
	,magnitude(magnitude)
	,minRadius(minRadius) {
}

ofVec2f FieldDonut::getForce(Particle& particle) const {
	ofVec2f diff = particle.getPosition() - center;
	float length = diff.length() - radius;
	if(fabsf(length) < minRadius) {
		return ofVec2f(0);
	} else {
		diff.normalize();
		diff *= length * magnitude;
		return diff;
	}
}

void FieldDonut::draw() const {
	ofPushMatrix();
	ofTranslate(center);
	ofPushStyle();
	ofNoFill();
	ofCircle(0, 0, radius);
	ofSetColor(128);
	for(int i = 0; i < 8; i++) {
		ofPushMatrix();
		ofRotate(ofMap(i, 0, 8, 0, 360));
		ofVec2f cur(radius, 0), side((magnitude > 0 ? +8 : -8), 0);
		if(magnitude > 0) {
			drawArrow(cur, cur + side);
			drawArrow(cur, cur - side);
		} else {
			drawArrow(cur + side, cur);
			drawArrow(cur - side, cur);
		}
		ofPopMatrix();
	}
	ofPopStyle();
	ofPopMatrix();
}


FieldLine::FieldLine(ofVec2f begin, ofVec2f end, float magnitude, float minRadius)
	:begin(begin)
	,end(end)
	,magnitude(magnitude)
	,minRadius(minRadius) {
}

ofVec2f FieldLine::getForce(Particle& particle) const {
	ofVec2f line = end - begin;
	const ofVec2f& p = particle.getPosition();
	float u = ((p.x - begin.x) * (end.x - begin.x) + (p.y - begin.y) * (end.y - begin.y)) / line.lengthSquared();
	ofVec2f inter = begin.getInterpolated(end, u);
	ofVec2f diff = p - inter;
	float length = diff.length();
	if(length < minRadius) {
		return ofVec2f(0);
	} else {
		diff.normalize();
		diff *= length * magnitude;
		return diff;
	}
}

void FieldLine::draw() const {
	ofLine(begin, end);
	ofVec2f side = end - begin;
	side.rotate(90);
	side.normalize();
	side *= 32;
	ofPushStyle();
	ofSetColor(128);
	for(int i = 0; i < 8; i++) {
		ofVec2f cur = begin.getInterpolated(end, ofMap(i, 0, 7, 0, 1));
		if(magnitude > 0) {
			drawArrow(cur, cur + side);
			drawArrow(cur, cur - side);
		} else {
			drawArrow(cur + side, cur);
			drawArrow(cur - side, cur);
		}
	}
	ofPopStyle();
}