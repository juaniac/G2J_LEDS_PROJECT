#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    202  //202 249 jules
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    //FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
    
}


void loop(){   
  //estimatePoints();
  int points[] = {0, 44, 45, 66, 86, 91, 101, 102, 111, 116, 137, 157, 158, 201};
  testSomePoints(sizeof(points), points);
  //pathTest();
  //firstLightTest();
  //lastLightTest();
  //testBrightness();
  //pride(leds, 0, 202);

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


