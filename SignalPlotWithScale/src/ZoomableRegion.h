#pragma once

#include "ofMain.h"

class ZoomableRegion {
private:
    float speed = 5;
    float zoom = 1, offset = 1;
    float startZoom, startOffset;
    ofVec2f startDrag;
    float width, height;
public:
    ZoomableRegion() {
        ofAddListener(ofEvents().mouseDragged , this, &ZoomableRegion::mouseDragged);
        ofAddListener(ofEvents().mousePressed, this, &ZoomableRegion::mousePressed);
    }
    float unapply(float x) {
        return (x / zoom) - offset;
    }
    void begin(float width, float height) {
        this->width = width, this->height = height;
        float x = ofGetMouseX();
        ofPushMatrix();
        ofScale(zoom, 1);
        ofTranslate(offset, 0);
    }
    void end() {
        ofPopMatrix();
    }
    void mouseDragged(ofMouseEventArgs& args) {
        ofVec2f diff = args - startDrag;
        zoom = startZoom * (1 + (exp((speed * diff.y) / height) - 1));
        zoom = MAX(zoom, 1);
        offset = startOffset + (diff.x / zoom);
        float center = (startDrag.x) * (1 - zoom) / zoom;
        offset += center;
        offset = ofClamp(offset, width * (1 - zoom) / zoom, 0);
    }
    void mousePressed(ofMouseEventArgs& args) {
        startDrag = args;
        startZoom = zoom;
        float center = (args.x) * (1 - zoom) / zoom;
        startOffset = offset - center;
    }
};