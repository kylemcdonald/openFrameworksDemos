#include "ofMain.h"
#include <memory>
using namespace std;

#define USE_VBO

class Particle {
public:
    ofVec2f pos, lastPos;
    Particle(const ofVec2f & pos)
    :pos(pos)
    ,lastPos(pos) {
    }
    void draw() {
        ofFill();
        ofSetHexColor(0x2dad8f);
        ofDrawCircle(pos, 2);
    }
};

class Constraint {
public:
	virtual ~Constraint(){}
    virtual void relax(float stepCoef) = 0;
    virtual void draw() const = 0;
};

class PinConstraint : public Constraint {
public:
    Particle * a;
    ofVec2f pos;
    PinConstraint(Particle * a, const ofVec2f& pos)
    :a(a)
    ,pos(pos){
    }
    void relax(float stepCoef) {
        a->pos.set(pos);
    }
    void draw() const {
        ofFill();
        ofSetColor(0, 153, 255, 25);
        ofDrawCircle(pos, 6);
    }
};

class DistanceConstraint : public Constraint {
public:
    Particle *a, *b;
    float stiffness, distance;
    DistanceConstraint(Particle * a, Particle * b, float stiffness)
    :a(a)
    ,b(b)
    ,stiffness(stiffness) {
        distance = a->pos.distance(b->pos);
    }
    DistanceConstraint(Particle * a, Particle * b, float stiffness, float distance)
    :a(a)
    ,b(b)
    ,stiffness(stiffness)
    ,distance(distance) {
    }
    void relax(float stepCoef) {
        ofVec2f normal = a->pos - b->pos;
        float m = normal.lengthSquared();
        normal *= ((distance*distance - m)/m)*stiffness*stepCoef;
        a->pos += normal;
        b->pos -= normal;
    }
    void draw() const {
        ofFill();
        ofSetHexColor(0xd8dde2);
        ofDrawLine(a->pos, b->pos);
    }
};

class ForceConstraint : public Constraint {
public:
    Particle *a, *b;
    float force;
    ForceConstraint(Particle * a, Particle * b, float force)
    :a(a)
    ,b(b)
    ,force(force) {
    }
    void relax(float stepCoef) {
        ofVec2f normal = a->pos - b->pos;
//        float m = normal.length();
        normal *= force;
        a->pos += normal;
        b->pos -= normal;
    }
    void draw() const {
        ofFill();
        ofSetHexColor(0xd833e2);
        ofDrawLine(a->pos, b->pos);
    }
};

class AngleConstraint : public Constraint {
public:
    Particle *a, *b, *c;
    float stiffness, angle;
    AngleConstraint(Particle * a, Particle * b, Particle * c, float stiffness)
    :a(a)
    ,b(b)
    ,c(c)
    ,stiffness(stiffness) {
        angle = (a->pos - b->pos).angleRad(c->pos - b->pos);
    }
    void relax(float stepCoef) {
        float angle = (a->pos - b->pos).angleRad(c->pos - b->pos);
        float diff = angle - this->angle;
        if(diff <= -PI)
            diff += TWO_PI;
        else if(diff >= PI)
            diff -= TWO_PI;
        diff *= stepCoef*stiffness;
        a->pos.rotateRad(diff, b->pos);
        c->pos.rotateRad(-diff, b->pos);
        b->pos.rotateRad(diff, a->pos);
        b->pos.rotateRad(-diff, c->pos);
    }
    void draw() const {
        ofPushStyle();
        ofSetLineWidth(5);
        ofNoFill();
        ofSetColor(255, 255, 0, 50);
        ofDrawLine(a->pos, b->pos);
        ofDrawLine(b->pos, c->pos);
        ofPopStyle();
    }
};

class Composite {
public:
    vector<Particle> particles;
    vector<Constraint*> constraints;
    
    PinConstraint & pin(int index, const ofVec2f& pos) {
        constraints.push_back(new PinConstraint(&particles[index], pos));
        return static_cast<PinConstraint&>(*constraints.back());
    }
    PinConstraint & pin(int index) {
        return pin(index, particles[index].pos);
    }
};

class Verlet {
public:
    float width, height;
    float friction;
    ofVec2f gravity;
    ofVec2f mouse;
    
    ofVec2f * draggedEntity;
    vector<Composite> composites;
    ofVbo particlesVbo;
    ofVbo constraintsVbo;
    
    Verlet(float width, float height)
    :width(width)
    ,height(height)
    ,friction(0.99)
    ,gravity(0, 0.2)
    ,draggedEntity(nullptr){
    }
    
