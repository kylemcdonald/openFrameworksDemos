#include "ofMain.h"

#include "PlotUtilities.h"
#include "LinePlot.h"
#include "ZoomableRegion.h"
#include "Markers.h"

// 5th order high pass butterworth filter at 1.5 Hz from mkfilter 60Hz samplerate
#define NZEROS 5
#define NPOLES 5
#define GAIN   1.289795686e+00
static float highPassFilter(float input) {
    static float xv[NZEROS+1], yv[NPOLES+1];
    static float runningLowAmp = 0;
    static float runningHighAmp = 0;
    xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5];
    xv[5] = input / GAIN;
    yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5];
    yv[5] =   (xv[5] - xv[0]) + 5 * (xv[1] - xv[4]) + 10 * (xv[3] - xv[2])
    + (  0.6011158229 * yv[0]) + ( -3.3110475620 * yv[1])
    + (  7.3120812802 * yv[2]) + ( -8.0940554178 * yv[3])
    + (  4.4918309651 * yv[4]);
    return yv[5];
}

static float lerp(float x, float y, float t) {
    return x*(1-t) + y*(t);
}

struct agcDoubleFilter{
    float smoothing = .001;
    float min = 0;
    float max = 0;
    float run(float x) {
        if(x <= min) {
            min = x;
        } else {
            min = lerp(min, x, smoothing);
        }
        if(x >= max) {
            max = x;
        } else {
            max = lerp(max, x, smoothing);
        }
        return (x - min) / (max - min);
    }
};

struct agcSingleFilter{
    float smoothingRising = .9;
    float smoothingFalling = .002;
    float max = 0;
    float run(float x) {
        if(x >= max) {
            max = lerp(max, x, smoothingRising);
        } else {
            max = lerp(max, x, smoothingFalling);
        }
        return x / max;
    }
};

struct derivativeFilter {
    float px = 0;
    float run(float x) {
        float cur = x - px;
        px = x;
        return cur;
    }
};

float unsignedToSigned(float x) {
    return (x * 2) - 1;
}

// peak filter triggers immediately but is rate limited
static bool rateLimitedPeakFilter(bool input) {
    static long previousPeak = 0;
    static const long rateLimit = 20; // 180 bpm at 60 Hz (60 * 60 / 180)
    if(input && previousPeak > rateLimit) {
        previousPeak = 0;
        return true;
    } else {
        previousPeak++;
        return false;
    }
}

const float minBpm = 30, maxBpm = 150;
bool rangeFilter(float input) {
    static const int count = 3;
    static float recent[count];
    static const float range = 2.;
    if(input < minBpm || input > maxBpm) {
        return false;
    }
    int inRange = 0;
    for(int i = 0; i < count; i++) {
        if(recent[i] / range < input && recent[i] * range > input) {
            inRange++;
        }
        recent[i] = recent[i + 1];
    }
    recent[count - 1] = input;
    return (inRange + 1 >= count);
}
float bpmFilter(unsigned long timeMs) {
    static unsigned long previousTimeMs = 0;
    float curBpm = (60. * 1000.) / (timeMs - previousTimeMs);
    previousTimeMs = timeMs;
    return curBpm;
}

bool heartbeatFilter(float x, unsigned long timeMs) {
    static derivativeFilter derivativea, derivativeb;
    static agcDoubleFilter agca;
    static  agcSingleFilter agcb;
    static float smoothBpm = 60;
    static float prevBpm = 60;
    if(x != x) {
        return false;
    }
    x = highPassFilter(x);
    x = agca.run(x);
    x = unsignedToSigned(x);
    x = derivativea.run(x);
    x = derivativeb.run(x);
    x = abs(x);
    x = agcb.run(x);
    x = rateLimitedPeakFilter(x > .5);
    bool beat = false;
    if(x) {
        float filteredBpm = bpmFilter(timeMs);
        if(rangeFilter(filteredBpm)) {
            x = filteredBpm;
            prevBpm = filteredBpm;
            beat = true;
        } else {
            x = prevBpm;
        }
    } else {
        x = prevBpm;
    }
    smoothBpm = lerp(smoothBpm, x, .005);
    return beat;
//    return smoothBpm;
}

void analyzeHeartRate(vector<float>& data, vector<float>& clean, vector<float>& analysis) {
    derivativeFilter derivativea, derivativeb;
    agcDoubleFilter agca;
    agcSingleFilter agcb;
    clean.resize(data.size());
    analysis.resize(data.size());
    float smoothBpm = 60;
    float prevBpm = 60;
    for(int i = 0; i < data.size(); i++) {
        float x = data[i];
        float strength;
        clean[i] = x;
        x = highPassFilter(x);
        x = agca.run(x);
        x = unsignedToSigned(x);
        x = derivativea.run(x);
        x = derivativeb.run(x);
        x = abs(x);
        x = agcb.run(x);
        x = rateLimitedPeakFilter(x > .5);
        if(x) {
            unsigned long timeMs = i * 1000 / 60.;
            float y = bpmFilter(timeMs);
            if(rangeFilter(y)) {
                ofLog() << (int) y << " bpm";
                x = y;
                prevBpm = y;
            } else {
                ofLog() << (int) y << " bpm (ignored)";
                x = prevBpm;
            }
        } else {
            if(i == 0) x = 60;
            else x = prevBpm;
        }
        smoothBpm = lerp(smoothBpm, x, .005);
        analysis[i] = smoothBpm;
    }
}

Markers buildHeartRateMarkers(vector<float>& data) {
    Markers result;
    for(int i = 0; i < data.size(); i++) {
        if(heartbeatFilter(data[i], i * 1000 / 60.)) {
            result.addMarker(i);
        }
    }
    return result;
}

class ofApp : public ofBaseApp {
public:
    vector<float> data;
    LinePlot clean, analysis;
    ZoomableRegion zoomable;
    Markers markers;
    
    void setup() {
        ofBackground(0);
        loadData("kyle-yun.csv");
    }
    void draw() {
        zoomable.begin(ofGetWidth(), ofGetHeight());
        ofSetColor(255, 255, 0);
        markers.draw(clean.size(), ofGetWidth(), ofGetHeight());
        analysis.draw(ofGetWidth(), ofGetHeight());
        ofSetColor(255);
        clean.draw(ofGetWidth(), ofGetHeight());
        zoomable.end();
    }
    void loadData(string filename) {
        ofBuffer buffer = ofBufferFromFile(filename);
        data = vector<float>();
        for(auto line : buffer.getLines()) {
            data.push_back(ofToFloat(line));
        }
        markers = buildHeartRateMarkers(data);
        vector<float> cleanData, analysisData;
        analyzeHeartRate(data, cleanData, analysisData);
        clean = LinePlot();
        clean.set(cleanData);
        analysis = LinePlot();
        analysis.set(analysisData);
    }
    void dragEvent(ofDragInfo dragInfo) {
        if(dragInfo.files.size()) {
            loadData(dragInfo.files.front());
        }
    }
};
int main() {
    ofSetupOpenGL(1024, 512, OF_WINDOW);
    ofSetWindowShape(2048, 512);
    ofRunApp(new ofApp());
}