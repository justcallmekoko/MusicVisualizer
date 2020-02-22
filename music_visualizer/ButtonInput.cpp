#include "ButtonInput.h"
#include "Patterns.h"

extern Patterns patternObj;

ButtonInput::ButtonInput(int button_pin)
{
  this->initButtonInput(button_pin);
}

void ButtonInput::adjustSetting(int adjustment)
{
  patternObj.active_leds = patternObj.active_leds + adjustment;
  if (patternObj.active_leds >= NUM_LEDS)
    patternObj.active_leds = NUM_LEDS;
  else if (patternObj.active_leds <= 0)
    patternObj.active_leds = 0;

  Serial.println("Active LEDs: " + (String)patternObj.active_leds);

  patternObj.saveEEPROM(ACTIVE_LED_ADDR, patternObj.active_leds);
  patternObj.adjustMidway(patternObj.active_leds);
}

void ButtonInput::adjustTracerThresh(int adjustment)
{
  patternObj.TRACER_THRESHOLD = patternObj.TRACER_THRESHOLD + adjustment;
  if (patternObj.TRACER_THRESHOLD <= 0)
    patternObj.TRACER_THRESHOLD = 0;
  else if (patternObj.TRACER_THRESHOLD >= 1023)
    patternObj.TRACER_THRESHOLD = 1023;

  patternObj.saveEEPROM(TRACER_THRESH_ADDR, patternObj.TRACER_THRESHOLD, true);

  Serial.println("Tracer Threshold: " + (String)patternObj.TRACER_THRESHOLD);
}

void ButtonInput::changePattern()
{
  patternObj.currentPattern++;
  if (patternObj.currentPattern >= NUM_PATTERN)
    patternObj.currentPattern = 0;
  Serial.println("Pattern select: " + (String)patternObj.currentPattern);
  patternObj.resetWheel();
}

void ButtonInput::debugButtonState(int button, bool button_state, String message)
{
  if (button_state)
    Serial.println("Button " + (String)button + " state is true: " + (String)message);
  else
    Serial.println("Button " + (String)button + " state is false: " + (String)message);
}

void ButtonInput::setButtonState(bool state)
{
  //Serial.println("Setting button " + (String)this->BUTTON + " " + (String)state);
  this->current_button_state = state;
}

void ButtonInput::checkPatternChange()
{
  // Check if button is actuated
  if ((digitalRead(this->BUTTON) == 1) && (digitalRead(this->BUTTON) == 1))
  {
    //this->debugButtonState(this->BUTTON, this->current_button_state, "Pre check");
      
    // Check if button states has changed
    if (!this->current_button_state)
    {
      Serial.println("Button pressed");
      this->setButtonState(true);

      // Check which button was pressed
      if (this->BUTTON == CYCLE_BUTTON) {
        this->changePattern();
      }
      else if (this->BUTTON == UP_BUTTON) {
        // Tracer patterns
        if ((patternObj.currentPattern == TRACER) || 
            (patternObj.currentPattern == DOUBLE_TRACER))
          this->adjustTracerThresh(1);

        // Setting pattern
        else if (patternObj.currentPattern == SETTING)
          this->adjustSetting(1);
      }
      else if (this->BUTTON == DOWN_BUTTON) {
        // Tracer patterns
        if ((patternObj.currentPattern == TRACER) || 
            (patternObj.currentPattern == DOUBLE_TRACER))
          this->adjustTracerThresh(-1);

        // Setting pattern
        else if (patternObj.currentPattern == SETTING)
          this->adjustSetting(-1);
      }
    }

    //this->debugButtonState(this->BUTTON, this->current_button_state, "Post check");
    
    this->setButtonState(true);

    //this->debugButtonState(this->BUTTON, this->current_button_state, "Pre hold check");

    // Button hold check for tracer
    if ((this->BUTTON == UP_BUTTON) || (this->BUTTON == DOWN_BUTTON)) {

      // Don't increase if we are already at the threshold
      if (this->hold_count <= HOLD_THRESHOLD) {
        this->hold_count++;
        //Serial.println(this->hold_count);
      }

      // Threshold is met here
      if (this->hold_count >= HOLD_THRESHOLD) {
        // Button down and up shit
        if (this->BUTTON == UP_BUTTON) {
          // Check which pattern is active
          if ((patternObj.currentPattern == TRACER) || 
              (patternObj.currentPattern == DOUBLE_TRACER))
            this->adjustTracerThresh(1);
        }
        else if (this->BUTTON == DOWN_BUTTON) {
          // Check which pattern is active
          if ((patternObj.currentPattern == TRACER) || 
              (patternObj.currentPattern == DOUBLE_TRACER))
            this->adjustTracerThresh(-1);
        }
      }
    }
    //this->debugButtonState(this->BUTTON, this->current_button_state, "Post hold check\n");
  }
  else {
    //Serial.println("Setting button " + (String)this->BUTTON + " false");
    this->setButtonState(false);
    this->hold_count = 0;
  }

  if (digitalRead(this->BUTTON) == 1)
    this->setButtonState(true);
}

void ButtonInput::initButtonInput(int tBUTTON)
{
  this->BUTTON = tBUTTON;
  pinMode(this->BUTTON, INPUT);
}
