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

    //Serial.println("Saved remainder " + (String)modu + " to addr " + (String)addr);
    //Serial.println("Saved multiplier " + (String)whole_mult + " to addr " + (String)MOD_ADDR);
  }
  else {
    EEPROM.update(addr, value);
    //Serial.println("Saved value " + (String)value + " to addr " + (String)addr);
  }
}

void Patterns::loadActiveLEDs()
{
  //Serial.println("Loading active LEDs");
  this->active_leds = EEPROM.read(ACTIVE_LED_ADDR);
  if ((this->active_leds < 0) || (this->active_leds > NUM_LEDS)) {
    this->active_leds = NUM_LEDS;
  //  Serial.println("Loaded active_leds from default: " + (String)this->active_leds);
    this->saveEEPROM(ACTIVE_LED_ADDR, this->active_leds, true);
  }
  //else
  //  Serial.println("Loaded active_leds from EEPROM: " + (String)this->active_leds);
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
    //Serial.println("Loaded TRACER_THRESHOLD from default: " + (String)this->TRACER_THRESHOLD);
    this->saveEEPROM(TRACER_THRESH_ADDR, this->TRACER_THRESHOLD);
  }
  //else
    //Serial.println("Loaded TRACER_THRESHOLD from EEPROM: " + (String)this->TRACER_THRESHOLD);
}

