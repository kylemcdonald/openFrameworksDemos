#include "ofMain.h"

class Particle {
public:
    shared_ptr<ofVec2f> pos, lastPos;
    Particle(const ofVec2f& pos)
    :pos(new ofVec2f(pos))
    ,lastPos(new ofVec2f(pos)) {
    }
    virtual void draw() {
        ofPushStyle();
        ofFill();
        ofSetHexColor(0x2dad8f);
        ofDrawCircle(*pos, 2);
        ofPopStyle();
    }
};

class Constraint {
public:
    virtual void relax(float stepCoef) = 0;
    virtual void draw() const = 0;
};

class PinConstraint : public Constraint, public Particle {
public:
    shared_ptr<Particle> a;
    PinConstraint(shared_ptr<Particle> a, const ofVec2f& pos)
    :a(a)
    ,Particle(pos) {
    }
    void relax(float stepCoef) {
        a->pos->set(*pos);
    }
    void draw() const {
        ofPushStyle();
        ofFill();
        ofSetColor(0, 153, 255, 25);
        ofDrawCircle(*pos, 6);
        ofPopStyle();
    }
};

class DistanceConstraint : public Constraint {
public:
    shared_ptr<Particle> a, b;
    float stiffness, distance;
    DistanceConstraint(shared_ptr<Particle> a, shared_ptr<Particle> b, float stiffness)
    :a(a)
    ,b(b)
    ,stiffness(stiffness) {
        distance = a->pos->distance(*b->pos);
    }
    DistanceConstraint(shared_ptr<Particle> a, shared_ptr<Particle> b, float stiffness, float distance)
    :a(a)
    ,b(b)
    ,stiffness(stiffness)
    ,distance(distance) {
    }
    void relax(float stepCoef) {
        ofVec2f normal = *a->pos - *b->pos;
        float m = normal.lengthSquared();
        normal *= ((distance*distance - m)/m)*stiffness*stepCoef;
        *a->pos += normal;
        *b->pos -= normal;
    }
    void draw() const {
        ofPushStyle();
        ofFill();
        ofSetHexColor(0xd8dde2);
        ofDrawLine(*a->pos, *b->pos);
        ofPopStyle();
    }
};

class ForceConstraint : public Constraint {
public:
    shared_ptr<Particle> a, b;
    float force;
    ForceConstraint(shared_ptr<Particle> a, shared_ptr<Particle> b, float force)
    :a(a)
    ,b(b)
    ,force(force) {
    }
    void relax(float stepCoef) {
        ofVec2f normal = *a->pos - *b->pos;
//        float m = normal.length();
        normal *= force;
        *a->pos += normal;
        *b->pos -= normal;
    }
    void draw() const {
        ofPushStyle();
        ofFill();
        ofSetHexColor(0xd833e2);
        ofDrawLine(*a->pos, *b->pos);
        ofPopStyle();
    }
};

class AngleConstraint : public Constraint {
public:
    shared_ptr<Particle> a, b, c;
    float stiffness, angle;
    AngleConstraint(shared_ptr<Particle> a, shared_ptr<Particle> b, shared_ptr<Particle> c, float stiffness)
    :a(a)
    ,b(b)
    ,c(c)
    ,stiffness(stiffness) {
        angle = (*a->pos - *b->pos).angleRad(*c->pos - *b->pos);
    }
    void relax(float stepCoef) {
        float angle = (*a->pos - *b->pos).angleRad(*c->pos - *b->pos);
        float diff = angle - this->angle;
        if(diff <= -PI)
            diff += TWO_PI;
        else if(diff >= PI)
            diff -= TWO_PI;
        diff *= stepCoef*stiffness;
        a->pos->rotateRad(diff, *b->pos);
        c->pos->rotateRad(-diff, *b->pos);
        b->pos->rotateRad(diff, *a->pos);
        b->pos->rotateRad(-diff, *c->pos);
    }
    void draw() const {
        ofPushStyle();
        ofSetLineWidth(5);
        ofNoFill();
        ofSetColor(255, 255, 0, 50);
        ofDrawLine(*a->pos, *b->pos);
        ofDrawLine(*b->pos, *c->pos);
        ofPopStyle();
    }
};

class Composite {
public:
    vector<shared_ptr<Particle> > particles;
    vector<shared_ptr<Constraint> > constraints;
    
    shared_ptr<PinConstraint> pin(int index, const ofVec2f& pos) {
        shared_ptr<PinConstraint> pc(new PinConstraint(particles[index], pos));
        constraints.push_back(pc);
        return pc;
    }
    shared_ptr<PinConstraint> pin(int index) {
        return pin(index, *particles[index]->pos);
    }
};

class Verlet {
public:
    float width, height;
    float friction;
    ofVec2f gravity;
    ofVec2f mouse;
    
    shared_ptr<Particle> draggedEntity;
    vector<shared_ptr<Composite> > composites;
    
    Verlet(float width, float height)
    :width(width)
    ,height(height)
    ,friction(0.99)
    ,gravity(0, 0.2) {
    }
    
