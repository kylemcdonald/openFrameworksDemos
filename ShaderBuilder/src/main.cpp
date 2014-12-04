#include "ofMain.h"
#include "ofAutoShader.h"

class ofApp : public ofBaseApp {
public:
    ofAutoShader shader;
    int mouseXPressed, mouseYPressed;
    void setup() {
        shader.setup("shader");
    }
    void draw() {
        shader.begin();
        shader.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
        shader.setUniform1f("iGlobalTime", ofGetElapsedTimef());
        shader.setUniform4f("iMouse", mouseX, mouseY, mouseXPressed, mouseYPressed);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        shader.end();
    }
    void mousePressed(int x, int y, int button) {
        mouseDragged(x, y, button);
    }
    void mouseDragged(int x, int y, int button) {
        mouseXPressed = x;
        mouseYPressed = y;
    }
};

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
