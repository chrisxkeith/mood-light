#include <Wire.h>
#include <vector>
#include <set>

class Timer {
  private:
    String        msg;
    unsigned long start;
  public:
    Timer(String msg) {
      this->msg = msg;
      start = millis();
    }
    ~Timer() {
      unsigned long ms = millis() - start;
      String msg(ms);
      msg.concat(" milliseconds for ");
      msg.concat(this->msg);
      Serial.println(msg);
    }
};

#include <FastLED.h>
const int     LEDS_WIDTH = 16;
const int     LEDS_HEIGHT = 16;
const int     NUM_LEDS = LEDS_WIDTH * LEDS_HEIGHT;
CRGB          leds[NUM_LEDS] = {0};     // Software gamma mode.

#define HALF_WHITE 0x808080
#define QUARTER_WHITE 0x404040
#define EIGHTH_WHITE 0x202020
#define WHITENESS EIGHTH_WHITE
class LEDStripWrapper {
  private:
    static int pixelToLedIndex[NUM_LEDS];
  public:
    static const int  DATA_PIN = 8;   // green wire
    static const int  CLOCK_PIN = 10; // blue wire
    static uint32_t   theDelay;
    static void speedTest() {
      Timer timer("speedTest()");
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = WHITENESS;
        FastLED.show();
        // delay(theDelay);
        leds[i] = CRGB::Black;
        FastLED.show();
      }
    }
    static void rampTestForward() {
      Timer timer("rampTestForward()");
      // Draw a linear ramp of brightnesses to showcase the difference between
      // the HD and non-HD mode.
      const int NUM_TO_TEST = NUM_LEDS;
      for (int i = 0; i < NUM_TO_TEST; i++) {
          int brightness = map(i, 0, NUM_TO_TEST - 1, 0, 255);
          CRGB c(brightness, brightness, brightness);  // Just make a shade of white.
          leds[i] = c;
      }
      FastLED.show();  // All LEDs are now displayed.
      delay(5000);
      clear();
    }
    static void rampTestBackward() {
      Timer timer("rampTestBackward()");
      // Draw a linear ramp of brightnesses to showcase the difference between
      // the HD and non-HD mode.
      const int NUM_TO_TEST = NUM_LEDS;
      for (int i = NUM_TO_TEST - 1; i > -1; i--) {
          int brightness = map(i, 0, NUM_TO_TEST - 1, 0, 255);
          CRGB c(brightness, brightness, brightness);  // Just make a shade of white.
          leds[i] = c;
      }
      FastLED.show();  // All LEDs are now displayed.
      delay(5000);
      clear();
    }
    static void clear() {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
    }
    static void startup() {
      clear();
      speedTest();
    }
};
uint32_t LEDStripWrapper::theDelay = 0;
int LEDStripWrapper::pixelToLedIndex[NUM_LEDS] = {
   15,  14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0,
   16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
   47,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32,
   48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
   79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,  64,
   80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
  111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
  143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128,
  144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
  175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192,
  208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
  239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224,
  240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

class App {
  private:
    String configs[2] = {
      "~2025Dec26:09:42", // date +"%Y%b%d:%H:%M"
      "https://github.com/chrisxkeith/mood-light.git",
    };

    void checkSerial() {
      if (Serial.available() > 0) {
        String teststr = Serial.readString();  // read until timeout
        teststr.trim();                        // remove any \r \n whitespace at the end of the String
        if (teststr.equals("runSpeedTest")) {
          LEDStripWrapper::speedTest();
        } else if (teststr.equals("runRampTest")) {
          LEDStripWrapper::rampTestForward();
          LEDStripWrapper::rampTestBackward();
        } else {
          String msg("Unknown command: '");
          msg.concat(teststr);
          msg.concat("'. Expected one of ");
          msg.concat("  runSpeedTest or runRampTest.");
          Serial.println(msg);
        }
      }
    }
  public:
    void setup() {
      Timer timer("setup()");
      delay(500); // power-up safety delay
      Serial.begin(115200);
      Serial.println("setup() started.");
      Wire.begin();
      FastLED.addLeds<APA102, LEDStripWrapper::DATA_PIN, LEDStripWrapper::CLOCK_PIN, BGR>(leds, NUM_LEDS);
      LEDStripWrapper::startup();
    }
    void loop() {
      checkSerial();
    }
};
App app;

void setup() {
  app.setup();
}

void loop() {
  app.loop();
}
