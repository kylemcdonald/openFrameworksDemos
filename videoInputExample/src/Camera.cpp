#include "Camera.h"

videoInput vi;

Camera::Camera()
	:deviceId(0)
	,ready(false)
	,previous(0)
	,width(0)
	,height(0)
	,horizontalFlip(false) {
}

Camera::~Camera() {
	vi.stopDevice(deviceId);
}

void Camera::setHorizontalFlip(bool horizontalFlip) {
	this->horizontalFlip = horizontalFlip;
}

void Camera::setDeviceId(int deviceId) {
	this->deviceId = deviceId;
}

bool Camera::initGrabber(int width, int height) {
	this->width = width;
	this->height = height;
	int numDevices = vi.listDevices();
	if(deviceId >= numDevices) {
		return false;
	}
	vi.setupDevice(deviceId, width, height);
	this->width = vi.getWidth(deviceId);
	this->height = vi.getHeight(deviceId);
	img.allocate(this->width, this->height, OF_IMAGE_COLOR);
	img.update();
	return true;
}

bool Camera::update() {
	bool newFrame = vi.isFrameNew(deviceId);
	if(newFrame) {
		vi.getPixels(deviceId, img.getPixels(), true, true);
		if(!ready) {
			unsigned long cur = ((long*) img.getPixels())[0];
			ready = previous != 0 && cur != previous;
			previous = cur;
		}
		if(ready) {
			if(horizontalFlip) {
				img.mirror(false, true);
			}
			img.update();
		} else {
			newFrame = false;
		}
	}
	return newFrame;
}

int Camera::getWidth() const {
	return width;
}

int Camera::getHeight() const {
	return height;
}

bool Camera::getReady() const {
	return ready;
}

unsigned char* Camera::getPixels() {
	return img.getPixels();
}

ofPixels& Camera::getPixelsRef() {
	return img.getPixelsRef();
}

ofTexture& Camera::getTextureReference() {
	return img.getTextureReference();
}

void Camera::draw(int x, int y) {
	img.draw(x, y);
}
