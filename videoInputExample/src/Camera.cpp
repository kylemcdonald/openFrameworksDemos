#include "Camera.h"

videoInput vi;

Camera::Camera()
	:deviceId(0)
	,width(0)
	,height(0) {
}

Camera::~Camera() {
	vi.stopDevice(deviceId);
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
	width = vi.getWidth(deviceId);
	height = vi.getHeight(deviceId);
	img.allocate(width, height, OF_IMAGE_COLOR);
	return true;
}

bool Camera::update() {
	bool newFrame = vi.isFrameNew(deviceId);
	if(newFrame) {
		vi.getPixels(deviceId, img.getPixels(), true, true);
		img.update();
	}
	return newFrame;
}

int Camera::getWidth() const {
	return width;
}

int Camera::getHeight() const {
	return height;
}

ofPixels& Camera::getPixelsRef() {
	return img.getPixelsRef();
}

void Camera::draw(int x, int y) {
	img.draw(x, y);
}