    void frame(float step) {
        for(auto & composite : composites) {
            for(auto & particle : composite.particles) {
                // calculate velocity
                ofVec2f velocity = (particle.pos - particle.lastPos) * friction;
                
                // unimplemented: ground friction
                
                // save last good state
                particle.lastPos = particle.pos;
                
                // gravity
                particle.pos += gravity;
                
                // inertia
                particle.pos += velocity;
            }
        }
        
        // handle dragging of entities
        if(draggedEntity) {
            *draggedEntity = mouse;
        }
        
        // relax
        float stepCoef = 1 / step;
        for(auto & composite : composites) {
            for(int i = 0; i < step; i++) {
                for(auto & constraint : composite.constraints) {
                    constraint->relax(stepCoef);
                }
            }
        }
    	particlesVbo.updateVertexData(&composites[0].particles[0].pos.x,composites[0].particles.size());
    }
    
    void draw() {
#ifndef USE_VBO
        for(auto & composite : composites) {
            for(auto & constraint : composite.constraints) {
                constraint->draw();
            }

            for(auto & particle : composite.particles) {
                particle.draw();
            }
        }
#else
        ofSetColor(0, 153, 255, 25);
    	glPointSize(6);
    	particlesVbo.draw(GL_POINTS,0,composites[0].particles.size());
#endif
        // highlight nearest / dragged entity
        auto nearest = nearestEntity();
        if(nearest) {
            ofNoFill();
            ofSetHexColor(0x4f545c);
            ofDrawCircle(*nearest, 8);
        }
    }
    
    ofVec2f * nearestEntity() {
        ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
        ofVec2f * entity = nullptr;
        vector<Constraint*> * constraintsNearest = nullptr;
        float selectionRadius = 10;
        float d2Nearest = numeric_limits<float>::max();
        for(auto & composite : composites) {
            for(auto & particle : composite.particles) {
                float d2 = particle.pos.squareDistance(mouse);
                if(d2 < selectionRadius*selectionRadius &&
                   (!entity || d2 < d2Nearest)) {
                    entity = &particle.pos;
                    constraintsNearest = &composite.constraints;
                    d2Nearest = d2;
                }
            }
        }
        
        if(constraintsNearest) {
            for(auto & constraint : *constraintsNearest) {
                PinConstraint * pin = dynamic_cast<PinConstraint*>(constraint);
                if(pin && &pin->a->pos == entity) {
                    return &pin->pos;
                }
            }
        }
        
        return entity;
    }
    
    void mousePressed(int x, int y) {
        auto nearest = nearestEntity();
        if(nearest) {
            draggedEntity = nearest;
        }
    }

    void mouseMoved(int x, int y) {
        mouse.x = x;
        mouse.y = y;
    }
    
    void mouseReleased() {
        draggedEntity = nullptr;
    }
};

class Cloth : public Verlet {
public:
    Cloth(const ofVec2f & origin, float width, float height, int segments, int pinMod, float stiffness)
    :Verlet(width, height) {
    	composites.resize(1);
    	auto & composite = composites.back();
        composite.particles.reserve(segments * segments);
        int xStride = width / segments;
        int yStride = height / segments;
        for(int y = 0; y < segments; y++) {
            for(int x = 0; x < segments; x++) {
                float px = origin.x + x*xStride - width/2 + xStride/2;
                float py = origin.y + y*yStride - height/2 + yStride/2;
                composite.particles.push_back(Particle(ofVec2f(px, py)));
                
                if(x > 0) {
                    auto a = &composite.particles[y*segments+x];
                    auto b = &composite.particles[y*segments+(x-1)];
                    composite.constraints.push_back(new DistanceConstraint(a, b, stiffness));
                }
                
                if(y > 0) {
                    auto a = &composite.particles[y*segments+x];
                    auto b = &composite.particles[(y-1)*segments+x];
                    composite.constraints.push_back(new DistanceConstraint(a, b, stiffness));
                }
            }
        }
        
        for(int x = 0; x < segments; x++) {
            if(x % pinMod == 0) {
                composite.pin(x);
            }
        }
    	particlesVbo.setVertexData(&composites[0].particles[0].pos.x,2,composites[0].particles.size(),GL_STREAM_DRAW,sizeof(Particle));
    }
};

class ofApp : public ofBaseApp {
public:
    Cloth cloth;
    uint64_t t;

    ofApp()
    :cloth(ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2),
    		ofGetWidth() / 2, ofGetHeight() / 2,
            150, 6, 2)
    ,t(0)
    {

    }

    void setup() {
        ofBackground(255);
    }
    void update() {
    	auto now = ofGetElapsedTimeMicros();
        cloth.frame(16);
    	auto then = ofGetElapsedTimeMicros();
    	t += then-now;
    	if(ofGetFrameNum()%60==0){
    		cout << "time " << t/60 << endl;
    		t = 0;
    	}
    }
    void draw() {
        cloth.draw();
        ofSetColor(0);
        ofDrawBitmapString(ofGetFrameRate(),20,20);
    }
    void mousePressed(int x, int y, int button) {
        cloth.mousePressed(x, y);
    }
    void mouseDragged(int x, int y, int button) {
        cloth.mouseMoved(x, y);
    }
    void mouseReleased(int x, int y, int button) {
        cloth.mouseReleased();
    }
};
int main() {
    ofSetupOpenGL(720, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
