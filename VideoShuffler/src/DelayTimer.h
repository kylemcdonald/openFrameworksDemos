#pragma once

#include "ofMain.h"

/*
 DelayTimer is a synchronous timer, which means you can ask it
 whether it's time to do something again yet.
 Set it up with setFramerate() or setPeriod() (in seconds).
 Then call tick() to find out whether the timer has ticked
 since the last time you checked.

 setUseBuffer(true) means tick() will return true multiple times
 when multiple ticks have passed without you checking.
 */

class DelayTimer {
protected:
	float period;
	float lastTime;
	int ticks;
	bool useBuffer;
public:
	DelayTimer() :
		period(0),
		lastTime(0),
		ticks(0),
		useBuffer(false) {
	};
	void setFramerate(float framerate) {
		period = 1 / framerate;
	}
	void setPeriod(float period) {
		this->period = period;
	}
	float getPeriod() {
		return period;
	}
	void setUseBuffer(bool useBuffer) {
		this->useBuffer = useBuffer;
	}
	bool tick() {
		float curTime = ofGetElapsedTimef();
		int curTicks = (int) (curTime / period);
		int lastTicks = (int) (lastTime / period);
		lastTime = curTime;
		ticks += curTicks - lastTicks;
		if(ticks > 0) {
			if(useBuffer) {
				ticks--;
			} else {
				ticks = 0;
			}
			return true;
		} else {
			return false;
		}
	}
};
