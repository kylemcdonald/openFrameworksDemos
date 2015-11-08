#include "ofMain.h"

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

class ofApp : public ofBaseApp {
public:
    vector<float> o18, power;
    
    void setup() {
        ofBackground(255);
        prep();
    }
    void prep() {
        int n = 600;
        vector<float> age = range(0, n);
        //  randomSeed(0);
        vector<float> ager = add(age, add(-.15, mult(0.3, rand(age.size()))));
        ager[0] = age[0]; ager[n - 1] = age[n - 1];
        vector<float> depth = mult(1/10., age);
        vector<float> bkg = interp1(range(0, 10, n), rand(n / 10), ager);
        float f1 = 1./mouseX, f2 = 1./125;
        vector<float> sig = add(cos(mult(TWO_PI*f1, ager)),
                                cos(add(PI, mult(TWO_PI*f2, ager))));
        o18 = add(sig, bkg);
//        o18 = hann(o18);
        // pick frequencies to evaluate spectral power
        vector<float> freq = range(0, 0.0001, 0.02);
        power = lomb(age, o18, freq);
        power[0] = 0;
        // normalize to average 1
        power = mult(1./stddev(power), power);
    }
    void update() {
    }
    void draw() {
        prep();
        ofNoFill();
        ofSetColor(0);
        
        float width = ofGetWidth();
        float height = ofGetHeight();
        
        // plot the results
        ofBeginShape();
        ofVertex(0, height);
        for(int i = 0; i < power.size(); i++) {
            float x = map(i, 0, power.size(), 0, width);
            float y = map(power[i], 0, 6, height, 0);
            ofVertex(x, y);
        }
        ofVertex(width, height);
        ofEndShape();
        
        ofBeginShape();
        for(int i = 0; i < o18.size(); i++) {
            float x = map(i, 0, o18.size(), 0, width);
            float y = map(o18[i], -10, 10, 0, height);
            ofVertex(x, y);
        }
        ofEndShape();
    }
};
int main() {
    ofSetupOpenGL(512, 512, OF_WINDOW);
    ofRunApp(new ofApp());
}
