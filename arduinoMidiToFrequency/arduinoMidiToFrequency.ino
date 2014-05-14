#include <SoftwareSerial.h>

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
const unsigned STEP_PIN = 3;
const unsigned DIR_PIN = 2;
const unsigned MIDI_IN_PIN = 10;

SoftwareSerial midiIn(MIDI_IN_PIN, 9); // RX, TX

unsigned long halfPeriodMicros = 0;

void setup()
{                
  pinMode(LED_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 
  pinMode(DIR_PIN, OUTPUT); 
  
  digitalWrite(LED_PIN, LOW); 
  digitalWrite(STEP_PIN, LOW); 
  digitalWrite(DIR_PIN, LOW); 
  
  midiIn.begin(MIDI_BAUD_RATE);
}

void loop()
{
  if (halfPeriodMicros != 0)
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(halfPeriodMicros);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(halfPeriodMicros);
  }
}

double midiToFrequency(int midiNote)
{
  return 440.0 * exp(0.057762265 * (midiNote - 69.0));
}

void noteOn(int channel, int midiNote, int vel)
{
  halfPeriodMicros = 0.5 * 1000000.0 / midiToFrequency(midiNote);
  digitalWrite(LED_PIN, HIGH);
}

void noteOff(int channel, int midiNote)
{
  halfPeriodMicros = 0;
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
    }
  }
}

