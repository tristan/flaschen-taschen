#include "FastLED.h"

#define NUM_STRIPS 8
#define NUM_CRATES_TALL 9

#define CRATE_WIDTH 6
#define CRATE_HEIGHT 4

#define NUM_LEDS_PER_STRIP 216

#define TOTAL_PIXELS 1728
// CRATE_WIDTH * NUM_STRIPS
#define TOTAL_WIDTH 48
#define TOTAL_HEIGHT 36


int pixelIndex = 0;
const int stateR = 0;
const int stateG = 1;
const int stateB = 2;
int state = stateR;

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

uint16_t xy2arr(uint8_t x, uint8_t y) {

  // find strip
  uint16_t strip = x / CRATE_WIDTH;

  // find the x in the specific strip
  uint16_t s_x = x % CRATE_WIDTH;

  // find the y pos
  uint16_t i;
  if (y & 0x01) {
    // Odd rows are in reverse
    uint16_t reverseX = (CRATE_WIDTH - 1) - s_x;
    i = (y * CRATE_WIDTH) + reverseX;
  } else {
    // Even rows run forwards
    i = (y * CRATE_WIDTH) + s_x;
  }

  return NUM_LEDS_PER_STRIP * strip + i;
}

void setup() {
  Serial.begin(9600);
  LEDS.addLeds<WS2811_PORTD,NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
}

void loop() {

  if (Serial.available()) {
    byte incomingByte = Serial.read();  // will not be -1
    switch (incomingByte) {
    case 1: // packet state indicator used by GLEDIATOR
      FastLED.show();
      pixelIndex = 0; // restart with new data
      state = stateR;
      break;
    default: // all values !=1 are payload data
      int x = pixelIndex % TOTAL_WIDTH;
      int y = pixelIndex / TOTAL_WIDTH;

      uint16_t pos = xy2arr(x, y);
      switch (state) {
      case stateR:
        leds[pos].r = incomingByte;
        state = stateG;
        break;
      case stateG:
        leds[pos].g = incomingByte;
        state = stateB;
        break;
      case stateB:
        leds[pos].b = incomingByte;
        state = stateR;
        pixelIndex++;
        // if we've hit the last pixel, show the current data
        //if (pixelIndex >= TOTAL_PIXELS - 1) {
        //  FastLED.show();
        //}
        break;
      default:
        state = 0;
      } // switch state
    } // switch incomingByte
  } // if available
}