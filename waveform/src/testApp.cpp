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
    
    // Let's put a filter on the tone
    
    // It's just a steady tone until we modulate the amplitude with an envelope
    ControlGenerator envelopeTrigger = synth.addParameter("trigger");
    Generator toneWithEnvelope = tone * ADSR().attack(0.01).decay(1.5).sustain(0).release(0).trigger(envelopeTrigger).legato(true);
    
    // let's send the tone through some delay
    //Generator toneWithDelay = StereoDelay(0.5, 0.75).input(toneWithEnvelope).wetLevel(0.1).feedback(0.2);
    
    synth.setOutputGen( toneWithEnvelope );
    
    doge.loadImage("doge.png");
}

//--------------------------------------------------------------
void testApp::update()
{
}

//--------------------------------------------------------------
void testApp::draw()
{
    float keyWidth = ofGetWindowWidth() / NUMBER_OF_KEYS;
    for(int i = 0; i < NUMBER_OF_KEYS; i++)
    {
        float hue = i / (float)NUMBER_OF_KEYS;
        float saturation = 0.8f;
        if ((i == scaleDegree) && ofGetMousePressed() ) saturation = 0.f;
        ofSetColor(ofFloatColor::fromHsb(hue, saturation, 1.f));
        doge.draw(i * keyWidth, 0.75 * ofGetHeight(), keyWidth, keyWidth * doge.getHeight() / doge.getWidth());
    }
    
    if (noteFreq != 0.f)
    {
        ofSetColor(0);
        unsigned x = 0;
        // decay is 1.5 secs
        float vol = ofMap((ofGetElapsedTimef() - lastTriggerSecs) / 1.5f, 0.f, 1.f, 0.25f * ofGetHeight(), 0.f, true);
        ofPushMatrix();
        ofSetLineWidth(2.f);
        ofTranslate(0, 0.25f * ofGetHeight());
        
        // step is half a period
        // show 0.1 secs of a waveform in total
        float step = 0.5f * ofGetWidth() / (0.1f * noteFreq);
        while (x < ofGetWidth())
        {
            ofLine(x, 0, x, vol);
            ofLine(x, vol, x + step, vol);
            x += step;
            ofLine(x, vol, x, -vol);
            ofLine(x, -vol, x + step, -vol);
            x += step;
            ofLine(x, -vol, x, 0);
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

void testApp::mouseMoved(int x, int y )
{
    setScaleDegreeBasedOnMouseX();
}

void testApp::mouseDragged(int x, int y, int button)
{
    setScaleDegreeBasedOnMouseX();
}

void testApp::mousePressed(int x, int y, int button)
{
    trigger();
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
