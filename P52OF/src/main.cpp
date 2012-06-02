#include "PApp.h"

class ofApp : public PApp {
public:	
	vector<float> pollenMass;
	vector<ofVec2f> points;
	ofMesh mesh;
	
	int nPoints, step;
	float complexity, maxMass, timeSpeed, phase, windSpeed, t;
	bool debugMode;
	
	void setup() {		
		nPoints = 4096; // points to draw
		complexity = 6; // wind complexity
		maxMass = .8; // max pollen mass
		timeSpeed = .02; // wind variation speed
		phase = TWO_PI; // separate u-noise from v-noise
		windSpeed = 40; // wind vector magnitude for debug
		step = 10; // spatial sampling rate for debug
		debugMode = false;
		
		points.resize(nPoints);
		pollenMass.resize(nPoints);
		for(int i = 0; i < nPoints; i++) {
			points[i] = ofVec2f(random(0, width), random(0, height));
			pollenMass[i] = random(0, maxMass);
		}
		background(255);
		mesh.clear();
	}
	
	ofVec2f getField(ofVec2f position) {
		float normx = norm(position.x, 0, width);
		float normy = norm(position.y, 0, height);
		float u = noise(t + phase, normx * complexity + phase, normy * complexity + phase);
		float v = noise(t - phase, normx * complexity - phase, normy * complexity + phase);
		return ofVec2f(u, v);
	}
	
	void draw() { 	
		t = framecount * timeSpeed;
		if(debugMode) {
			background(255);
			stroke(0);
			float s = windSpeed;
			for(int i = 0; i < width; i += step) {
				for(int j = 0; j < height; j += step) {
					ofVec2f field = getField(ofVec2f(i, j));
					pushMatrix();
					translate(i, j);
					line(0, 0, lerp(-windSpeed, windSpeed, field.x), lerp(-windSpeed, windSpeed, field.y));
					popMatrix();
				}
			}   
			stroke(255, 0, 0);
		} else {
			stroke(0, 10);
		}
		
		mesh.setMode(OF_PRIMITIVE_POINTS);
		for(int i = 0; i < nPoints; i++) {
			float x = points[i].x, y = points[i].y;
			ofVec2f field = getField(points[i]);
			float speed = (1 + noise(t, field.x, field.y)) / pollenMass[i];
			x += lerp(-speed, speed, field.x);
			y += lerp(-speed, speed, field.y);
			
			if(x < 0 || x > width || y < 0 || y > height) {
				x = random(0, width);
				y = random(0, height);
			}
			
			if(debugMode) {
				ellipse(x, y, 3, 3);
			} else {
				mesh.addVertex(ofVec2f(x, y));
			}
			
			points[i].x = x;
			points[i].y = y;
		}
		if(!debugMode) {
			mesh.draw();
		}
	}
	
	void mousePressed() {
		setup();
	}
	
	void keyPressed() {
		debugMode = !debugMode;
		background(255);
	}
};

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 512, 512, OF_WINDOW);
	ofRunApp(new ofApp());
}
