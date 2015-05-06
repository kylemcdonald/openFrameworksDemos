#include "ofMain.h"

class LineRenderer {
private:
    ofShader shader;
    
#define stringify(x) #x
    
    string frag =
    stringify(
        const float pi = 3.1415926536;
        uniform float feather;
        uniform float taper;
        uniform float lineLength;
        uniform float lineWidth;
        uniform float dashSpacing;
        uniform float dashWidth;
        void main() {
            vec2 texCoord = gl_TexCoord[0].st;
            float alpha = 1;
            
            // determine alpha
            if(dashWidth < lineLength) {
                float x = texCoord.x * lineLength;
                float modWidth = mod(x, dashSpacing);
                if(modWidth < feather) {
                    alpha = modWidth / feather;
                } else if(modWidth > dashWidth) {
                    alpha = 1 - ((modWidth - dashWidth) / feather);
                    alpha = max(0, alpha);
                }
            }
            
            // determine taper
            if(taper < 1) {
                float threshold = mix(sin(pi * texCoord.x), 1, taper);
                float offset = abs((2 * texCoord.y) - 1);
                float taperAlpha = (threshold - offset) / (2 * feather / lineWidth);
                alpha = alpha * clamp(taperAlpha, 0, 1);
            }
            
            if(alpha == 0) discard;
            gl_FragColor = vec4(gl_Color.rgb, alpha);
        }
        );
    
    bool centered;
    bool attached;
    float feather, dashWidth, dashSpacing, taper, lineWidth;
    
public:
    LineRenderer()
    :attached(false)
    ,centered(true)
    ,feather(1)
    ,dashWidth(0)
    ,dashSpacing(0)
    ,taper(1)
    ,lineWidth(2) {
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, "#version 120\n" + frag);
        shader.linkProgram();
    }
    // to draw pixel-precise odd-width lines (e.g., 1 or 3 or 11 pixels wide) set centered = false
    static void drawLine(const ofVec2f& a, const ofVec2f& b, float width, bool centered = true) {
        ofVec2f normal = (b - a).rotateRad(HALF_PI).normalize();
        ofVboMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        mesh.addTexCoord(ofVec2f(0, 0));
        mesh.addTexCoord(ofVec2f(1, 0));
        mesh.addTexCoord(ofVec2f(1, 1));
        mesh.addTexCoord(ofVec2f(0, 1));
        if(centered) {
            normal *= width / 2;
            mesh.addVertex(a - normal);
            mesh.addVertex(b - normal);
            mesh.addVertex(b + normal);
            mesh.addVertex(a + normal);
        } else {
            ofVec2f up = normal * ((width / 2) + .5);
            ofVec2f down = normal * ((width / 2) - .5);
            mesh.addVertex(a - up);
            mesh.addVertex(b - up);
            mesh.addVertex(b + down);
            mesh.addVertex(a + down);
        }
        mesh.draw();
    }
    LineRenderer& setCentered(bool centered) {
        this->centered = centered;
        return *this;
    }
    LineRenderer& setFeather(float feather) {
        this->feather = feather;
        return *this;
    }
    LineRenderer& setWidth(float lineWidth) {
        this->lineWidth = lineWidth;
        return *this;
    }
    LineRenderer& setDash(float dashSpacing = 0, float dashWidth = 0) {
        this->dashSpacing = dashSpacing;
        this->dashWidth = dashWidth;
        return *this;
    }
    LineRenderer& setTaper(float taper) {
        this->taper = taper;
        return *this;
    }
    bool getDash() const {
        return dashSpacing > 0 & dashWidth > 0;
    }
    LineRenderer& begin() {
        shader.begin();
        attached = true;
        return *this;
    }
    LineRenderer& draw(const ofVec2f& a, const ofVec2f& b) {
        if(!attached) {
            shader.begin();
        }
        float lineLength = a.distance(b);
        shader.setUniform1f("feather", feather);
        shader.setUniform1f("taper", taper);
        shader.setUniform1f("lineLength", lineLength);
        shader.setUniform1f("lineWidth", lineWidth);
        if(getDash()) {
            shader.setUniform1f("dashSpacing", dashSpacing);
            shader.setUniform1f("dashWidth", dashWidth);
        } else {
            shader.setUniform1f("dashSpacing", lineLength);
            shader.setUniform1f("dashWidth", lineLength);
        }
        drawLine(a, b, lineWidth, centered);
        if(!attached) {
            shader.end();
        }
        return *this;
    }
    LineRenderer& end() {
        shader.end();
        attached = false;
        return *this;
    }
};

class ofApp : public ofBaseApp {
public:
    LineRenderer lineRenderer;
    void setup() {
        ofBackground(0);
    }
    void update() {
    }
    void draw() {
        ofSetColor(255);
        int spacing = 20;
        for(int i = 0; i < ofGetHeight(); i += spacing) {
            LineRenderer::drawLine(ofVec2f(0, i), ofVec2f(ofGetWidth(), i), 1, false);
        }
        for(int i = 0; i < ofGetWidth(); i += spacing) {
            LineRenderer::drawLine(ofVec2f(i, 0), ofVec2f(i, ofGetHeight()), 1, false);
        }
        lineRenderer
        .begin()
        .setWidth(1)
        .setTaper(1)
        .draw(ofVec2f(0, ofGetHeight()), ofVec2f(mouseX, mouseY))
        .setDash(25, 20)
        .setWidth(20)
        .draw(ofVec2f(0, 0), ofVec2f(mouseX, mouseY))
        .setTaper(0)
        .setDash(8, 4)
        .setWidth(50)
        .draw(ofVec2f(ofGetWidth(), 0), ofVec2f(mouseX, mouseY))
        .setDash(0, 0)
        .draw(ofVec2f(ofGetWidth(), ofGetHeight()), ofVec2f(mouseX, mouseY))
        .end();
    }
};
int main() {
    ofSetupOpenGL(800, 800, OF_WINDOW);
    ofRunApp(new ofApp());
}