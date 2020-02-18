#ifndef ButtonInput_h
#define ButtonInput_h

#define HOLD_THRESHOLD 124
#define CYCLE_BUTTON 2
#define DOWN_BUTTON  7
#define UP_BUTTON    3

class ButtonInput
{
  private:
    int BUTTON;
    int hold_count = 0;
    bool current_button_state = false;
  public:
    ButtonInput(int button_pin);

    void adjustTracerThresh(int adjustment);
    void changePattern();
    void checkPatternChange();
    void initButtonInput(int BUTTON);
};

#endif