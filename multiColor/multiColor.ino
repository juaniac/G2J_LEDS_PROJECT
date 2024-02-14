#include <FastLED.h>
#include "FastLEDSExs.h"

#define LED_PIN     2
#define NUM_LEDS    81 
#define BRIGHTNESS  200 
#define LED_TYPE    SK6812
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 40

typedef enum {RED, GREEN, BLUE, PRIDE, PACIFICA, RAINBOW, CONFETTI, SINELON, BPM, JUGGLE, FIRE} Anim_states;
#define NUMBER_OF_STATES 11
Anim_states cur_state = RED;
long previousUpdateTime;

void setup() {
    delay( 1000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);

    previousUpdateTime = millis();
}

void loop(){
  Anim_states next_state = cur_state;   
  if(4000 + previousUpdateTime < millis()){
    if(next_state == RED){
      next_state = GREEN;
    }else if(next_state == GREEN){
      next_state = BLUE;
    }else {
      uint8_t rand = random8(3, NUMBER_OF_STATES + 5);
      next_state = ((rand < NUMBER_OF_STATES) ? rand : FIRE);
    }
    previousUpdateTime = millis();
  }
  if(cur_state == RED){
    fill_solid(leds, NUM_LEDS, 0xFF0000);
  }else if(cur_state == GREEN){
    fill_solid(leds, NUM_LEDS, 0x00FF00);
  }else if(cur_state == BLUE){
    fill_solid(leds, NUM_LEDS, 0x0000FF);
  }else if(cur_state == PRIDE){
    pride(leds, 0, NUM_LEDS);
  }else if(cur_state == PACIFICA){
    pacifica(leds, 0, NUM_LEDS);
  }else if(cur_state == RAINBOW){
    rainbow(leds, 0, NUM_LEDS);
  }else if(cur_state == CONFETTI){
    confetti(leds, 0, NUM_LEDS);
  }else if(cur_state == SINELON){
    sinelon(leds, 0, NUM_LEDS);
  }else if(cur_state == JUGGLE){
    juggle(leds, 0, NUM_LEDS);
  }else if(cur_state == FIRE){
    Fire2012(leds, 0, 29, 55, 140, false);
    Fire2012(leds, 30, 56, 55, 140, true);
    Fire2012(leds,57, 66, 10, 120, false);
    Fire2012(leds, 67, 81, 10, 120, true);
  }

  cur_state = next_state; 

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}