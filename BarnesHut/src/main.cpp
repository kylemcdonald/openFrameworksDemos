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
            float mass = 1; //ofRandom(10, 100);
            ps.addParticle(Particle(x, y, mass, xv, yv));
            
            if(i > 1) {
                shared_ptr<Particle>& a = ps.getParticle(i);
                shared_ptr<Particle>& b = ps.getParticle(ofRandom(i / 10));
                ps.addSpring(a, b, 2000, 100);
            }
        }
        
        ps.setTimeStep(.8);
        ps.setFriction(.9);
        ps.setCentering(.1);
        ps.setGravitationalConstant(-1000);
        ps.setIterations(40);
    }
    
    void draw(){
        ofPushMatrix();
        ofFill();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        
        int choice = ofRandom(4);
        switch(choice) {
            case 0:
                ps.addParticle(Particle(ofRandomWidth() - ofGetWidth() / 2, ofRandomHeight() - ofGetHeight() / 2, 1, 0, 0));
                break;
            case 1:
                ps.removeParticle(ofRandom(ps.getParticleCount()));
                break;
            case 2:
                ps.addSpring(ofRandom(ps.getParticleCount()), ofRandom(ps.getParticleCount()), 2000, 100);
                break;
            case 3:
                ps.removeSpring(ofRandom(ps.getSpringCount()));
                break;
        }
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
        float mass = 1;//ofRandom(1, 4);
        Particle particle(x - ofGetWidth() / 2, y - ofGetHeight() / 2, mass);
        ps.addParticle(particle);
    }
};

int main() {
    ofSetupOpenGL(1680, 1050, OF_FULLSCREEN);
    ofRunApp(new ofApp());
}
