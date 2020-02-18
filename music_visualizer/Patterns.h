#ifndef Patterns_h
#define Patterns_h

#include <Adafruit_NeoPixel.h>
//#include <SimpleList.h>

#define DOUBLE_THRESHOLD 80
//#define TRACER_THRESHOLD 300
#define NUM_LEDS 150
#define LED_PIN 6
#define BRIGHTNESS 50
#define strobe 4
#define reset 5
#define audio1 A0
#define audio2 A1

#define NUM_PATTERN 3

// Patterns
#define SINGLE 0
#define DOUBLE 1
#define TRACER 2

extern Adafruit_NeoPixel strip;

class Patterns
{
  private:
    //SimpleList<int> *tracerList = new SimpleList<int>();

    int tracerList[NUM_LEDS] = {};
  
    int left[7];
    int right[7];
    int band;
    int audio_input = 0;
    int freq = 0;

    int active_leds = NUM_LEDS;
    int midway = active_leds / 2;
    int loop_max = 0;
    int k = 255;
    int decay = 0;
    int decay_check = 0;
    long pre_react = 0;
    long react = 0;
    long react_downer = 1;
    long post_react = 0;

    int wheel_speed = 1; // Low is slower
    

  public:
    Patterns();

    byte currentPattern PROGMEM = 0;

    int TRACER_THRESHOLD = 300;

    // Patterns
    void singleLevel();
    void doubleLevel();
    void tracerLevel();

    // Aux methods
    void initPattern(uint8_t pattern);
    void convertSingle();
    void convertDouble();
    void displayTracer();
    void singleRainbow();
    void doubleRainbow();
    void readMSGEQ7();
    void resetWheel();
    void setupAudio();
    void setupStrip();
    void shiftTracer();
    uint32_t Wheel(byte WheelPos);
};

#endif
