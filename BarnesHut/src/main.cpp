#include "ofMain.h"
#include "ParticleSystem.h"

class ofApp : public ofBaseApp {
public:
    ParticleSystem ps;
    
    void setup(){
        ofBackground(0);
        
        ps.clear();
        ps.setExact(false);
        ps.setTimeStep(.2);
        ps.setFriction(.9);
        ps.setCentering(.1);
        ps.setGravitationalConstant(-1000);
        ps.setIterations(10);
    }
    
    void draw(){
        ofPushMatrix();
        ofFill();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        
        for(int i = 0; i < 10; i++) {
            int choice = ofRandom(3);
            switch(choice) {
                case 0:
                    if(ps.getParticleCount() < 2000) {
                        ps.addParticle(Particle(ofRandomWidth() - ofGetWidth() / 2, ofRandomHeight() - ofGetHeight() / 2, 1, 0, 0));
                    } else {
                        ps.removeParticle(ofRandom(ps.getParticleCount()));
                    }
                    break;
                case 2:
                    if(ps.getSpringCount() > ps.getParticleCount()) {
                        ps.removeSpring(ofRandom(ps.getSpringCount()));
                    } else {
                        ps.addSpring(ofRandom(ps.getParticleCount()), ofRandom(ps.getParticleCount()), 2000, 200);
                    }
                    break;
            }
        }
        ps.update();
        
        ps.draw();
        ofPopMatrix();
        
        ofSetColor(255);
        ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps @ " + ofToString((int) ps.getParticleCount()), 32, 52);
    }
    
    void keyPressed(int key){
        if(key == 'p')
            ofSaveFrame();
        if(key == 'r')
            setup();
    }
    
    void mousePressed(int x, int y, int button){
        float mass = 1;//ofRandom(1, 4);
        Particle particle(x - ofGetWidth() / 2, y - ofGetHeight() / 2, mass);
        ps.addParticle(particle);
    }
};

int main() {
    ofSetupOpenGL(1680, 1050, OF_FULLSCREEN);
    ofRunApp(new ofApp());
}