    void frame(float step) {
        for(auto composite : composites) {
            auto particles = composite->particles;
            for(auto particle : particles) {
                // calculate velocity
                ofVec2f velocity = (*particle->pos - *particle->lastPos) * friction;
                
                // unimplemented: ground friction
                
                // save last good state
                particle->lastPos->set(*particle->pos);
                
                // gravity
                *particle->pos += gravity;
                
                // inertia
                *particle->pos += velocity;
            }
        }
        
        // handle dragging of entities
        if(draggedEntity) {
            draggedEntity->pos->set(mouse);
        }
        
        // relax
        float stepCoef = 1 / step;
        for(auto composite : composites) {
            auto constraints = composite->constraints;
            for(int i = 0; i < step; i++) {
                for(auto constraint : constraints) {
                    constraint->relax(stepCoef);
                }
            }
        }
    }
    
    void draw() {
        for(auto composite : composites) {
            auto constraints = composite->constraints;
            for(auto constraint : constraints) {
                constraint->draw();
            }
            auto particles = composite->particles;
            for(auto particle : particles) {
                particle->draw();
            }
        }
        
        // highlight nearest / dragged entity
        shared_ptr<Particle> nearest = nearestEntity();
        if(nearest) {
            ofPushStyle();
            ofNoFill();
            ofSetHexColor(0x4f545c);
            ofDrawCircle(*nearest->pos, 8);
            ofPopStyle();
        }
    }
    
    shared_ptr<Particle> nearestEntity() {
        ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
        shared_ptr<Particle> entity;
        vector<shared_ptr<Constraint> >* constraintsNearest = NULL;
        float selectionRadius = 10;
        float d2Nearest;
        for(auto composite : composites) {
            for(auto particle : composite->particles) {
                float d2 = particle->pos->squareDistance(mouse);
                if(d2 < selectionRadius*selectionRadius &&
                   (!entity || d2 < d2Nearest)) {
                    entity = particle;
                    constraintsNearest = &(composite->constraints);
                    d2Nearest = d2;
                }
            }
        }
        
        if(constraintsNearest) {
            for(auto constraint : *constraintsNearest) {
                shared_ptr<PinConstraint> pin = dynamic_pointer_cast<PinConstraint>(constraint);
                if(pin && pin->a == entity) {
                    return dynamic_pointer_cast<Particle>(pin);
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
        draggedEntity = shared_ptr<Particle>();
    }
};

class Cloth : public Verlet {
public:
    Cloth(ofVec2f origin, float width, float height, int segments, int pinMod, float stiffness)
    :Verlet(width, height) {
        shared_ptr<Composite> composite(new Composite());
        int xStride = width / segments;
        int yStride = height / segments;
        for(int y = 0; y < segments; y++) {
            for(int x = 0; x < segments; x++) {
                float px = origin.x + x*xStride - width/2 + xStride/2;
                float py = origin.y + y*yStride - height/2 + yStride/2;
                shared_ptr<Particle> particle(new Particle(ofVec2f(px, py)));
                composite->particles.push_back(particle);
                
                if(x > 0) {
                    auto a = composite->particles[y*segments+x];
                    auto b = composite->particles[y*segments+(x-1)];
                    shared_ptr<DistanceConstraint> c(new DistanceConstraint(a, b, stiffness));
                    composite->constraints.push_back(c);
                }
                
                if(y > 0) {
                    auto a = composite->particles[y*segments+x];
                    auto b = composite->particles[(y-1)*segments+x];
                    shared_ptr<DistanceConstraint> c(new DistanceConstraint(a, b, stiffness));
                    composite->constraints.push_back(c);
                }
            }
        }
        
        for(int x = 0; x < segments; x++) {
            if(x % pinMod == 0) {
                composite->pin(x);
            }
        }
        
        composites.push_back(composite);
    }
};

class ofApp : public ofBaseApp {
public:
    shared_ptr<Cloth> cloth;
    uint64_t t;
    void setup() {
        ofBackground(255);
        float width = ofGetWidth(), height = ofGetHeight();
        cloth = shared_ptr<Cloth>(new Cloth(ofVec2f(width / 2, height / 2),
                                            width / 2, height / 2,
                                            25, 6, 2));
        t = 0;
    }
    void update() {
    	auto now = ofGetElapsedTimeMicros();
        cloth->frame(16);
    	auto then = ofGetElapsedTimeMicros();
    	t += then-now;
    	if(ofGetFrameNum()%60==0){
    		cout << "time " << t/60 << endl;
    		t = 0;
    	}
    }
    void draw() {
        cloth->draw();
        ofSetColor(0);
        ofDrawBitmapString(ofGetFrameRate(),20,20);
    }
    void mousePressed(int x, int y, int button) {
        cloth->mouseMoved(x, y);
        cloth->mousePressed(x, y);
    }
    void mouseDragged(int x, int y, int button) {
        cloth->mouseMoved(x, y);
    }
    void mouseReleased(int x, int y, int button) {
        cloth->mouseMoved(x, y);
        cloth->mouseReleased();
    }
};
int main() {
    ofSetupOpenGL(720, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
