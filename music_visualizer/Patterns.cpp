#include "Patterns.h"

Patterns::Patterns()
{

}

void Patterns::saveEEPROM(long addr, int value, bool fix)
{

  if (fix) {
    int modu = value % 255; // the remainder of value after deviding by 255
    int whole_mult = (value - modu) / 255; // The number of times 255 goes into the value

    EEPROM.update(addr, modu);
    EEPROM.update(MOD_ADDR, whole_mult);

    Serial.println("Saved remainder " + (String)modu + " to addr " + (String)addr);
    Serial.println("Saved multiplier " + (String)whole_mult + " to addr " + (String)MOD_ADDR);
  }
  else {
    EEPROM.update(addr, value);
    Serial.println("Saved value " + (String)value + " to addr " + (String)addr);
  }
}

void Patterns::loadActiveLEDs()
{
  this->active_leds = EEPROM.read(ACTIVE_LED_ADDR);
  if ((this->active_leds < 0) || (this->active_leds > NUM_LEDS)) {
    this->active_leds = NUM_LEDS;
    Serial.println("Loaded active_leds from default: " + (String)this->active_leds);
    this->saveEEPROM(ACTIVE_LED_ADDR, this->active_leds);
  }
  else
    Serial.println("Loaded active_leds from EEPROM: " + (String)this->active_leds);
}

void Patterns::loadTracerThresh()
{
  // Need to calculate tracer threshold because we cannot
  // store numbers greater than 255 in EEPROM
  // tracer threshold is calculated using the int
  // stored in (MOD_ADDR * 255) + the int stored in TRACER_THRESH_ADDR
  int remainder = EEPROM.read(TRACER_THRESH_ADDR);
  int multiplier = EEPROM.read(MOD_ADDR);
  this->TRACER_THRESHOLD = (multiplier * 255) + remainder;
  if ((this->TRACER_THRESHOLD < 0) || (this->TRACER_THRESHOLD > 1023)){
    this->TRACER_THRESHOLD = DEFAULT_TRACER_THRESH;
    Serial.println("Loaded TRACER_THRESHOLD from default: " + (String)this->TRACER_THRESHOLD);
    this->saveEEPROM(TRACER_THRESH_ADDR, this->TRACER_THRESHOLD);
  }
  else
    Serial.println("Loaded TRACER_THRESHOLD from EEPROM: " + (String)this->TRACER_THRESHOLD);
}

void Patterns::initPattern(int pattern)
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
  else if (pattern == DOUBLE_TRACER) {
    this->doubleTracerLevel();
  }
  else if (pattern == SETTING) {
    this->settingLevel();
  }
}

void Patterns::adjustMidway(int active)
{
  this->midway = this->active_leds / 2;
  Serial.println("New midway: " + (String)this->midway);
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

void Patterns::settingLevel()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < this->active_leds)
      strip.setPixelColor(i, strip.Color(255, 0, 0));
    else
      strip.setPixelColor(i, strip.Color(0, 0, 0));
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
    this->tracerList[0] = 1;
  }
  else
    this->tracerList[0] = 0;

  this->displayTracer();
  this->shiftTracer();
}

void Patterns::doubleTracerLevel()
{
  this->readMSGEQ7();

  // Determine which channel volume is greater
  if (this->left[this->freq] > this->right[this->freq])
    this->audio_input = this->left[this->freq];
  else
    this->audio_input = this->right[this->freq];

  // Fire a shot
  if (this->audio_input > TRACER_THRESHOLD) {
    this->tracerList[this->midway] = 1;
    this->tracerList[this->midway - 1] = 1; 
  }
  else {
    this->tracerList[this->midway] = 0;
    this->tracerList[this->midway - 1] = 0;
  }

  this->displayTracer();
  this->shiftDoubleTracer();
}

void Patterns::shiftDoubleTracer()
{
  // Shift pixels to the right of the midway point
  for (int i = this->active_leds; i >= this->midway; i--) {
    this->tracerList[i + 1] = this->tracerList[i];
    this->tracerList[i] = 0;
  }
 
  
  // Shift pixels to the left of the midway point
  // This for loop cause current_button_state to fail for the
  // up button during double tracer
  for (int i = 0; i <= midway; i++) {
    if (i != 0) { // Need this if statement or it will cause the issue above
      this->tracerList[i - 1] = this->tracerList[i];
      this->tracerList[i] = 0;
    }
  }
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

  this->loadActiveLEDs();
  this->loadTracerThresh();
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
