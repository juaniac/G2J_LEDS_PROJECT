#include <FastLED.h>
#include "FastLEDSExs.h"

#define LED_PIN     2
#define NUM_LEDS    202  //202 249 jules
#define BRIGHTNESS  200  //64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 240

uint16_t temp;

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
    
}

void loop(){   
  pacifica(leds, 0, 202);
  leds[0].setRGB(0,0,0);
  leds[1].setRGB(0,0,0);
  leds[28].setRGB(0,0,0);
  leds[29].setRGB(0,0,0);
  leds[30].setRGB(0,0,0);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void lastLightTest(){
  leds[NUM_LEDS - 1].setRGB(255,255,255);
}

void firstLightTest(){
  leds[0].setRGB(255,255,255);
}

void pathTest(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i].setRGB(255 - (i*255)/(NUM_LEDS - 1),0,0);  
  }
}

void estimatePoints(){
  for(int i = 0; i < NUM_LEDS; i+=10){
    leds[i].setRGB(255,255,255);  
  }
}

void testSomePoints(int nbPoints, int* pointList){
  for(int i = 0; i < nbPoints; i++){
    leds[pointList[i]].setRGB(255,0,0);
  }
}

void clearLeds(){
  for(int i = 0; i < NUM_LEDS; i+=10){
    leds[i].setRGB(0,0,0);  
  }  
}

void testBrightness(){
  for(int i = 0; i < 255; i +=10){
    leds[i/10].setRGB(0,0,i);
  }
}