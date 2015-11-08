#pragma once

#include "ofMain.h"

inline void scaleAndTranslate(const ofRectangle& from, const ofRectangle& to) {
    ofTranslate(to.x, to.y);
    ofScale(to.width, to.height);
    ofScale(1. / from.width, 1. / from.height);
    ofTranslate(-from.x, -from.y);
}