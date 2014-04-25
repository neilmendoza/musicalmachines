#pragma once

#include "ofMain.h"
#include "ofxTonic.h"

// simple waveform illustration based on
// https://github.com/TonicAudio/ofxTonic/tree/master/example_SimpleInstrument

using namespace Tonic;

class testApp : public ofBaseApp
{
public:
    static const unsigned NUMBER_OF_KEYS = 10;
    static const float PENTATONIC_FREQS[NUMBER_OF_KEYS];
    
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void audioRequested(float* output, int bufferSize, int nChannels);
    
private:
    void trigger();
    void setScaleDegreeBasedOnMouseX();
    
    float vol;
    ofImage doge;
    float lastTriggerSecs;
    ofxTonicSynth synth;
    unsigned scaleDegree;
    float noteFreq;
};
