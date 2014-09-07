#include "StackedPlot.h"

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

StackedPlot::StackedPlot()
:historyLength(100) {
}
void StackedPlot::clear() {
    history.clear();
}
void StackedPlot::addData(const vector<float>& data) {
    if(size() && data.size() != dimensions()) {
        ofLogWarning("StackedPlot") << "added data does not have same size as existing data";
        clear();
    }
    vector<float> normalized = normalize(data);
    vector<float> accumulated = accumulate(normalized);
    vector<float> all;
    all.push_back(0);
    all.insert(all.end(), accumulated.begin(), accumulated.end());
    all.push_back(1);
    history.push_back(all);
    while(history.size() > historyLength) {
        history.pop_front();
    }
}
void StackedPlot::addColor(ofColor color) {
    colors.push_back(color);
}
int StackedPlot::size() const {
    return history.size();
}
int StackedPlot::dimensions() const {
    return size() ? history.front().size() - 2 : 0;
}
void StackedPlot::draw(float width, float height) const {
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
        const vector<float>& cur = *itr;
        for(int j = 0; j < d; j++) {
            meshes[j].addVertex(ofVec2f(i, cur[j]));
            meshes[j].addVertex(ofVec2f(i, cur[j + 1]));
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