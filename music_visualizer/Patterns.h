#ifndef Patterns_h
#define Patterns_h

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
//#include <SimpleList.h>

#define DOUBLE_THRESHOLD 80
//#define TRACER_THRESHOLD 300
#define NUM_LEDS 255
#define LED_PIN 6
#define BRIGHTNESS 100
#define strobe 4
#define reset 5
#define audio1 A0
#define audio2 A1
#define SETTING_TIMER 500

#define NUM_PATTERN 8

// Patterns
#define SINGLE 0
#define DOUBLE 1
#define TRACER 2
#define DOUBLE_TRACER 3
#define PULSE 4
#define FREQ_PULSE 5
#define FREQ_PULSE_TAI 6
#define SETTING 7

// EEPROM Addrs
#define TRACER_THRESH_ADDR 0
#define ACTIVE_LED_ADDR    (sizeof(long) * 1)
#define MOD_ADDR           (sizeof(long) * 2)

#define DEFAULT_TRACER_THRESH 550

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
    
    int loop_max = 0;
    int k = 255;
    int decay = 0;
    int decay_check = 0;
    long pre_react = 0;
    long react = 0;
    long react_downer = 1;
    long post_react = 0;

    int wheel_speed = 1; // Low is slower

    float led_level = 0.0;

    float freq_led_level[7] = {0.0,
                               0.0,
                               0.0,
                               0.0,
                               0.0,
                               0.0,
                               0.0};

    int freq_colors[7][3] = {{255, 0, 0},
                               {255, 128, 0},
                               {255, 255, 0},
                               {0, 255, 0},
                               {0, 255, 128},
                               {0, 255, 255},
                               {0, 0, 255}};
    

  public:
    Patterns();

    int currentPattern PROGMEM = DOUBLE_TRACER;

    int active_leds = NUM_LEDS;
    int midway = active_leds / 2;

    int TRACER_THRESHOLD = 550;

    uint32_t initTime = 0;

    // Patterns
    void settingLevel();
    void singleLevel();
    void doubleLevel();
    void tracerLevel();
    void doubleTracerLevel();

    // Aux methods
    void adjustMidway(int active);
    void initPattern(int pattern, uint32_t currentTime);
    void convertSingle();
    void convertDouble();
    void displayTracer();
    void pulsePattern();
    void freqPulsePattern();
    void singleRainbow();
    void doubleRainbow();
    void readMSGEQ7();
    void resetWheel();
    void setupAudio();
    void setupStrip();
    void shiftDoubleTracer();
    void shiftTracer();
    void loadActiveLEDs();
    void loadTracerThresh();
    void showTracerThresh();
    void rainbowCycle(uint8_t wait);
    void saveEEPROM(long addr, int value, bool fix = false);
    bool checkSettingTimer(uint32_t currentTime);
    uint32_t Wheel(byte WheelPos);
};

#endif
