#include "ofMain.h"

bool lineIntersection(const ofVec2f& v1, const ofVec2f& v2, const ofVec2f& v3, const ofVec2f& v4) {
    float d = (v4.y-v3.y)*(v2.x-v1.x)-(v4.x-v3.x)*(v2.y-v1.y);
    float u = (v4.x-v3.x)*(v1.y-v3.y)-(v4.y-v3.y)*(v1.x-v3.x);
    float v = (v2.x-v1.x)*(v1.y-v3.y)-(v2.y-v1.y)*(v1.x-v3.x);
    if (d < 0) u = -u; v = -v; d = -d;
    return (0<=u && u<=d) && (0<=v && v<=d);
}

float cross(const ofVec2f& u, const ofVec2f& v) {
    return u.x*v.y-u.y*v.x;
}

// being on a corner or on an edge is not being inside
bool pointInTriangle(const ofVec2f& P, const ofVec2f& A, const ofVec2f& B, const ofVec2f& C) {
    ofVec2f v0(C.x-A.x, C.y-A.y);
    ofVec2f v1(B.x-A.x, B.y-A.y);
    ofVec2f v2(P.x-A.x, P.y-A.y);
    float u = cross(v2,v0);
    float v = cross(v1,v2);
    float d = cross(v1,v0);
    if (d < 0) u = -u; v = -v; d = -d;
    return u>0 && v>0 && ((u+v) < d);
}

bool nearbyCircle(const ofVec2f& A1, const ofVec2f& B1, const ofVec2f& C1,
                  const ofVec2f& A2, const ofVec2f& B2, const ofVec2f& C2) {
    ofVec2f center1 = (A1 + B1 + C1) / 3;
    ofVec2f center2 = (A2 + B2 + C2) / 3;
    float sizeSquared1 = MAX(A1.squareDistance(center1), MAX(B1.squareDistance(center1), C1.squareDistance(center1)));
    float sizeSquared2 = MAX(A2.squareDistance(center2), MAX(B2.squareDistance(center2), C2.squareDistance(center2)));
    float minimumDistance = sqrtf(sizeSquared1) + sqrtf(sizeSquared2);
    return center1.squareDistance(center2) <= (minimumDistance * minimumDistance);
}

bool triangleIntersection(const ofVec2f& A1, const ofVec2f& B1, const ofVec2f& C1,
                          const ofVec2f& A2, const ofVec2f& B2, const ofVec2f& C2) {
    return
    pointInTriangle(A1, A2, B2, C2) ||
    pointInTriangle(B1, A2, B2, C2) ||
    pointInTriangle(C1, A2, B2, C2) ||
    pointInTriangle(A2, A1, B1, C1) ||
    pointInTriangle(B2, A1, B1, C1) ||
    pointInTriangle(C2, A1, B1, C1) ||
    lineIntersection(A1, B1, A2, B2) ||
    lineIntersection(A1, B1, B2, C2) ||
    lineIntersection(A1, B1, C2, A2) ||
    lineIntersection(B1, C1, A2, B2) ||
    lineIntersection(B1, C1, B2, C2) ||
    lineIntersection(B1, C1, C2, A2) ||
    lineIntersection(C1, A1, A2, B2) ||
    lineIntersection(C1, A1, B2, C2) ||
    lineIntersection(C1, A1, C2, A2);
}

// this is faster if triangles are small and spread out
bool triangleIntersectionFast(const ofVec2f& A1, const ofVec2f& B1, const ofVec2f& C1,
                              const ofVec2f& A2, const ofVec2f& B2, const ofVec2f& C2) {
    return nearbyCircle(A1, B1, C1, A2, B2, C2) &&
    triangleIntersection(A1, B1, C1, A2, B2, C2);
}


class Triangle {
public:
    ofVec2f A, B, C;
    Triangle(ofVec2f A, ofVec2f B, ofVec2f C)
    :A(A), B(B), C(C) {
    }
};

