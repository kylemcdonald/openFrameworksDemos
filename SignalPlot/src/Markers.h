#pragma once

#include "ofMain.h"

class Markers {
public:
    class Marker {
    public:
        float position;
        string name;
        Marker(float position = 0, string name = "")
        :position(position)
        ,name(name) {
        }
    };
    vector<Marker> markers;
    void addMarker(float position, string name = "") {
        markers.push_back(Marker(position, name));
    }
    int getRegion(float position) {
        int maxMarker = markers.size() - 1;
        for(int i = 0; i < markers.size(); i++) {
            if(markers[i].position > position) {
                maxMarker = i;
                break;
            }
        }
        return MAX(maxMarker - 1, 0);
    }
    void draw(float length, float w, float h) {
        if(length > 0) {
            ofPushMatrix();
            ofScale(w / length, 1);
            for(int i = 0; i < markers.size(); i++) {
                float x = markers[i].position;
                ofDrawLine(x, 0, x, h);
            }
            ofPopMatrix();
        }
    }
};