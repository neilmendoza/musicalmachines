#include <SoftwareSerial.h>

SoftwareSerial phone(2, 3);

unsigned long lastCallTime = 0;
String buffer;

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
  // check for calls
  bool callReceived = checkForCalls();
  
  // if we got a call send a message to the computer
  if (callReceived)
  {
    Serial.println("I got a call :)");
  }
}

bool checkForCalls()
{
    bool callReceived = false;
    if (phone.available())
    {
        char newChar = (char)phone.read();
        
        if (isNewLine(newChar))
        {
            if (!buffer.equals("") && !buffer.equals(" "))
            {
                Serial.println(buffer);
                if (buffer.indexOf("RING") != -1)
                {
                    lastCallTime = millis();
                    callReceived = true;
                }
            }
            buffer = "";
        }
        else 
        {
            buffer = buffer + newChar;
        }
    }
    return callReceived;
}

bool isNewLine(char c)
{
    return c == '\n' || c == '\r' || c == '\t';
}

