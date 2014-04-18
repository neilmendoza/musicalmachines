#include "testApp.h"

const float testApp::PENTATONIC_FREQS[NUMBER_OF_KEYS] = { 103.8261743950, 116.5409403795, 130.8127826503, 155.5634918610, 174.6141157165, 207.6523487900, 233.0818807590, 261.6255653006, 311.1269837221, 349.2282314330 };

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetFrameRate(60);
    
    ofBackground(255);
    
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
    
    noteFreq = 0.f;
    
    ControlGenerator noteFreqControl = synth.addParameter("noteFreq");
    
    // Here's the actual noise-making object
    Generator tone = SquareWave().freq( noteFreqControl );
    
    // It's just a steady tone until we modulate the amplitude with an envelope
    ControlGenerator envelopeTrigger = synth.addParameter("trigger");
    Generator toneWithEnvelope = tone * ADSR().attack(0.01).decay(1.5).sustain(0).release(0).trigger(envelopeTrigger).legato(true);
    
    synth.setOutputGen( toneWithEnvelope );
    
    doge.loadImage("doge.png");
    
    lastTriggerSecs = -100000.f;
}

//--------------------------------------------------------------
void testApp::update()
{
    vol = ofMap((ofGetElapsedTimef() - lastTriggerSecs) / 1.5f, 0.f, 1.f, 1.f, 0.f, true);
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofEnableAlphaBlending();
    
    float keyWidth = ofGetWidth() / NUMBER_OF_KEYS;
    float w = keyWidth;
    float h = keyWidth * doge.getHeight() / doge.getWidth();
    
    for(int i = 0; i < NUMBER_OF_KEYS; i++)
    {
        ofPushMatrix();
        float hue = i / (float)NUMBER_OF_KEYS;
        ofSetColor(ofFloatColor::fromHsb(hue, 0.8f, 1.f));
        ofTranslate((i + .5f) * keyWidth, 0.75f * ofGetHeight());
        if (vol == 0.f || i != scaleDegree) doge.draw(-.5f * w, -.5f * h, w, h);
        ofPopMatrix();
    }
    
    if (vol > 0.f)
    {
        ofPushMatrix();
        ofSetColor(255);
        ofTranslate((scaleDegree + .5f) * keyWidth, 0.75f * ofGetHeight());
        float dogeScale = ofMap(vol, 0.f, 1.f, 1.f, 3.f);
        doge.draw(-.5f * w * dogeScale, -.5f * h * dogeScale, w * dogeScale, h * dogeScale);
        ofPopMatrix();
    }
    
    if (noteFreq != 0.f)
    {
        ofSetColor(0);
        unsigned x = 0;
        // decay is 1.5 secs
        ofPushMatrix();
        ofSetLineWidth(2.f);
        ofTranslate(0, 0.25f * ofGetHeight());
        
        // step is half a period
        // show 0.1 secs of a waveform in total
        float waveHeight = 0.25 * ofGetHeight() * vol;
        float step = 0.5f * ofGetWidth() / (0.1f * noteFreq);
        while (x < ofGetWidth())
        {
            ofLine(x, 0, x, waveHeight);
            ofLine(x, waveHeight, x + step, waveHeight);
            x += step;
            ofLine(x, waveHeight, x, -waveHeight);
            ofLine(x, -waveHeight, x + step, -waveHeight);
            x += step;
            ofLine(x, -waveHeight, x, 0);
        }
        ofPopMatrix();
    }
}

void testApp::setScaleDegreeBasedOnMouseX()
{
    int newScaleDegree = ofGetMouseX() * NUMBER_OF_KEYS / ofGetWindowWidth();
    if(ofGetMousePressed() && ( newScaleDegree != scaleDegree ))
    {
        scaleDegree = newScaleDegree;
        trigger();
    }
    else
    {
        scaleDegree = newScaleDegree;
    }
}

void testApp::trigger()
{
    scaleDegree = ofGetMouseX() * NUMBER_OF_KEYS / ofGetWindowWidth();
    int degreeToTrigger = floor(ofClamp(scaleDegree, 0, 9));
    
    noteFreq = PENTATONIC_FREQS[degreeToTrigger];
    
    // set a parameter that we created when we defined the synth
    synth.setParameter("noteFreq", noteFreq);
    
    // simply setting the value of a parameter causes that parameter to send a "trigger" message to any
    // using them as triggers
    synth.setParameter("trigger", 1);
    
    lastTriggerSecs = ofGetElapsedTimef();
}

void testApp::audioRequested(float* output, int bufferSize, int nChannels)
{
    synth.fillBufferOfFloats(output, bufferSize, nChannels);
}

void testApp::mousePressed(int x, int y, int button)
{
    trigger();
}

void testApp::mouseMoved(int x, int y )
{
}

void testApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}



//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
