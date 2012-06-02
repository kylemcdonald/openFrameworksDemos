#pragma once

class Body {
public:
	float x, y;
	float mass;
	Body(float _x = 0, float _y = 0, float _mass = 0) :
		x(_x), y(_y), mass(_mass) {
	}
};
