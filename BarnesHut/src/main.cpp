/*
 switch from shared_ptr to reserved memory
 might be worth it to add a USE_MASS declaration
 would be a significant increase if all mass=1
 */

#include "ofMain.h"
#include "ParticleSystem.h"

class ofApp : public ofBaseApp {
public:
    ParticleSystem ps;
    
    void setup(){
        int q = 100;
        
        ofBackground(0);
        
        ps.clear();
        int particleCount = 1000;
        float size = MIN(ofGetWidth(), ofGetHeight()) / 2;
        
        ps.setExact(false);
        
        float velocity = 0;
        for(int i = 0; i < particleCount; i++) {
            float x = ofRandom(-size, size);
            float y = ofRandom(-size, size);
            float theta = ofRandom(-PI, PI);
            float xv = cos(theta) * velocity;
            float yv = sin(theta) * velocity;
            float mass = ofRandom(10, 100);
            Particle particle(x, y, mass, xv, yv);
            ps.add(particle);
        }
        for(int i = 1; i < particleCount; i++) {
            shared_ptr<Particle> a = ps.getParticle(i);
            shared_ptr<Particle> b = ps.getParticle(ofRandom(i / 10));
            ps.addSpring(a, b, 1000, 100);
        }
        
        ps.setTimeStep(.5);
        ps.setFriction(.9);
        ps.setCentering(.001);
        ps.setGravitationalConstant(-100);
        ps.setIterations(5);
    }
    
    void draw(){
        if(ofGetFrameNum() % 10 == 0) {
            float mass = ofRandom(10, 500);
            Particle particle(0, -ofGetHeight() / 2, mass, 0, 100);
//            ps.add(particle);
        }
        
        ofPushMatrix();
        ofFill();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        ps.update();
        ps.draw();
        ofPopMatrix();
        
        ofSetColor(255);
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
        Particle particle(x - ofGetWidth() / 2, y - ofGetHeight() / 2, mass);
        ps.add(particle);
    }
};

int main() {
    ofSetupOpenGL(1680, 1050, OF_FULLSCREEN);
    ofRunApp(new ofApp());
}
