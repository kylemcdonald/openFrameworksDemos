#pragma once

#include "ofMain.h"
#include "PlotUtilities.h"

class LinePlot {
private:
    ofVboMesh polyline;
    int counter = 0;
    float min = 0, max = 0;
    bool minmaxSet = false;
    bool valid(float data) {
        return data == data;
    }

public:
    LinePlot() {
        polyline.setMode(OF_PRIMITIVE_LINE_STRIP);
    }
    void add(float data) {
        if(valid(data)) {
            if(minmaxSet) {
                min = MIN(min, data);
                max = MAX(max, data);
            } else {
                min = data, max = data;
                minmaxSet = true;
            }
            polyline.addVertex(ofVec2f(counter, data));
        }
        counter++;
    }
    template <class T>
    void set(const vector<T>& data) {
        for(int i = 0; i < data.size(); i++) {
            add(data[i]);
        }
    }
    unsigned int size() {
        return polyline.getNumVertices();
    }
    void draw(float w, float h) {
        float xr = counter;
        float yr = max - min;
        ofPushStyle();
        ofPushMatrix();
        ofNoFill();
        scaleAndTranslate(ofRectangle(0, min, xr, yr), ofRectangle(0, h, w, -h));
        polyline.setMode(OF_PRIMITIVE_LINE_STRIP);
        polyline.draw();
        polyline.setMode(OF_PRIMITIVE_POINTS);
        glPointSize(3);
        polyline.draw();
        ofSetColor(ofGetStyle().color, 128);
        ofDrawLine(0, 0, size(), 0);
        ofDrawLine(0, 1, size(), 1);
        ofDrawLine(0, -1, size(), -1);
        ofPopMatrix();
        ofPopStyle();
    }
};