void Patterns::initPattern(int pattern, uint32_t currentTime)
{
  if (!this->checkSettingTimer(currentTime)) {
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
    else if (pattern == PULSE) {
      this->pulsePattern();
    }
    else if (pattern == FREQ_PULSE) {
      this->freqPulsePattern();
    }
    else if (pattern == FREQ_PULSE_TAI) {
      this->freqPulsePattern();
    }
    else if (pattern == SETTING) {
      this->settingLevel();
    }
  }
  else {
    if ((pattern == TRACER) ||
        (pattern == DOUBLE_TRACER) ||
        (pattern == FREQ_PULSE) ||
        (pattern == FREQ_PULSE_TAI) ||
        (pattern == PULSE))
      this->showTracerThresh();
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

void Patterns::showTracerThresh()
{
  long product = (long)TRACER_THRESHOLD * (long)this->active_leds;
  int thresh_react = abs(product / 1023L);
  //Serial.println("(" + (String)product + " * " + (String)this->active_leds + ") / " + (String)thresh_react + " = " + (String)thresh_react + "/" + (String)this->active_leds);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < thresh_react)
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    else
      strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

bool Patterns::checkSettingTimer(uint32_t currentTime) {

  // has not been X seconds yet
  if (currentTime - this->initTime < SETTING_TIMER)
    return true;
  else {
    return false;
  }
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

void Patterns::pulsePattern()
{
  this->readMSGEQ7();

  if (this->left[this->freq] > this->right[this->freq])
    this->audio_input = this->left[this->freq];
  else
    this->audio_input = this->right[this->freq];

  if (led_level > 0.0)
    led_level = led_level / 1.05;
    //led_level = led_level - 0.01;
  else
    led_level = 0.0;

  if (led_level < 0.0)
    led_level = 0.0;

  if (audio_input > TRACER_THRESHOLD)
  {
    led_level = (float)audio_input / 1000.0;
    if (led_level > 1.00)
      led_level = 1.00;
    Serial.println("audio_input: " + (String)audio_input + " -> led_level: " + (String)led_level);
  }

  for (int i = 0; i < active_leds; i++)
      strip.setPixelColor(i, strip.Color(255 * led_level, 0 * led_level, 0 * led_level));

  //Serial.println("led_level: " + (String)led_level + " -> " + (String)(255 * led_level) + " " + (String)(0 * led_level) + " " + (String)(0 * led_level));
  strip.show();
}

void Patterns::freqPulsePattern()
{

  int skill_points = 255;
  
  this->readMSGEQ7();

  // Calculate zoning
  int zoning = active_leds / 7;
  int half_zone = zoning / 2;

  // Iterate through all freqs
  for (int i = 0; i < 7; i++) {
    
    // Select left or right chanel
    if (this->left[i] > this->right[i])
      this->audio_input = this->left[i];
    else
      this->audio_input = this->right[i];

    // Fade LEDs over time
    if (freq_led_level[i] > 0.0)
      //freq_led_level[i] = freq_led_level[i] / 1.1;
      freq_led_level[i] = freq_led_level[i] - 0.015;
    else
      freq_led_level[i] = 0.00;

    // Bottom out the level if below zero
    if (freq_led_level[i] < 0.0)
      freq_led_level[i] = 0.00;

    // Generate new random color
    if (freq_led_level[i] <= 0.05) {
      int r = random(0, skill_points);
      int g = random(0, skill_points - r);
      int b = random(0, skill_points - r - g);


      /*
      this->freq_colors[i][0] = random(0, 255);
      this->freq_colors[i][1] = random(0, 255);
      this->freq_colors[i][2] = random(0, 255);
      */
      this->freq_colors[i][0] = r;
      this->freq_colors[i][1] = g;
      this->freq_colors[i][2] = b;

      //Serial.println("New Color: (" + (String)r + ", " + (String)g + ", " + (String)b + ")");
      
    }

    if (audio_input > TRACER_THRESHOLD)
    {
      freq_led_level[i] = (float)audio_input / 1000.0;
      if (freq_led_level[i] > 1.00)
        freq_led_level[i] = 1.00;
      //Serial.println("audio_input: " + (String)audio_input + " -> led_level: " + (String)freq_led_level[i]);
    }

    float modi = 0;

    for (int x = 0; x < zoning; x++) {
      if (x <= half_zone)
        modi = (float)x;
      else
        modi = (float)zoning - (float)x;

      float level_mod = modi / (float)half_zone;
      float inverse_mod = 1.00 - level_mod;
      //strip.setPixelColor(x + (zoning * i), strip.Color(freq_colors[i][0] * freq_led_level[i], freq_colors[i][1] * freq_led_level[i], freq_colors[i][2] * freq_led_level[i]));

      if (this->currentPattern == FREQ_PULSE)
        strip.setPixelColor(x + (zoning * i), strip.Color(level_mod * (freq_colors[i][0] * freq_led_level[i]), 
                                                          level_mod * (freq_colors[i][1] * freq_led_level[i]),
                                                          level_mod * (freq_colors[i][2] * freq_led_level[i])));
      
      if (this->currentPattern == FREQ_PULSE_TAI)
        strip.setPixelColor(x + (zoning * i), strip.Color((level_mod * freq_colors[i][0] + (80 * inverse_mod)) * freq_led_level[i], 
                                                          (level_mod * freq_colors[i][1] + (5 * inverse_mod)) * freq_led_level[i],
                                                          (level_mod * freq_colors[i][2] + (1 * inverse_mod)) * freq_led_level[i]));

    }
  }

  /*
  Serial.println((String)freq_led_level[0] + " " + 
                 (String)freq_led_level[1] + " " + 
                 (String)freq_led_level[2] + " " + 
                 (String)freq_led_level[3] + " " + 
                 (String)freq_led_level[4] + " " + 
                 (String)freq_led_level[5] + " " + 
                 (String)freq_led_level[6] + " ");
  */

  strip.show();
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
  //this->loadTracerThresh();
  //Serial.println("Done doing a setup");
}

void Patterns::setupStrip()
{
  for (int i = 0; i < this->active_leds; i++)
    this->tracerList[i] = 0;

  for (int i = 0; i < this->active_leds + 1; i++)
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  strip.show();

  strip = Adafruit_NeoPixel(this->active_leds, LED_PIN, NEO_GRB + NEO_KHZ800);
    
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  for (int i = 0; i < this->active_leds; i++)
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  strip.show();
}

// Slightly different, this makes the rainbow equally distributed throughout
void Patterns::rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
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
