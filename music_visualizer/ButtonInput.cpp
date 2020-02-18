#include "ButtonInput.h"
#include "Patterns.h"

extern Patterns patternObj;

ButtonInput::ButtonInput(int button_pin)
{
  this->initButtonInput(button_pin);
}

void ButtonInput::adjustTracerThresh(int adjustment)
{
  patternObj.TRACER_THRESHOLD = patternObj.TRACER_THRESHOLD + adjustment;
  if (patternObj.TRACER_THRESHOLD <= 0)
    patternObj.TRACER_THRESHOLD = 0;
  else if (patternObj.TRACER_THRESHOLD >= 1023)
    patternObj.TRACER_THRESHOLD = 1023;

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

void ButtonInput::checkPatternChange()
{
  // Check if button is actuated
  if (digitalRead(BUTTON) == 1)
  {
    // Check if button states has changed
    if (!this->current_button_state)
    {
      Serial.println("Button pressed");
      this->current_button_state = true;

      // Check which button was pressed
      if (BUTTON == CYCLE_BUTTON) {
        this->changePattern();
      }
      else if (BUTTON == UP_BUTTON) {
        // Check which pattern is active
        if (patternObj.currentPattern == TRACER)
          this->adjustTracerThresh(1);
      }
      else if (BUTTON == DOWN_BUTTON) {
        // Check which pattern is active
        if (patternObj.currentPattern == TRACER)
          this->adjustTracerThresh(-1);
      }
    }

    // Button hold check
    if ((BUTTON == UP_BUTTON) || (BUTTON == DOWN_BUTTON)) {

      // Don't increase if we are already at the threshold
      if (this->hold_count <= HOLD_THRESHOLD) {
        this->hold_count++;
        //Serial.println(hold_count);
      }

      // Threshold is met here
      if (this->hold_count >= HOLD_THRESHOLD) {
        // Button down and up shit
        if (BUTTON == UP_BUTTON) {
          // Check which pattern is active
          if (patternObj.currentPattern == TRACER)
            this->adjustTracerThresh(1);
        }
        else if (BUTTON == DOWN_BUTTON) {
          // Check which pattern is active
          if (patternObj.currentPattern == TRACER)
            this->adjustTracerThresh(-1);
        }
      }
    }
  }
  else {
    this->current_button_state = false;
    this->hold_count = 0;
  }
}

void ButtonInput::initButtonInput(int BUTTON)
{
  this->BUTTON = BUTTON;
  pinMode(this->BUTTON, INPUT);
}
