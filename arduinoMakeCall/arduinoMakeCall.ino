#include <SoftwareSerial.h>

const String PHONE_NUMBER = "07956641707";

SoftwareSerial phone(2, 3);

void setup()
{
  // start talking to phone over serial
  phone.begin(9600);
  
  // start talking to computer over serial
  // to receive trigger and send debug
  // messages
  Serial.begin(9600);
}

void loop()
{
}

void serialEvent()
{
  // received a serial event call number
  if (Serial.available())
  {
    // prepare the AT command
    String command("ATD");
    command += PHONE_NUMBER;
    command += ";";
    
    // send it to the phone
    phone.println(command);
    
    // debug message
    Serial.print("Sent AT command: ");
    Serial.println(command);
    
    // clear the serial buffer and wait for
    // next time
    Serial.flush();
  }
}
