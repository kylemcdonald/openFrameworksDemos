#include "ofMain.h"

float sinnp(float x) {
    return (sin(x * TWO_PI) + 1) / 2;
}

class Oscillator {
private:
    float offset = 0;
    float fadeInTime = 0;
public:
    int x, y;
    float startTime = 0;
    float speed = 1;
    Oscillator(int x, int y) : x(x), y(y) {
        offset = ofRandom(1);
        fadeInTime = ofRandom(.5, 2);
    }
    void draw(float time) {
        float alpha = MIN(1, (time - startTime) / fadeInTime);
        float localTime = time + offset;
        float brightness = sinnp(localTime * speed);
        ofSetColor(brightness * 255, alpha * 255);
        ofDrawRectangle(x, y, 1, 1);
    }
};

class OscillatorGrid {
public:
    int width, height;
    vector<Oscillator> inactive, active;
    void setup(int width, int height) {
        this->width = width;
        this->height = height;
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                inactive.push_back(Oscillator(x, y));
            }
        }
        ofRandomize(inactive);
    }
    bool exhausted() {
        return inactive.empty();
    }
    bool poke(float time, float speed) {
        if(exhausted()) {
            return false;
        }
        active.push_back(inactive.back());
        inactive.pop_back();
        active.back().startTime = time;
        active.back().speed = speed;
        return true;
    }
    void draw(float time) {
        for(int i = 0; i < active.size(); i++) {
            active[i].draw(time);
        }
    }
};

class OscillatorStack {
public:
    vector<OscillatorGrid> grids;
    void setup(int levels) {
        grids.resize(levels);
        for(int i = 0; i < levels; i++) {
            int n = 1 << i;
            grids[i].setup(n, n);
        }
    }
    int exhaustion() {
        for(int i = 0; i < grids.size(); i++) {
            if(!grids[i].exhausted()) {
                return i;
            }
        }
        return grids.size();
    }
    bool exhausted() {
        return exhaustion() == grids.size();
    }
    bool poke(float time, float speed) {
        for(int i = 0; i < grids.size(); i++) {
            if(grids[i].poke(time, speed)) {
                return true;
            }
        }
        return false;
    }
    void draw(float time) {
        ofPushMatrix();
        for(int i = 0; i < grids.size(); i++) {
            grids[i].draw(time);
            ofScale(.5, .5);
        }
        ofPopMatrix();
    }
};

class ofApp : public ofBaseApp {
public:
    OscillatorStack ost;
    void setup() {
        ost.setup(6);
    }
    void update() {
        float heartRate = ofRandom(55, 95); // heartRate bpm goes here
        
        float speed = 60 / heartRate;
        float time = ofGetElapsedTimef();
        if(!ost.exhausted()) {
            // pokes per second is based on how deep we are
            float pps = 1 << ost.exhaustion();
            // pokes per second increases exponentially
            pps *= pps;
            // but we slow it down a bit
            pps *= .5;
            // pokes per frame is based on the framerate
            float ppf = pps / ofGetFrameRate();
            // if pokes per frame is less than 1
            if(ppf < 1) {
                // we poke probablistically
                if(ofRandom(1) < ppf) {
                    ost.poke(time, speed);
                }
            } else {
                // otherwise we poke ppf times
                for(int i = 0; i < ppf; i++) {
                    ost.poke(time, speed);
                }
            }
        }
    }
    void draw() {
        int width = ofGetWidth();
        int height = ofGetHeight();
        int side = MAX(width, height);
        // center
        ofTranslate((width - side) / 2, (height - side) / 2);
        ofScale(side, side);
        ost.draw(ofGetElapsedTimef());
    }
};

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}