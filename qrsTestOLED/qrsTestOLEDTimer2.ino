#include <Arduino.h>
#include <U8g2lib.h>
#include "Timer.h"

#define DAC_PIN A0  // Make code a bit more legible
#define ADC_PIN A2

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
Timer timer;

int bpmSpeed[] = {2250, 2000, 1500, 1250, 1000, 750, 500};

float NSRarray[] = { 511, 511, 511, 561, 581, 581, 561, 511, 511, 511, 511, 511, 481, 441, 421,
                     1011, 1023, 1011, 511, 421, 441, 481, 511, 511, 511,
                     511, 521, 531, 541, 561, 571, 581, 581, 571, 541, 511, 511 };

int NSRarrayMax = sizeof(NSRarray) / sizeof(NSRarray[0]);
int arrayPos = 0;
int oldPixel = 32;
int bpm = 1000; // 1000ms is equal to 1 second

void setup() {
  analogWriteResolution(10);  // Set analog out resolution to max, 10-bits = 0 - 1023
  analogReadResolution(10);   // Set analog input resolution to max, 10-bits = 1024

  u8g2.begin();
  Serial.begin(9600);
  drawQRS();
  drawBaseline();
}

void loop() {
  timer.start();
  for (arrayPos = 0; arrayPos < NSRarrayMax; arrayPos++) {
    analogWrite(DAC_PIN, NSRarray[arrayPos]);
    int newPixel = map(NSRarray[arrayPos], 0, 1023, 63, 0);
    delay(13);
    float voltage = (analogRead(ADC_PIN) / 1023.0) * 3.3;
  }
  timer.stop();
  int qrsTime = timer.read();
  int baselineTime = bpm - qrsTime;
  delay(baselineTime);
  Serial.print("QRS time ms: ");
  Serial.println(qrsTime);
  Serial.print("delay time ms: ");
  Serial.println(baselineTime);
  Serial.print("bpm: ");
  Serial.println((bpmMilliSeconds/1000)*60);
}

void drawQRS() {
  for (arrayPos = 0; arrayPos < NSRarrayMax; arrayPos++) {
    int newPixel = map(NSRarray[arrayPos], 0, 1023, 63, 0);
    u8g2.drawLine(arrayPos, oldPixel, arrayPos + 1, newPixel);
    u8g2.sendBuffer();
    oldPixel = newPixel;
  }
}

void drawBaseline() {
    for (int line = arrayPos; line < NSRarrayMax+21; line++) {
    u8g2.drawPixel(line, 32);
    u8g2.sendBuffer();
  }
}
