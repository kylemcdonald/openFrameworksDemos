#include "ofMain.h"
#include "ParticleSystem.h"

class ofApp : public ofBaseApp {
public:
    ParticleSystem particleSystem;
    
    void setup(){
        ofBackground(0);
        
        particleSystem.clear();
        int particleCount = 100;
        float size = 256;
        
        particleSystem.setExact(true);
        
        float velocity = 1;
        for(int i = 0; i < particleCount; i++) {
            float x = ofRandom(-size, size);
            float y = ofRandom(-size, size);
            float theta = ofRandom(-PI, PI);
            float xv = cos(theta) * velocity;
            float yv = sin(theta) * velocity;
            float mass = ofRandom(1, 4);
            Particle particle(x, y, mass, xv, yv);
            particleSystem.add(particle);
        }
        particleSystem.setTimeStep(.1);
        particleSystem.setFriction(.99);
        particleSystem.setCentering(1.);
        particleSystem.setGravitationalConstant(-100);
        particleSystem.setIterations(100);
    }
    
    void draw(){
        ofPushMatrix();
        ofFill();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        particleSystem.update();
        particleSystem.draw();
        ofPopMatrix();
        
        ofDrawBitmapString(ofToString(particleSystem.size()) + " particles", 32, 32);
        ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps", 32, 52);
    }
    
    void keyPressed(int key){
        if(key == 'p')
            ofSaveFrame();
        if(key == 'r')
            setup();
    }
    
    void mousePressed(int x, int y, int button){
        float mass = ofRandom(1, 4);
        Particle particle(x, y, mass);
        particleSystem.add(particle);
    }
};

int main() {
    ofSetupOpenGL(512, 512, OF_WINDOW);
    ofRunApp(new ofApp());
}
