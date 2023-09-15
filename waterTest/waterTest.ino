#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    202  //202 249 jules
#define BRIGHTNESS  200  //64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 5

#define nbPathAndDirections 2
struct PAD{
  uint16_t start;
  uint16_t end;
  int dir;
};
PAD pathAndDirections[] = {{0, 44, 1},{158, 201, -1}};

struct PH{
  uint16_t pos;
};

PWL pathWaterLevel[nbPathAndDirections];

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    //FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);

    setupLEDS();
}

void loop(){   
  updateLEDS2();
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void setupLEDS(){
  pathWaterLevel[0].pos = 10;
  pathWaterLevel[1].pos = 201 - 10;
}

void updateLEDS1(){
  for(uint16_t i = 0; i < nbPathAndDirections; i++){
    int dir = pathAndDirections[i].dir;
    uint16_t start = pathAndDirections[i].start;
    uint16_t end = pathAndDirections[i].end;

    int newPosInt = pathWaterLevel[i].pos + random(-2, 3);
    uint16_t newPos = constrain(newPosInt, (int)start, (int)end);
    
    for(uint16_t j = start; j < newPos; j++){
      leds[j].setHSV(150, 255, dir == 1 ? 255 : 0);
    }
    leds[newPos].setHSV(150, 255, 255);
    for(uint16_t j = newPos + 1; j <= end; j++){
      leds[j].setHSV(150, 255, dir == 1 ? 0 : 255);
    }
  }
}

void updateLEDS2(){
  if(random8() > 200){
    pathAndDirections[random8(0, nbPathAndDirections)];
  }

  for(uint16_t i = 0; i < nbPathAndDirections; i++){
    int dir = pathAndDirections[i].dir;
    uint16_t start = pathAndDirections[i].start;
    uint16_t end = pathAndDirections[i].end;

    int newPosInt = pathWaterLevel[i].pos;
    uint16_t newPos = constrain(newPosInt, (int)start, (int)end);
    
    for(uint16_t j = start; j < newPos; j++){
      leds[j].setHSV(150, 255, dir == 1 ? 255 : 0);
    }
    leds[newPos].setHSV(150, 255, 255);
    for(uint16_t j = newPos + 1; j <= end; j++){
      leds[j].setHSV(150, 255, dir == 1 ? 0 : 255);
    }
    
  }
}
