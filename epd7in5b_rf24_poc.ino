/**
 *  @filename   :   epd7in5-demo.ino
 *  @brief      :   7.5inch e-paper display demo
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 25 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <SPI.h>
#include "epd7in5b.h"
#include "imagedata.h"
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN

long xoffset = 0;
Epd epd;

const byte address[6] = "00001";

volatile bool buttonA = false;
void handleButtonA()
{
  buttonA = true;
}

volatile bool buttonB = false;
void handleButtonB()
{
  buttonB = true;
}


void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);

    pinMode(A1, OUTPUT);
    digitalWrite(A1, HIGH); 

    if (epd.Init() != 0) {
        Serial.print("e-Paper init failed");
        return;
    }

    epd.Clean();

    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH); 

    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MAX);
    radio.startListening();

    attachInterrupt(digitalPinToInterrupt(2), handleButtonA, RISING);
    attachInterrupt(digitalPinToInterrupt(3), handleButtonB, RISING);
}

enum Command : byte
{
  BeginFrame = 0,
  SetPixels = 1,
  PresentFrame = 2,
  ClearScreen = 3,
};

struct Message
{
  Command cmd;
  byte dataSize;
  byte data[30];
};

void loop() 
{
  if (buttonA)
  {
    epd.Clean();
    buttonA = false; 
    return;
  }
  
  while (radio.available())
  {
    Message msg = {};
    radio.read(&msg, sizeof(Message));
    switch (msg.cmd)
    {
      case Command::BeginFrame:
        epd.BeginFrame();
        break;
      case Command::PresentFrame:
        epd.PresentFrame();
        break;
      case Command::ClearScreen:
        epd.Clean();
        break;
      case Command::SetPixels:
        for (long i = 0; i < msg.dataSize; i+=2)
        {
          epd.SetNextTwoPixels(&msg.data[i]);
        }
        break;
    }
  }
}
