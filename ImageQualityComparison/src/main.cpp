#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
    ofVideoGrabber cam;
    void setup() {
        cam.initGrabber(1280, 720);
    }
    void update() {
        cam.update();
    }
    void draw() {
        cam.draw(0, 0);
    }
    void keyPressed(int key) {
        if(key == '1') {
            ofSaveImage(cam.getPixels(), "out-best.jpg", OF_IMAGE_QUALITY_BEST);
        }
        if(key == '2') {
            ofSaveImage(cam.getPixels(), "out-high.jpg", OF_IMAGE_QUALITY_HIGH);
        }
        if(key == '3') {
            ofSaveImage(cam.getPixels(), "out-medium.jpg", OF_IMAGE_QUALITY_MEDIUM);
        }
    }
};
int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
