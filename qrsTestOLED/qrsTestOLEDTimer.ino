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

float NSRarray[] = { 511, 511, 511, 561, 581, 581, 561, 511, 511, 511, 511, 511, 481, 441, 421,
                     1011, 1023, 1011, 511, 421, 441, 481, 511, 511, 511,
                     511, 521, 531, 541, 561, 571, 581, 581, 571, 541, 511, 511 };

int NSRarrayMax = sizeof(NSRarray) / sizeof(NSRarray[0]);
int arrayPos = 0;
int oldPixel = 32;

void setup() {
  analogWriteResolution(10);  // Set analog out resolution to max, 10-bits = 0 - 1023
  analogReadResolution(10);   // Set analog input resolution to max, 10-bits = 1024

  u8g2.begin();
  Serial.begin(9600);
}

void loop() {
  timer.start();
  for (arrayPos = 0; arrayPos < NSRarrayMax; arrayPos++) {
    analogWrite(DAC_PIN, NSRarray[arrayPos]);
    int newPixel = map(NSRarray[arrayPos], 0, 1023, 63, 0);
    u8g2.drawLine(arrayPos, oldPixel, arrayPos + 1, newPixel);
    u8g2.sendBuffer();
    oldPixel = newPixel;

    //float voltage = (analogRead(ADC_PIN) / 1023.0) * 3.3;

    //Serial.println(voltage);  // Print the voltage.
  }
  timer.stop();
  Serial.print("time elapsed ms: ");
  Serial.println(timer.read());
  for (int line = arrayPos; line < arrayPos + 21; line++) {
    analogWrite(DAC_PIN, 511);
    u8g2.drawPixel(line, 32);
    u8g2.sendBuffer();
  }
  delay(250);
  u8g2.clearDisplay();
}