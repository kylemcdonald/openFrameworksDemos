#include "ofMain.h"

#include "PlotUtilities.h"
#include "LinePlot.h"
#include "ZoomableRegion.h"

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

#define random ofRandom
#define map ofMap

vector<float> hann(const vector<float>& x) {
    int n = x.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        float weight = .5 * (1 - cos((TWO_PI * i) / (n - 1)));
        result[i] = x[i] * weight;
    }
    return result;
}

vector<float> range(float start, float step, float end) {
    vector<float> result = vector<float>();
    for(float t = start; t < end; t += step) {
        result.push_back(t);
    }
    return result;
}

vector<float> range(float start, float end) {
    return range(start, 1, end);
}

vector<float> rand(int count) {
    vector<float> result = vector<float>(count);
    for(int i = 0; i < count; i++) {
        result[i] = random(1);
    }
    return result;
}

vector<float> mult(float x, const vector<float>& y) {
    int n = y.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        result[i] = x * y[i];
    }
    return result;
}

vector<float> dotasterisk(const vector<float>& x, const vector<float>& y) {
    int n = y.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        result[i] = x[i] * y[i];
    }
    return result;
}

vector<float> add(float x, const vector<float>& y) {
    int n = y.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        result[i] = x + y[i];
    }
    return result;
}

vector<float> add(const vector<float>& x, const vector<float>& y) {
    int n = y.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        result[i] = x[i] + y[i];
    }
    return result;
}

vector<float> interp1(const vector<float>& x, const vector<float>& v, const vector<float>& xq) {
    int n = xq.size();
    int m = x.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        float curq = xq[i];
        int prej = 0, curj = 0;
        float prex = x[0], curx = x[0];
        for(int j = 1; j < m; j++) {
            curj = j;
            curx = x[j];
            if(prex <= curq && curx > curq) {
                break;
            } else {
                prej = curj;
                prex = curx;
            }
        }
        if(prex == curx) {
            result[i] = v[prej];
        } else {
            result[i] = map(curq, prex, curx, v[prej], v[curj]);
        }
    }
    return result;
}

vector<float> cos(const vector<float>& x) {
    int n = x.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        result[i] = cos(x[i]);
    }
    return result;
}

vector<float> sin(const vector<float>& x) {
    int n = x.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        result[i] = cos(x[i]);
    }
    return result;
}

float sum(const vector<float>& x) {
    float sum = 0;
    int n = x.size();
    for(int i = 0; i < n; i++) {
        sum += x[i];
    }
    return sum;
}

float mean(const vector<float>& x) {
    return sum(x) / x.size();
}

vector<float> sq(const vector<float>& x) {
    int n = x.size();
    vector<float> result = vector<float>(n);
    for(int i = 0; i < n; i++) {
        result[i] = x[i] * x[i];
    }
    return result;
}

float cov(const vector<float>& x) {
    float ssd = sum(sq(add(-mean(x), x)));
    return ssd / x.size();
}

float stddev(const vector<float>& x) {
    return sqrt(cov(x));
}

vector<float> lomb(const vector<float>& t, const vector<float>& y, const vector<float>& freq) {
    int nfreq = freq.size();
    float fmax = freq[nfreq - 1];
    float fmin = freq[0];
    vector<float> power = vector<float>(nfreq);
    vector<float> f4pi = mult(4*PI, freq);
    float var = cov(y);
    vector<float> yn = add(-mean(y), y);
    for(int fi = 0; fi < nfreq; fi++) {
        float sinsum = sum(sin(mult(f4pi[fi], t)));
        float cossum = sum(cos(mult(f4pi[fi], t)));
        float tau = atan2(sinsum, cossum);
        vector<float> argu = mult(TWO_PI * freq[fi], add(-tau, t));
        vector<float> cosarg = cos(argu);
        float cfi = sum(dotasterisk(yn, cosarg));
        float cosnorm = sum(dotasterisk(cosarg, cosarg));
        vector<float> sinarg = sin(argu);
        float sfi = sum(dotasterisk(yn, sinarg));
        float sinnorm = sum(dotasterisk(sinarg, sinarg));
        power[fi] = (cfi*cfi/cosnorm+sfi*sfi/sinnorm)/(2*var);
    }
    return power;
}

vector<float> cleanHeartRate(vector<float>& data) {
    vector<float> result;
    for(int i = 0; i < data.size(); i++) {
        float x = data[i];
        x = highPassFilter(x);
        result.push_back(x);
    }
    return result;
}

float bpmToHz(float bpm) {
    return bpm / 60.;
}

class ofApp : public ofBaseApp {
public:
    vector<float> data;
    LinePlot plot, analysis;
    ZoomableRegion zoomable;
    
    void setup() {
        ofBackground(0);
        loadData("lauren-yun.csv");
    }
    void draw() {
        zoomable.begin(ofGetWidth(), ofGetHeight());
        ofSetColor(ofColor::yellow);
        analysis.draw(ofGetWidth(), ofGetHeight());
        ofSetColor(255);
        plot.draw(ofGetWidth(), ofGetHeight());
        zoomable.end();
    }
    void loadData(string filename) {
        ofBuffer buffer = ofBufferFromFile(filename);
        data = vector<float>();
        for(auto line : buffer.getLines()) {
            data.push_back(ofToFloat(line));
        }
        data = cleanHeartRate(data);
        plot = LinePlot();
        plot.set(data);
        
        int n = data.size();
        vector<float> t = mult(1./60, range(0, n));
        vector<float> freq = range(bpmToHz(40), bpmToHz(1), bpmToHz(80));
        vector<float> lombData = lomb(t, data, freq);
        analysis = LinePlot();
        analysis.set(lombData);
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
