#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 640, 360 * 2, OF_WINDOW);
	ofRunApp(new ofApp());
}
