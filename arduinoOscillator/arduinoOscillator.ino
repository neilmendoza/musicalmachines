#include <SoftwareSerial.h>

#include "StepperOscillator.h"
#include "OneShotOscillator.h"
#include "Constants.h"

int midiByte0 = -1;
int midiByte1  = -1;
int midiByte2  = -1;

#define HI_NIBBLE(b) (((b)>>4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)

#define NOTE_ON_STATUS  B1001
#define NOTE_OFF_STATUS B1000
#define CC_STATUS       B1011
#define PITCHBEND_STATUS B1110

#define MIDI_BAUD_RATE 31250

const unsigned LED_PIN = 13;
const unsigned MIDI_IN_PIN = 10;
const unsigned ARM1_PIN = 11;
const unsigned ARM2_PIN = 12;

StepperOscillator stepper1(CHANNEL1, 3, 2);
StepperOscillator stepper2(CHANNEL2, 5, 4);
OneShotOscillator arm1(CHANNEL3, ARM1_PIN);
OneShotOscillator arm2(CHANNEL4, ARM2_PIN);

SoftwareSerial midiIn(MIDI_IN_PIN, 9); // RX, TX

void setup()
{                
  pinMode(LED_PIN, OUTPUT);     
  midiIn.begin(MIDI_BAUD_RATE);
}

void loop()
{
  Oscillator::elapsedMicros = micros();
  
  // to keep things simple I have made these as 
  // separate variables, however, if these were 
  // in an array you could loop through them
  // for (int i = 0; i < NUM_OSCILLATORS; ++i)
  // {
  //    oscillators[i]->update();
  // }
  arm1.update();
  arm2.update();
  stepper1.update();
  stepper2.update();
  updateMidi();
}

void noteOn(int channel, int note, int vel)
{
  // similarly for these, you could loop through the 
  // oscillators
  stepper1.noteOn(channel, note, vel);
  stepper2.noteOn(channel, note, vel);
  arm1.noteOn(channel, note, vel);
  arm2.noteOn(channel, note, vel);
  digitalWrite(LED_PIN, HIGH);
}

void noteOff(int channel, int note)
{
  stepper1.noteOff(channel, note);
  stepper2.noteOff(channel, note);
  digitalWrite(LED_PIN, LOW);
}

void updateMidi()
{
  int b = midiIn.read();
  if(b != -1)
  { 
    midiByte2 = midiByte1;
    midiByte1 = midiByte0;
    midiByte0 = b;
    
    // only work if we've got a status byte
    // of some sort
    if(!(midiByte2 & B10000000)) return;
    
    int st = HI_NIBBLE(midiByte2);
    int channel = LO_NIBBLE(midiByte2);
   
    // now check to see if we have a midi
    if(st == NOTE_ON_STATUS)
    {
      if(midiByte0 == 0)
      { 
        // if the volume is zero, it's a note off
        noteOff(channel, midiByte1);
      }
      else
      {
        noteOn(channel, midiByte1, midiByte0);  
      }
    }
    else if(st == NOTE_OFF_STATUS)
    {
      noteOff(channel, midiByte1);
#ifdef USING_CC
    }
    else if(st == CC_STATUS)
    {
      cc(channel, midiByte1, midiByte0);
    }
    else if(st==PITCHBEND_STATUS)
    {
      cc(channel, -1, midiByte0);
#endif
    }
  }
}

