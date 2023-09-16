#include <FastLED.h>

#define LED_PIN     2
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define nbLeds    202

#define BRIGHTNESS  100
#define FRAMES_PER_SECOND 60

CRGB leds[nbLeds];

#define nbSegments 10
struct Segment{
  uint16_t start;
  uint16_t end;
  int dir;
};
struct SegmentHeights{
  uint8_t lowHeight;
  uint8_t highHeight;
};
const Segment segments[]  = {{0, 44, 1}, {44, 66, -1}, {66, 86, 1}, {86, 91, -1},{91, 101, 1},{102, 111, -1}, {111, 116, 1},{116, 136, -1},{136, 157, 1},{158, 201, -1}};
const SegmentHeights sgh[] = {{0, 56}, {27, 56}, {27, 54},{48, 54},{48, 63}, {48, 63} ,{48, 54}, {27, 54}, {27, 56}, {0, 56}};
uint8_t displacements[nbSegments]; 

struct HSV{
  uint8_t hue;
  uint8_t sat;
};
HSV ledsHSV[nbLeds];

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, nbLeds).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(9600);
}

void loop(){
  update();
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

/**
uint8_t mask8bitsGet(uint16_t i){
  return (mask8bits[i/2] >> 4*(1 - i%2)) % 16;
}

void mask8bitsSet(uint16_t i, uint8_t val){
  mask8bits[i/2] = (mask8bits[i/2] & ((0b00001111) << 4*(i%2))) | (val << 4*(1 - i%2));
}
*/

#define scaleUpHeights 5
uint16_t waterHeight = 0;

void update(){ 
  if(waterHeight > 63*scaleUpHeights +1){return;}

  for(size_t i = 0; i < nbLeds; i++){
    ledsHSV[i] = {150, 255};
  }

  for (size_t i = 0; i < nbSegments; i++){
    int dir = segments[i].dir;
    uint16_t start = segments[i].start;
    uint16_t end = segments[i].end;

    uint16_t lowHeight = scaleUpHeights*sgh[i].lowHeight;
    uint16_t highHeight = scaleUpHeights*sgh[i].highHeight;
  
    int rand = newRandom(i);
    int curWaterHeight = max(0, waterHeight + rand);

    if(lowHeight <= curWaterHeight && curWaterHeight < highHeight){

      uint16_t newPos = map(curWaterHeight, lowHeight, highHeight, dir == 1 ? start : end, dir == 1 ? end : start);

      uint8_t bri = dir == 1 ? 255 : 0;
      for(uint16_t j = start; j < newPos; j++){
        leds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }

      uint16_t total = mapScale(curWaterHeight, lowHeight, highHeight, start, end, 10);
      
      leds[newPos].setHSV(ledsHSV[newPos].hue, ledsHSV[newPos].sat, ((int)(total) % 10)*255/10);

      bri = dir == 1 ? 0 : 255;
      for(uint16_t j = newPos + 1; j <= end; j++){
        leds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
    }else{
      uint8_t bri = curWaterHeight < lowHeight ? 0 : 255;
      for(uint16_t j = start; j <= end; j++){
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
  waterHeight += 1;
}

int newRandom(uint16_t index){
  uint8_t curDisBrute = displacements[index];
  uint8_t msb = curDisBrute >> 7;
  uint8_t rest = curDisBrute & 0b01111111;
  int newDis;
  if(rest == 0){
    newDis = random(0, 0*scaleUpHeights + 1);
    displacements[index] = (newDis < 0 ? (1 << 7) : 0) | (newDis & 0b01111111);
  }else{
    rest = rest > 2 ? rest - 2 : 0 ;    
    newDis = (msb == 1 ? -1 : 1)*rest;
    displacements[index] = (msb << 7) | rest;
  }
  if(index ==0) Serial.println(newDis);
  return newDis;
}

long mapScale(long x, long in_min, long in_max, long out_min, long out_max, long scale) {
  return (x - in_min) * (out_max - out_min) * scale / (in_max - in_min) + out_min * scale;
}



/**
uint8_t sinCounter = 0;
bool sinUp = true;
#define nbValues 7
int sinWaveVals[] = {-2,-1,-1,0,1,1,2};
int sudoSinWave(){
  int ret = sinWaveVals[sinCounter];
  if(sinUp){
    if(sinCounter < (nbValues - 1)){
      sinCounter += 1;
    }else{
      sinCounter -= 1;
      sinUp = false;
    }
  }else{
    if(sinCounter > 0){
      sinCounter -= 1;
    }else{
    sinCounter += 1;
    sinUp = true;
    }
  }
  return ret;
}
*/