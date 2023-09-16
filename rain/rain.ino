#include <FastLED.h>

#define LED_PIN     2
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define nbLeds    202

#define BRIGHTNESS  100

CRGB leds[nbLeds];

typedef uint8_t CentiMeter;
typedef uint16_t MilliMeter;
typedef uint16_t LedIndex;
typedef uint16_t MilliSecond;
typedef uint16_t MilliMeterPerSecond;

#define CMtoMM(cm) ((cm)*10) 

#define nbSegments 10
struct Segment{
  LedIndex start;
  LedIndex end;
  int dir;
};
struct SegmentHeights{
  CentiMeter lowHeight;
  CentiMeter highHeight;
};
const Segment segments[]  = {{0, 44, 1}, {44, 66, -1}, {66, 86, 1}, {86, 91, -1}, {91, 101, 1},{102, 111, -1}, {111, 116, 1}, {116, 136, -1}, {136, 157, 1}, {158, 201, -1}};
const SegmentHeights sgh[] = {{0, 56}, {27, 56}, {27, 54}, {48, 54}, {48, 63}, {48, 63} ,{48, 54}, {27, 54}, {27, 56}, {0, 56}};

struct HSV{
  uint8_t hue;
  uint8_t sat;
};
HSV ledsHSV[nbLeds];

MilliMeter waterHeight = 0;
MilliSecond previousWaterUpdateTime;
MilliMeterPerSecond waterSpeed = 80;

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, nbLeds).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(9600);

  previousWaterUpdateTime = millis();
  
  for(size_t i = 0; i < nbLeds; i++){
    leds[i].setRGB(0,0,0);
  }
  FastLED.show();
  delay(3000);
}

void loop(){
  update();
  FastLED.show(); // display this frame
}

/**
uint8_t mask8bitsGet(uint16_t i){
  return (mask8bits[i/2] >> 4*(1 - i%2)) % 16;
}

void mask8bitsSet(uint16_t i, uint8_t val){
  mask8bits[i/2] = (mask8bits[i/2] & ((0b00001111) << 4*(i%2))) | (val << 4*(1 - i%2));
}
*/

void update(){ 
  if(waterHeight > CMtoMM(63)){return;}

  for(size_t i = 0; i < nbLeds; i++){
    ledsHSV[i] = {150, 255};
  }

  for (size_t i = 0; i < nbSegments; i++){
    int dir = segments[i].dir;
    LedIndex start = segments[i].start;
    LedIndex end = segments[i].end;

    MilliMeter lowHeight = CMtoMM(sgh[i].lowHeight);
    MilliMeter highHeight = CMtoMM(sgh[i].highHeight);
  
    int rand = beatsin16(90, 0, 40, 0, (i%4)*65535/4) - 20;
    Serial.println(rand);
    MilliMeter curWaterHeight = max(0, int(waterHeight + rand));

    if(lowHeight <= curWaterHeight && curWaterHeight < highHeight){

      LedIndex newPos = map(curWaterHeight, lowHeight, highHeight, dir == 1 ? start : end, dir == 1 ? end : start);

      uint8_t bri = dir == 1 ? 255 : 0;
      for(LedIndex j = start; j < newPos; j++){
        leds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }

      uint16_t afterTheDecimalPoint = mapScale(curWaterHeight, lowHeight, highHeight, start, end, 10) % 10;
      leds[newPos].setHSV(ledsHSV[newPos].hue, ledsHSV[newPos].sat, afterTheDecimalPoint*255/10);

      bri = dir == 1 ? 0 : 255;
      for(LedIndex j = newPos + 1; j <= end; j++){
        leds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
    }else{
      uint8_t bri = curWaterHeight < lowHeight ? 0 : 255;
      for(LedIndex j = start; j <= end; j++){
        leds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
    }
  }
/**
        uint8_t maxFusions = 4 + 2*incWater;
        uint8_t nbOfFusions = constrain((dir*(k - start)), 0, maxFusions);

        for (int l = 0; l < nbOfFusions; l++){
          ledsHSV[k - dir*l].sat = 255*l/maxFusions;
        }
        finished = true;
*/
  updateWaterHeight();
}

long mapScale(long x, long in_min, long in_max, long out_min, long out_max, long scale) {
  return (x - in_min) * (out_max - out_min) * scale / (in_max - in_min) + out_min * scale;
}

void updateWaterHeight(){
  MilliMeter waterAmount = 0;
  while(waterAmount == 0){
    MilliSecond deltaTime = millis() - previousWaterUpdateTime;
    waterAmount = (waterSpeed*deltaTime)/1000;
  }
  previousWaterUpdateTime = millis();
  waterHeight += waterAmount;
}