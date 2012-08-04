#pragma once

#include "Particle.h"
#include "Force.h"

Particle::Particle(float x, float y)
	:position(ofVec2f(x, y))
	,maxVelocity(1) {
}

void Particle::addForce(ofPtr<Force> force) {
	forces.push_back(force);
}

void Particle::update(float timeStep) {
	force.set(0);
	for(int i = 0; i < forces.size(); i++) {
		force += forces[i]->getForce(*this);
	}
	velocity += force * timeStep;
	velocity.limit(maxVelocity);
	position += velocity * timeStep;
}

const ofVec2f& Particle::getPosition() const {
	return position;
}

const ofVec2f& Particle::getVelocity() const {
	return velocity;
}

void Particle::draw() const {
	ofCircle((int) position.x, (int) position.y, 1);
}

void Particle::setMaxVelocity(float maxVelocity) {
	this->maxVelocity = maxVelocity;
}