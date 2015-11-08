#include "ofMain.h"

ofMesh thicken(ofMesh& mesh, float thickness) {
    ofMesh result;
    result.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int i = 0; i < mesh.getNumVertices()-1; i++) {
        ofVec3f& cur = mesh.getVertices()[i];
        ofVec3f& next = mesh.getVertices()[i+1];
        ofVec2f normal = ofVec2f(next - cur).rotate(90).normalize() * thickness;
        result.addVertex(cur - normal);
        result.addVertex(cur + normal);
        if(i+2 == mesh.getNumVertices()) {
            result.addVertex(next - normal);
            result.addVertex(next + normal);
        }
    }
    return result;
}

class Worm {
private:
    float phase;
    float angle() {
        return ofVec2f(1, 0).angle(velocity);
    }
    
public:
    ofVec2f position;
    ofVec2f velocity;
    float smoothing;
    int resolution;
    float thickness;
    float length, width;
    float wiggleSpeed;
    
    Worm() :
    phase(0) {
        
    }
    void update(ofVec2f position) {
        this->velocity.interpolate(position - this->position, smoothing);
        this->position.interpolate(position, smoothing);
        this->phase += (velocity.length() / length) * wiggleSpeed;
    }
    void draw() {
        ofPushMatrix();
        ofTranslate(position);
        ofRotate(angle());
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        for(int i = 0; i < resolution; i++) {
            float t = ofNormalize(i, 0, resolution - 1);
            float phase = t * TWO_PI + this->phase;
            float x = (t - 1.) * length;
            float y = sin(phase) * width;
            mesh.addVertex(ofVec2f(x, y));
        }
        if(thickness > 0) {
            mesh = thicken(mesh, thickness);
        }
        mesh.draw();
        ofPopMatrix();
    }
};

class ofApp : public ofBaseApp {
public:
    Worm worm;
    ofVec2f prevMouse;
    
    void setup() {
        ofBackground(0);
        worm.smoothing = .1;
        worm.resolution = 8;
        worm.length = 200;
        worm.width = 20;
        worm.thickness = 16;
        worm.wiggleSpeed = .5;
    }
    void update() {
        worm.update(ofVec2f(mouseX, mouseY));
    }
    void draw() {
        ofSetColor(255);
        worm.draw();
    }
    void keyPressed(int key) {
        worm.resolution = ofRandom(4, 20);
        worm.length = ofRandom(20, 400);
        worm.width = worm.length * ofRandom(.1, .3);
        worm.thickness = worm.length * ofRandom(0, .2);
        worm.wiggleSpeed = ofRandom(.1, .6);
    }
};
int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
