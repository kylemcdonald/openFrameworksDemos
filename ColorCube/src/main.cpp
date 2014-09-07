#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
    ofEasyCam cam;
    ofVideoGrabber vid;
    ofMesh mesh;
    int w = 1280, h = 720;
    int n = w * h;
    void setup() {
        vid.initGrabber(w, h);
        mesh.setMode(OF_PRIMITIVE_POINTS);
        mesh.addVertices(vector<ofVec3f>(n));
        mesh.addColors(vector<ofFloatColor>(n));
    }
    void update() {
        vid.update();
        if(vid.isFrameNew()) {
            unsigned char* pix = vid.getPixels();
            int j = 0;
            for(int i = 0; i < n; i++) {
                unsigned char& r = pix[j++];
                unsigned char& g = pix[j++];
                unsigned char& b = pix[j++];
                mesh.setVertex(i, ofVec3f(r, g, b));
                mesh.setColor(i, ofColor(r, g, b));
            }
        }
    }
    void draw() {
        ofBackground(0);
        cam.begin();
        ofNoFill();
        ofDrawBox(0, 0, 0, 256, 256, 256);
        ofTranslate(-128, -128, -128);
        mesh.draw();
        cam.end();
    }
};

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
