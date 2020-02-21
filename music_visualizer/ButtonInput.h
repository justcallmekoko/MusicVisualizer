#ifndef ButtonInput_h
#define ButtonInput_h

#include <String.h>
#include <Arduino.h>

#define HOLD_THRESHOLD 100
#define CYCLE_BUTTON 2

#define DOWN_BUTTON  7
#define UP_BUTTON    3

class ButtonInput
{
  private:
    
  public:
    ButtonInput(int button_pin);

    int BUTTON;
    int hold_count = 0;
    boolean current_button_state = false;

    void adjustSetting(int adjustment);
    void adjustTracerThresh(int adjustment);
    void changePattern();
    void checkPatternChange();
    void debugButtonState(int button, bool button_state, String message);
    void setButtonState(bool state);
    void initButtonInput(int tBUTTON);
};

#endif
