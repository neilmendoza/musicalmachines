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

const int LED = 13;
const int MIDI_IN = 10;
const int ARM1 = 11;
const int ARM2 = 12;

class Oscillator
{
public:
  static unsigned long elapsedMicros;
  
  Oscillator() : periodMicros(0), halfPeriodMicros(0) {}
  virtual ~Oscillator() {}
  
  virtual void risingEdge() {}
  virtual void fallingEdge() {}
  
  virtual void noteOn(int midiNote)
  {
    periodMicros = 1000000.0 / midiToFrequency(midiNote);
    halfPeriodMicros = 0.5 * periodMicros;
  };
  
  virtual void noteOff(int midiNote)
  {
    periodMicros = 0;
    halfPeriodMicros = 0;
  };
  
  virtual void update()
  {
    if(periodMicros > 0)
    {
        unsigned long elapsedPeriodMicros = elapsedMicros % periodMicros;
        
        if(elapsedPeriodMicros > halfPeriodMicros != !wave)
        {
          // wave form has flipped
          wave = !wave;
          if (wave) risingEdge();
          else fallingEdge();
        }
    }
  };

private:
  
  double midiToFrequency(int midiNote)
  {
    return 440.0 * exp(0.057762265 * (midiNote - 69.0));
  }
  
  unsigned int periodMicros;
  unsigned int halfPeriodMicros;
  bool wave;
};

unsigned long Oscillator::elapsedMicros;
  
class StepperOscillator : public Oscillator
{
public:
  StepperOscillator(unsigned stepPin, unsigned dirPin) : stepPin(stepPin), dirPin(dirPin), dir(LOW)
  {
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    digitalWrite(dirPin, dir);
  }
  
  void noteOff(int midiNote)
  {
    Oscillator::noteOff(midiNote);
    dir = !dir;
    digitalWrite(dirPin, dir);
  }
  
  void risingEdge()
  {
    digitalWrite(stepPin, HIGH);
  }
  
  void fallingEdge()
  {
    digitalWrite(stepPin, LOW);
  }
  
private:
  bool dir;
  unsigned stepPin, dirPin;
};

class OneShotOscillator : public Oscillator
{
public:
  OneShotOscillator(unsigned pin, float holdSecs = 0.1f) : 
    pin(pin), holdMicros(1e6 * holdSecs), startTimeMicros(0), Oscillator()
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  void noteOn(int midiNote)
  {
    startTimeMicros = elapsedMicros;
    digitalWrite(pin, HIGH);
  }
  
  void update()
  {
    if (startTimeMicros)
    {
      if (elapsedMicros - startTimeMicros > holdMicros)
      {
        digitalWrite(pin, LOW);
        startTimeMicros = 0;
      }
    }
  }
  
private:
  unsigned long startTimeMicros;
  unsigned holdMicros;
  unsigned pin;
};

StepperOscillator stepper(3, 2);
OneShotOscillator arm1(ARM1);
OneShotOscillator arm2(ARM2);

SoftwareSerial midiIn(MIDI_IN, 9); // RX, TX


void setup()
{                
  pinMode(LED, OUTPUT);     
  midiIn.begin(MIDI_BAUD_RATE);
}

void loop()
{
  Oscillator::elapsedMicros = micros();
  stepper.update();
  arm1.update();
  arm2.update();
  updateMidi();
}

void noteOn(int channel, int note, int vel)
{
  stepper.noteOn(note);
  //arm1.noteOn(note);
  //arm2.noteOn(note);
  digitalWrite(LED, HIGH);
}

void noteOff(int channel, int note)
{
  stepper.noteOff(note);
  digitalWrite(LED, LOW);
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
   
    // channel += 1; // we're doing this with defines
   
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

