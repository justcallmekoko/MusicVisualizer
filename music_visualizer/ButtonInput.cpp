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
  if (patternObj.currentPattern >= NUM_PATTERN + 10)
    patternObj.currentPattern = 10;
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
      this->current_button_state = true;

      // Check which button was pressed
      if (this->BUTTON == CYCLE_BUTTON) {
        this->changePattern();
      }
      else if (this->BUTTON == UP_BUTTON) {
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

    //this->debugButtonState(this->BUTTON, this->current_button_state, "Post check");
    
    this->current_button_state = true;

    //this->debugButtonState(this->BUTTON, this->current_button_state, "Pre hold check");

    // Button hold check
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
    this->current_button_state = false;
    this->hold_count = 0;
  }

  if (digitalRead(this->BUTTON) == 1)
    this->current_button_state = true;
}

void ButtonInput::initButtonInput(int _BUTTON)
{
  this->BUTTON = _BUTTON;
  pinMode(this->BUTTON, INPUT);
}
