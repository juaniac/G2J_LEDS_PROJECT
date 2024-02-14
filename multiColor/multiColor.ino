#include <FastLED.h>
#include "FastLEDSExs.h"

#define LED_PIN     2
#define NUM_LEDS    175 //202  //202 249 jules
#define BRIGHTNESS  200  //64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 240

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
}

void loop(){   
  pacifica(leds, 0, 88);
  Fire2012(leds, 88, NUM_LEDS, 55, 120, false);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}