#include "Patterns.h"

Patterns::Patterns()
{
  
}

void Patterns::initPattern(uint8_t pattern)
{
  if (pattern == SINGLE) {
    this->singleLevel();
  }
  else if (pattern == DOUBLE) {
    this->doubleLevel();
  }
  else if (pattern == TRACER) {
    this->tracerLevel();
  }
}

void Patterns::convertSingle()
{
  if (this->left[this->freq] > this->right[this->freq])
    this->audio_input = this->left[this->freq];
  else
    this->audio_input = this->right[this->freq];

  if (this->audio_input > DOUBLE_THRESHOLD)
  {
    this->pre_react = ((long)this->active_leds * (long)this->audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs

    if (this->pre_react > this->react) // ONLY ADJUST LEVEL OF LED IF LEVEL HIGHER THAN CURRENT LEVEL
      this->react = this->pre_react;
  }
}

void Patterns::convertDouble()
{
  if (this->left[this->freq] > this->right[this->freq])
    this->audio_input = this->left[this->freq];
  else
    this->audio_input = this->right[this->freq];

  if (this->audio_input > DOUBLE_THRESHOLD) {
    this->pre_react = ((long)this->midway * (long)this->audio_input) / 1023L;

    if (this->pre_react > this->react)
      this->react = this->pre_react;
  }
  
}

void Patterns::displayTracer()
{
  for (int i = 0; i < this->active_leds; i++) {
    if (this->tracerList[i] != 0)
      strip.setPixelColor(i, Wheel((i * 256 / 50 + this->k) % 256));
    else
      strip.setPixelColor(i, strip.Color(25, 0, 0));
  }

  strip.show();
}

void Patterns::singleLevel()
{
  this->readMSGEQ7();
  this->convertSingle();
  this->singleRainbow();

  this->k = this->k - this->wheel_speed;
  if (this->k < 0)
    this->k = 255;

  this->decay_check++;
  if (this->decay_check > this->decay) {
    this->decay_check = 0;
    if (this->react > 0)
      this->react--;
  }
}

void Patterns::doubleLevel()
{
  this->readMSGEQ7();
  this->convertDouble();
  this->doubleRainbow();

  this->k = this->k - this->wheel_speed;
  if (this->k < 0)
    this->k = 255;

  this->decay_check++;
  if (this->decay_check > this->decay) {
    this->decay_check = 0;
    if (this->react > 0)
      this->react--;
  }
}

void Patterns::tracerLevel()
{
  this->readMSGEQ7();

  if (this->left[this->freq] > this->right[this->freq])
    this->audio_input = this->left[this->freq];
  else
    this->audio_input = this->right[this->freq];

  if (this->audio_input > TRACER_THRESHOLD) {
    //Serial.println("audio_input: " + (String)audio_input);
    this->tracerList[0] = 1;
  }
  else
    this->tracerList[0] = 0;

  this->shiftTracer();
  this->displayTracer();
}

void Patterns::shiftTracer()
{
  for (int i = this->active_leds; i >= 0; i--) {
    //if ((this->active_leds != 0) && (i != active_leds)) {
    this->tracerList[i + 1] = this->tracerList[i];
    this->tracerList[i] = 0;
    //}
  }
}

void Patterns::singleRainbow()
{
  for(int i = this->active_leds - 1; i >= 0; i--) {
    if (i < this->react)
      strip.setPixelColor(i, Wheel((i * 256 / 50 + this->k) % 256));
    else
      strip.setPixelColor(i, strip.Color(0, 0, 0));      
  }
  strip.show();
}

void Patterns::doubleRainbow()
{
  for (int i = this->active_leds - 1; i >= this->midway; i--) {
    if (i < this->react + this->midway) {
      strip.setPixelColor(i, Wheel((i * 256 / 50 + this->k) % 256));
      //strip.setPixelColor((this->midway - i) + this->midway, Wheel(((this->active_leds / (i + 1)) + this->k) & 255));
      strip.setPixelColor((this->midway - i) + this->midway, Wheel((i * 256 / 50 + this->k) % 256));
    }
    else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.setPixelColor(this->midway - this->react, strip.Color(0, 0, 0));
    }
  }
  strip.show();
}

void Patterns::resetWheel()
{
  for (uint16_t l = 0; l < this->active_leds; l++) {
    strip.setPixelColor(l, strip.Color(0, 0, 0));
  }
  strip.show();
}

void Patterns::readMSGEQ7()
{
  digitalWrite(reset, HIGH);
  digitalWrite(reset, LOW);
  for (this->band = 0; this->band < 7; this->band++) {
    digitalWrite(strobe, LOW);
    delayMicroseconds(30);
    this->left[this->band] = analogRead(audio1);
    this->right[this->band] = analogRead(audio2);
    digitalWrite(strobe, HIGH);
  }
}

void Patterns::setupAudio()
{
  pinMode(audio1, INPUT);
  pinMode(audio2, INPUT);
  pinMode(strobe, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(reset, LOW);
  digitalWrite(strobe, HIGH);
}

void Patterns::setupStrip()
{
  for (int i = 0; i < this->active_leds; i++)
    this->tracerList[i] = 0;
    
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  for (int i = 0; i < this->active_leds; i++)
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  strip.show();
}

uint32_t Patterns::Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
