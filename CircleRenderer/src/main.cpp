#include "ofMain.h"

#define STRINGIFY(A) #A
string shaderProgram = STRINGIFY(
uniform sampler2DRect spriteTex;
uniform sampler2DRect colorTex;
uniform vec2 colorSize;
void main(void) {
	vec4 spriteTxt = texture2DRect(spriteTex, gl_TexCoord[0].st);
	vec4 colorTxt = texture2DRect(colorTex, gl_Color.rg * colorSize);
	gl_FragColor = spriteTxt * colorTxt;
}
);

int n = 32, m = 24;
int circleSize = 10;

class ofApp : public ofBaseApp {
public:
	ofFbo smaller;
	ofVideoPlayer video;
	ofImage circle;
	ofMesh mesh;
	ofShader shader;
	void setup() {
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
		shader.linkProgram();
		video.loadMovie("fingers.mov");
		video.play();
		smaller.allocate(n, m, GL_RGB);
		circle.loadImage("circle.png");
		mesh.setMode(OF_PRIMITIVE_TRIANGLES);
		ofVec2f smallSize(n, m);
		ofVec2f circleTexSize = circle.getTextureReference().getCoordFromPercent(1, 1);
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < m; j++) {
				ofVec2f nwv(i, j), nev(i + 1, j), sev(i + 1, j + 1), swv(i, j + 1);
				mesh.addVertex(nwv);
				mesh.addVertex(nev);
				mesh.addVertex(sev);
				mesh.addVertex(nwv);
				mesh.addVertex(sev);
				mesh.addVertex(swv);
				
				ofVec2f nwt(0, 0), net(circleTexSize.x, 0), set(circleTexSize), swt(0, circleTexSize.y);
				mesh.addTexCoord(nwt);
				mesh.addTexCoord(net);
				mesh.addTexCoord(set);
				mesh.addTexCoord(nwt);
				mesh.addTexCoord(set);
				mesh.addTexCoord(swt);
				
				ofVec2f	nwc(nwv / smallSize),	nec(nev / smallSize),	sec(sev / smallSize), swc(swv / smallSize);
				mesh.addColor(ofFloatColor(nwc.x, nwc.y, 0));
				mesh.addColor(ofFloatColor(nec.x, nec.y, 0));
				mesh.addColor(ofFloatColor(sec.x, sec.y, 0));
				mesh.addColor(ofFloatColor(nwc.x, nwc.y, 0));
				mesh.addColor(ofFloatColor(sec.x, sec.y, 0));
				mesh.addColor(ofFloatColor(swc.x, swc.y, 0));
			}
		}
	}
	void update() {
		video.update();
		smaller.begin();
		video.draw(0, 0, n, m);
		smaller.end();
	}
	void draw() {
		ofBackground(0);
		//video.draw(0, 0);
		//smaller.draw(0, 0);
		ofScale(video.getWidth() / smaller.getWidth(), video.getHeight() / smaller.getHeight());
		shader.begin();
		shader.setUniformTexture("spriteTex", circle.getTextureReference(), 0);
		shader.setUniformTexture("colorTex", smaller.getTextureReference(), 1);
		ofVec2f smallerTexSize = smaller.getTextureReference().getCoordFromPercent(1, 1);
		shader.setUniform2f("colorSize", smallerTexSize.x, smallerTexSize.y);
		mesh.draw();
		shader.end();
	}
};

int main( ){
	ofSetupOpenGL(320, 240, OF_WINDOW);
	ofRunApp(new ofApp());
}