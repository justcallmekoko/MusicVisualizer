#include <Arduino.h>
#include <stdio.h>

#include "Patterns.h"
#include "ButtonInput.h"

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip;

Patterns patternObj;

ButtonInput upButton(UP_BUTTON);
ButtonInput downButton(DOWN_BUTTON);
ButtonInput cycleButton(CYCLE_BUTTON);

uint32_t currentTime  = 0;

void setup() {
  Serial.begin(115200);

  delay(500);

  Serial.println(F("\n\nArduino Music Visualizer\n"));

  Serial.println(F("Setting audio..."));
  patternObj.setupAudio();
  
  Serial.println(F("Setting LEDs..."));
  patternObj.setupStrip();

  //patternObj.setupAudio();

  //patternObj.rainbowCycle(20);
  Serial.println("All set: " + (String)patternObj.active_leds);
}

void loop()
{
  currentTime = millis();

  patternObj.initPattern(patternObj.currentPattern, currentTime);
  
  downButton.checkPatternChange();
  upButton.checkPatternChange();
  cycleButton.checkPatternChange();
  
  //patternObj.doubleLevel();

  //patternObj.tracerLevel();
}
