#include "ofMain.h"

float sum(const vector<float>& x) {
    float sum = 0;
    for(int i = 0; i < x.size(); i++) {
        sum += x[i];
    }
    return sum;
}

vector<float> divide(const vector<float>& x, float y) {
    vector<float> out = x;
    for(int i = 0; i < out.size(); i++) {
        out[i] /= y;
    }
    return out;
}

vector<float> normalize(const vector<float>& x) {
    float summed = sum(x);
    return divide(x, summed);
}

vector<float> accumulate(const vector<float>& x) {
    vector<float> out = x;
    for(int i = 1; i < out.size(); i++) {
        out[i] += out[i - 1];
    }
    return out;
}

class StackedPlot {
private:
    list<vector<float> > history;
    vector<ofColor> colors;
public:
    int historyLength;
    
    StackedPlot()
    :historyLength(100) {
    }
    void clear() {
        history.clear();
    }
    void addData(const vector<float>& data) {
        if(size() && data.size() != dimensions()) {
            ofLogWarning("StackedPlot") << "added data does not have same size as existing data";
            clear();
        }
        history.push_back(data);
        while(history.size() > historyLength) {
            history.pop_front();
        }
    }
    void addColor(ofColor color) {
        colors.push_back(color);
    }
    int size() const {
        return history.size();
    }
    int dimensions() const {
        return size() ? history.front().size() : 0;
    }
    void draw(float width, float height) const {
        ofPushMatrix();
        ofPushStyle();
        int d = dimensions();
        vector<ofMesh> meshes(d);
        for(int i = 0; i < d; i++) {
            meshes[i].setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        }
        list<vector<float> >::const_iterator itr;
        int i;
        for(itr = history.begin(), i = 0; itr != history.end(); itr++, i++) {
            vector<float> normalized = normalize(*itr);
            vector<float> accumulated = accumulate(normalized);
            vector<float> all;
            all.push_back(0);
            all.insert(all.end(), accumulated.begin(), accumulated.end());
            all.push_back(1);
            for(int j = 0; j < d; j++) {
                meshes[j].addVertex(ofVec2f(i, all[j]));
                meshes[j].addVertex(ofVec2f(i, all[j + 1]));
            }
        }
        ofScale(width / (historyLength - 1), height);
        for(int i = 0; i < d; i++) {
            if(colors.size()) {
                ofSetColor(colors[i % colors.size()]);
            } else {
                float hue = ofMap(i, 0, d, 0, 255);
                ofSetColor(ofColor::fromHsb(hue, 255, 255));
            }
            meshes[i].draw();
        }
        ofPopMatrix();
        ofPopStyle();
    }
};

class ofApp : public ofBaseApp {
public:
    StackedPlot plot;
    
	void setup() {
        plot.historyLength = 100;
        plot.addColor(ofColor::fromHex(0xF4C341));
        plot.addColor(ofColor::fromHex(0x05A180));
        plot.addColor(ofColor::fromHex(0xE25D48));
    }
    void update() {
        vector<float> data(3);
        data[0] = abs(mouseX);
        data[1] = abs(mouseY);
        data[2] = abs(sin(ofGetElapsedTimef())) * 100;
        plot.addData(data);
    }
    void draw() {
        plot.draw(ofGetWidth(), ofGetHeight());
    }
};

int main() {
	ofSetupOpenGL(300, 150, OF_WINDOW);
	ofRunApp(new ofApp());
}
