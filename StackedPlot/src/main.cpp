#include "ofMain.h"
#include "StackedPlot.h"

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
