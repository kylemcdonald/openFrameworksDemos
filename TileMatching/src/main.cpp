#include "ofMain.h"

float getBrightness(const ofPixels& pix, int x, int y, int w, int h) {
    float brightness = 0;
    for(int j = y; j < y + h; j++) {
        for(int i = x; i < x + w; i++) {
            brightness += pix.getColor(i, j).getBrightness();
        }
    }
    brightness /= (w * h);
    return brightness;
}

class Tile {
public:
    int x, y, side;
    float brightness;
    Tile(int x, int y, int side, float brightness)
    :x(x)
    ,y(y)
    ,side(side)
    ,brightness(brightness) {
    }
    static vector<Tile> buildTiles(const ofPixels& pix, int x, int y, int w, int h, int side) {
        int nx = w / side, ny = h / side;
        vector<Tile> tiles;
        for(int j = y; j < y + h; j+=side) {
            for(int i = x; i < x + w; i+=side) {
                float brightness = getBrightness(pix, i, j, side, side);
                // try the direction of the brightness (overall gradient)
                tiles.push_back(Tile(i, j, side, brightness));
            }
        }
        return tiles;
    }
    
};

bool TileCompare(const Tile& a, const Tile& b) {
    return a.brightness < b.brightness;
}

class ofApp : public ofBaseApp {
public:
    ofVideoGrabber video;
    vector<Tile> leftTiles, rightTiles;
    int side = 40;
    void setup() {
        video.initGrabber(1280, 720);
    }
    void update() {
        video.update();
        if(video.isFrameNew()) {
            int w = video.getWidth();
            int h = video.getHeight();
            ofPixels& pix = video.getPixels();
            leftTiles = Tile::buildTiles(pix, 0, 0, w / 2, h, side);
            rightTiles = Tile::buildTiles(pix, w / 2, 0, w / 2, h, side);
            ofSort(leftTiles, TileCompare);
            ofSort(rightTiles, TileCompare);
        }
    }
    void draw() {
        int n = leftTiles.size();
        ofTexture& tex = video.getTexture();
        for(int i = 0; i < n; i++) {
            Tile& left = leftTiles[i];
            Tile& right = rightTiles[i];
            tex.drawSubsection(left.x, left.y, side, side, right.x, right.y);
            tex.drawSubsection(right.x, right.y, side, side, left.x, left.y);
        }
        ofSetWindowTitle(ofToString((int) ofGetFrameRate()));
    }
};
int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
