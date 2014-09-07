#include "CurveLut.h"

CurveLut::CurveLut()
:n(0)
,lutNew(false) {
}
	
void CurveLut::setup(int n) {
	this->n = n;
	lut.resize(n);
	add(ofVec2f(0, 0));
	add(ofVec2f(n, n));
}

bool xSort(const ofVec2f& a, const ofVec2f& b) {
	return a.x < b.x;
}

void CurveLut::add(ofVec2f controlPoint) {
	controlPoint.x = ofClamp(controlPoint.x, 0, n - 1);
	controlPoint.y = ofClamp(controlPoint.y, 0, n - 1);
	controlPoints.push_back(controlPoint);
	sort(controlPoints.begin(), controlPoints.end(), xSort);
	update();
}

void CurveLut::set(int i, ofVec2f controlPoint) {
	int m = controlPoints.size();
	int lower = 0, upper = n - 1;
	if(m > 1) {
		if(i > 0) {
			lower = controlPoints[i - 1].x + 1;
		}
		if(i < m - 1) {
			upper = controlPoints[i + 1].x - 1;
		}
	}
	controlPoint.x = ofClamp(controlPoint.x, lower, upper);
	controlPoint.y = ofClamp(controlPoint.y, 0, n - 1);
	controlPoints[i] = controlPoint;
	update();
}

void CurveLut::remove(int i) {
	controlPoints.erase(controlPoints.begin() + i);
	update();
}

// based on code from: http://goo.gl/Y9Ik6
vector<float> CurveLut::getSecondDerivative() {	
	// build the tridiagonal system 
	// (assume 0 boundary conditions: y2[0]=y2[-1]=0) 
	vector<ofVec2f>& c = controlPoints;
	int m = c.size();
	vector< vector<float> > matrix(m, vector<float>(3, 0));
	vector<float> result(m, 0);
	matrix[0][1] = 1;
	for(int i = 1; i < m - 1; i++) {
		matrix[i][0] = (c[i].x-c[i-1].x) / 6;
		matrix[i][1] = (c[i+1].x-c[i-1].x) / 3;
		matrix[i][2] = (c[i+1].x-c[i].x) / 6;
		result[i] = (c[i+1].y-c[i].y)/(c[i+1].x-c[i].x) - (c[i].y-c[i-1].y)/(c[i].x-c[i-1].x);
	}
	matrix[m-1][1]=1;
	
	// solving pass1 (up->down)
	for(int i = 1; i < m; i++) {
		float k = matrix[i][0] / matrix[i-1][1];
		matrix[i][1] -= k * matrix[i-1][2];
		matrix[i][0] = 0;
		result[i] -= k * result[i-1];
	}
	
	// solving pass2 (down->up)
	for(int i = m - 2; i >= 0; i--) {
		float k = matrix[i][2] / matrix[i+1][1];
		matrix[i][1] -= k * matrix[i+1][0];
		matrix[i][2] = 0;
		result[i] -= k * result[i+1];
	}
	
	// return second derivative value for each point P
	vector<float> y2(m);
	for (int i = 0; i < m; i++) {
		y2[i] = result[i] / matrix[i][1];
	}
	return y2;
}


void CurveLut::update() {
	vector<float> sd = getSecondDerivative();
	int m = controlPoints.size();
	ofVec2f& first = controlPoints.front();
	ofVec2f& last = controlPoints.back();
	for(int x = 0; x < first.x; x++) {
		lut[x] = first.y;
	}
	for(int i = 0; i < m - 1; i++) {
		ofVec2f& cur = controlPoints[i];
		ofVec2f& next = controlPoints[i+1];
		for (int x = cur.x; x < next.x; x++) {
			float t = (x-cur.x) / (next.x-cur.x);
			float a = 1-t;
			float b = t;
			float h = next.x-cur.x;
			lut[x] = (a*cur.y + b*next.y + (h*h/6)*((a*a*a-a)*sd[i]+(b*b*b-b)*sd[i+1]));
		}
	}
	for(int x = last.x; x < n; x++) {
		lut[x] = last.y;
	}
	for(int x = 0; x < n; x++) {
		lut[x] = ofClamp(lut[x], 0, n - 1);
	}
	lutNew = true;
}

float CurveLut::operator[](int i) const {
	return lut[i];
}

int CurveLut::size() const {
	return lut.size();
}

int CurveLut::getStart() const {
	return controlPoints.front().x;
}

int CurveLut::getStop() const {
	return controlPoints.back().x;
}

bool CurveLut::isLutNew() {
	bool cur = lutNew;
	lutNew = false;
	return cur;
}