class ofApp : public ofBaseApp {
public:
    ofVec2f A1, B1, C1;
    ofVec2f A2, B2, C2;
    vector<ofVec2f> points;
    vector<Triangle> triangles;
    void setup() {
        ofBackground(0);
        for(int i = 0; i < 10000; i++) {
            points.emplace_back(ofRandomWidth(), ofRandomHeight());
        }
        
        // test for on-corner behavior
        A1.set(0, 0);
        B1.set(100, 100);
        C1.set(0, 100);
        ofLog() << pointInTriangle(A1, A1, B1, C1);
        ofLog() << pointInTriangle(B1, A1, B1, C1);
        ofLog() << pointInTriangle(C1, A1, B1, C1);
        ofLog() << pointInTriangle((A1+B1) / 2, A1, B1, C1);
        ofLog() << lineIntersection(A1, B1, A1, B1);
        
        placeTriangle();
    }
    void placeTriangle() {
        A1.set(ofRandomWidth(), ofRandomHeight());
        B1.set(ofRandomWidth(), ofRandomHeight());
        C1.set(ofRandomWidth(), ofRandomHeight());
        A2.set(ofRandomWidth(), ofRandomHeight());
        B2.set(ofRandomWidth(), ofRandomHeight());
        C2.set(ofRandomWidth(), ofRandomHeight());
//
//        cout
//        << "A1.set(" << A1 << ");" << endl
//        << "B1.set(" << B1 << ");" << endl
//        << "C1.set(" << C1 << ");" << endl
//        << "A2.set(" << A2 << ");" << endl
//        << "B2.set(" << B2 << ");" << endl
//        << "C2.set(" << C2 << ");" << endl
//        << endl;
    }
    void update() {
//        triangles.clear();
//        for(int i = 0; i < 1000; i++) {
//            triangles.emplace_back(ofVec2f(ofRandomWidth(), ofRandomHeight()),
//                                   ofVec2f(ofRandomWidth(), ofRandomHeight()),
//                                   ofVec2f(ofRandomWidth(), ofRandomHeight()));
//        }
//        float targetScale = 10;
//        for(Triangle& t : triangles) {
//            ofVec2f center = (t.A + t.B + t.C) / 3;
//            float size = MAX(t.A.distance(center), MAX(t.B.distance(center), t.C.distance(center)));
//            float scale = targetScale / size;
//            t.A = (t.A - center) * scale + center;
//            t.B = (t.B - center) * scale + center;
//            t.C = (t.C - center) * scale + center;
//        }
//        uint64_t start = ofGetElapsedTimeMillis();
//        int intersecting = 0, total = 0;
//        for(Triangle t0 : triangles) {
//            for(Triangle t1 : triangles) {
//                if(triangleIntersectionFast(t0.A, t0.B, t0.C, t1.A, t1.B, t1.C)) {
//                    intersecting++;
//                }
//                total++;
//            }
//        }
//        uint64_t stop = ofGetElapsedTimeMillis();
//        ofLog() << stop - start << "ms for " << ((100.*intersecting)/total) << "% of " << total;
    }
    void draw() {
        ofFill();
        for(auto& P : points) {
            bool T1 = pointInTriangle(P, A1, B1, C1);
            bool T2 = pointInTriangle(P, A2, B2, C2);
            ofSetColor(255, T1 * 255, T2 * 255, 64);
            ofDrawCircle(P, 2);
        }
        ofNoFill();
        bool TT = triangleIntersection(A1, B1, C1, A2, B2, C2);
        ofSetColor(ofColor::green);
        if(TT) {
            ofSetColor(255, 128);
            ofFill();
        }
        ofDrawTriangle(A1, B1, C1);
        ofDrawTriangle(A2, B2, C2);
        
        ofPushMatrix();
        ofSetColor(255);
        ofTranslate(10, 20);
        ofDrawBitmapString(pointInTriangle(A1, A2, B2, C2) ? "pointInTriangle(A1, A2, B2, C2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(pointInTriangle(B1, A2, B2, C2) ? "pointInTriangle(B1, A2, B2, C2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(pointInTriangle(C1, A2, B2, C2) ? "pointInTriangle(C1, A2, B2, C2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(pointInTriangle(A2, A1, B1, C1) ? "pointInTriangle(A2, A1, B1, C1)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(pointInTriangle(B2, A1, B1, C1) ? "pointInTriangle(B2, A1, B1, C1)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(pointInTriangle(C2, A1, B1, C1) ? "pointInTriangle(C2, A1, B1, C1)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(A1, B1, A2, B2) ? "lineIntersection(A1, B1, A2, B2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(A1, B1, B2, C2) ? "lineIntersection(A1, B1, B2, C2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(A1, B1, C2, A2) ? "lineIntersection(A1, B1, C2, A2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(B1, C1, A2, B2) ? "lineIntersection(B1, C1, A2, B2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(B1, C1, B2, C2) ? "lineIntersection(B1, C1, B2, C2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(B1, C1, C2, A2) ? "lineIntersection(B1, C1, C2, A2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(C1, A1, A2, B2) ? "lineIntersection(C1, A1, A2, B2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(C1, A1, B2, C2) ? "lineIntersection(C1, A1, B2, C2)" : "-", 0, 0); ofTranslate(0, 20);
        ofDrawBitmapString(lineIntersection(C1, A1, C2, A2) ? "lineIntersection(C1, A1, C2, A2)" : "-", 0, 0); ofTranslate(0, 20);
        ofPopMatrix();

//        ofSetColor(255);
//        ofNoFill();
//        for(Triangle& t : triangles) {
//            ofDrawTriangle(t.A, t.B, t.C);
//        }
    }
    void keyPressed(int key) {
        placeTriangle();
    }
};
int main() {
    ofSetupOpenGL(800, 800, OF_WINDOW);
    ofRunApp(new ofApp());
}
