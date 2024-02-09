#include <FastLED.h>
//
//{"on":true,"bri":255,"transition":7,"mainseg":0,"seg":
//[{"id":0,"start":0,"stop":36,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":"Allu doré","col":[[255,239,230],[0,0,0],[0,0,0]],"fx":0,"sx":128,"ix":128,"pal":0,"c1":128,"c2":128,"c3":16,"sel":true,"rev":false,"mi":false,"o1":false,"o2":false,"o3":false,"si":0,"m12":0},
//{"id":1,"start":151,"stop":195,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":"Allu doré 2","col":[[255,239,230],[0,0,0],[0,0,0]],"fx":0,"sx":128,"ix":128,"pal":0,"c1":128,"c2":128,"c3":16,"sel":true,"rev":false,"mi":false,"o1":false,"o2":false,"o3":false,"si":0,"m12":0},
//{"id":2,"start":195,"stop":233,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":"Jet","col":[[255,160,0],[255,200,0],[0,0,0]],"fx":76,"sx":240,"ix":22,"pal":3,"c1":128,"c2":128,"c3":16,"sel":false,"rev":false,"mi":false,"o1":false,"o2":false,"o3":false,"si":0,"m12":0},
//{"id":3,"start":37,"stop":150,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":"Corps bouteille","col":[[30,255,0],[0,0,0],[0,0,0]],"fx":106,"sx":164,"ix":251,"pal":50,"c1":128,"c2":128,"c3":16,"sel":false,"rev":false,"mi":false,"o1":false,"o2":false,"o3":false,"si":0,"m12":0},
//{"id":4,"start":234,"stop":299,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":"Liquide","col":[[255,160,0],[255,200,0],[0,0,0]],"fx":110,"sx":216,"ix":213,"pal":3,"c1":128,"c2":128,"c3":16,"sel":false,"rev":false,"mi":false,"o1":false,"o2":false,"o3":false,"si":0,"m12":1},
//{"id":5,"start":220,"stop":233,"grp":1,"spc":0,"of":0,"on":true,"frz":false,"bri":255,"cct":127,"set":0,"n":"Gouttes","col":[[255,170,0],[0,0,0],[0,0,0]],"fx":0,"sx":128,"ix":128,"pal":0,"c1":128,"c2":128,"c3":16,"sel":true,"rev":false,"mi":false,"o1":false,"o2":false,"o3":false,"si":0,"m12":0}]}

#define Glass_LED_PIN     2
#define Rest_LED_PIN      1
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS  100

#define nbGlassLeds    66
#define nbRestLeds 234

CRGB glassLeds[nbGlassLeds];
CRGB restLeds[nbRestLeds];

typedef uint8_t CentiMeter;
typedef uint16_t MilliMeter;
typedef uint16_t LedIndex;
typedef uint16_t MilliSecond;
typedef uint16_t MilliMeterPerSecond;

#define CMtoMM(cm) ((cm)*10) 

struct Segment{
  LedIndex start;
  LedIndex end;
  int dir;
};
struct SegmentHeights{
  CentiMeter lowHeight;
  CentiMeter highHeight;
};
#define nbGlassSegments 5
const Segment glassSegment[]  = {{0, 11, -1}, {11, 28, -1}, {28, 36, -1}, {36, 44, 1}, {44, 66, 1}};
const SegmentHeights glassSegmentHeights[] = {{227, 267}, {55, 227}, {0, 55}, {0, 55}, {55, 267}};

const Segment droplet = {221, 234, 1};
const Segment bottle = {37, 151, 1};
const Segment stream = {196, 221, 1};

struct HSV{
  uint8_t hue;
  uint8_t sat;
};
HSV ledsHSV[nbGlassLeds];

MilliMeter waterHeight = 0;
MilliSecond previousWaterUpdateTime;
MilliMeterPerSecond waterSpeed = 60;
#define maxWaveHeight 20

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, Glass_LED_PIN, COLOR_ORDER>(glassLeds, nbGlassLeds).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, Rest_LED_PIN, COLOR_ORDER>(restLeds, nbRestLeds).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(9600);

  for(size_t i = 0; i < nbGlassLeds; i++){
    glassLeds[i].setRGB(0,0,0);
  }
  for(size_t i = 0; i < nbRestLeds; i++){
    restLeds[i].setRGB(0,0,0);
  }

  FastLED.show();
  delay(3000);
  for(size_t i = 0; i < 37; i++){
    restLeds[i].setRGB(255,239,230);
  }for(size_t i = 151; i < 196; i++){
    restLeds[i].setRGB(255,239,230);
  }

  previousWaterUpdateTime = millis();
}

typedef enum {START_WATERING, FILLING_GLASS, STOP_WATERING, GLASS_STAYS_FULL, DRINKING_GLASS, GLASS_STAYS_EMPTY} Anim_states;
#define NUMBER_OF_STATES 6
Anim_states cur_state = START_WATERING;
#define WAIT_TIME_TRANS_MILLIS 4000 

#define lowHue 27
#define highHue 34

void loop(){
  if(cur_state == START_WATERING){
    startStream();
  }else if(cur_state == FILLING_GLASS){
    goingUp();
    randUpToAndBlend(restLeds, stream.start, stream.end, lowHue, highHue + 1);
    dropletAnimation(0);
  }else if(cur_state == STOP_WATERING){
    stopStream();
    randUpToAndBlend(glassLeds, glassSegment[0].start, glassSegment[nbGlassSegments-1].end, lowHue, highHue + 1);
    dropletAnimation(1);
  }else if(cur_state == GLASS_STAYS_FULL){
    randUpToAndBlend(glassLeds, glassSegment[0].start, glassSegment[nbGlassSegments-1].end, lowHue, highHue + 1);
    if(WAIT_TIME_TRANS_MILLIS < millis() - previousWaterUpdateTime){
      cur_state = DRINKING_GLASS;
      previousWaterUpdateTime = millis();
    }
  }else if(cur_state == DRINKING_GLASS){
    goingDown();
  }else{
    if(WAIT_TIME_TRANS_MILLIS < millis() - previousWaterUpdateTime){
      cur_state = START_WATERING;
      previousWaterUpdateTime = millis();
    }
  }
  randUpToAndBlend(restLeds, bottle.start, bottle.end, lowHue, highHue + 1);
  
  FastLED.show(); // display this frame
}

void goingUp(){
  for(size_t i = 0; i < nbGlassLeds; i++){
    ledsHSV[i] = {random8(lowHue, highHue+1), 255};
  }

  for (size_t i = 0; i < nbGlassSegments; i++){
    int dir = glassSegments[i].dir; 
    LedIndex start = glassSegments[i].start;
    LedIndex end = glassSegments[i].end;

    MilliMeter lowHeight = CMtoMM(glassSegmentHeights[i].lowHeight);
    MilliMeter highHeight = CMtoMM(glassSegmentHeights[i].highHeight);
  
    int waveHeight = beatsin16(45, 0, 2*maxWaveHeight, 0, (i%4)*65535/4) - maxWaveHeight; 
    MilliMeter curWaterHeight = max(0, int(waterHeight + waveHeight));

    if(lowHeight <= curWaterHeight && curWaterHeight < highHeight){
      
      LedIndex newPosTimes10 = mapScale(curWaterHeight, lowHeight, highHeight, dir == 1 ? start : end, dir == 1 ? end : start, 10);
      LedIndex newPos = newPosTimes10/10;

      uint8_t bri = dir == 1 ? 255 : 0;
      for(LedIndex j = start; j < newPos; j++){
        glassLeds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
      
      uint8_t afterTheDecimalPoint = dir == 1 ? (newPosTimes10 % 10) : (10 - (newPosTimes10 % 10)) ;
      glassLeds[newPos].setHSV(ledsHSV[newPos].hue, ledsHSV[newPos].sat, afterTheDecimalPoint*255/10);
      
      bri = dir == 1 ? 0 : 255;
      for(LedIndex j = newPos + 1; j <= end; j++){
        glassLeds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
    }else{
      uint8_t bri = curWaterHeight < lowHeight ? 0 : 255;
      for(LedIndex j = start; j <= end; j++){
        glassLeds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
    }
  }
  juansBlend(glassLeds, glassSegment[0].start, glassSegment[nbGlassSegments-1].end);
  waterHeight += getWaterAmount();
  if(CMtoMM(267) < waterHeight + maxWaveHeight){
    cur_state = STOP_WATERING;
  }
}

long mapScale(long x, long in_min, long in_max, long out_min, long out_max, long scale) {
  return (x - in_min) * (out_max - out_min) * scale / (in_max - in_min) + out_min * scale;
}

int curPower = 0;
void startStream(){
  curPower += getWaterAmount();

  if(curPower/10 < (stream.end - stream.start)){
    LedIndex newEnd = curPower/10 + stream.start;
    randUpToAndBlend(restLeds, stream.start, newEnd, lowHue, highHue + 1);
    restLeds[newEnd].setHSV(random8(lowHue, highHue + 1), 255, (curPower%10)*255/9);
    for(size_t i = newEnd+1; i < stream.end; i++){
      restLeds[i].setRGB(0,0,0);
    }
  }else{
    randUpToAndBlend(restLeds, stream.start, stream.end, lowHue, highHue + 1);
    cur_state = FILLING_GLASS;
    curPower = (stream.end - stream.start);
  }
}

void stopStream(){
  curPower = (curPower < getWaterAmount()) ? 0 : (curPower - getWaterAmount());

  if(0 < curPower){
    LedIndex newEnd = curPower/10 + stream.start;
    for(size_t i = stream.start; i < newEnd ; i++){
      restLeds[i].setRGB(0,0,0);
    }
    restLeds[newEnd].setHSV(random8(lowHue, highHue + 1), 255, (curPower%10)*255/9);
    randUpToAndBlend(restLeds, newEnd + 1, stream.end, lowHue, highHue + 1);
    
  }else{
    for(size_t i = stream.start; i < stream.end; i++){
      restLeds[i].setRGB(0,0,0);
    }
    cur_state = GLASS_STAYS_FULL;
  }
}

void goingDown(){
  for(size_t i = 0; i < nbGlassLeds; i++){
    ledsHSV[i] = {random8(lowHue, highHue+1), 255};
  }

  for (size_t i = 0; i < nbGlassSegments; i++){
    int dir = glassSegments[i].dir; 
    LedIndex start = glassSegments[i].start;
    LedIndex end = glassSegments[i].end;

    MilliMeter lowHeight = CMtoMM(glassSegmentHeights[i].lowHeight);
    MilliMeter highHeight = CMtoMM(glassSegmentHeights[i].highHeight);
  
    MilliMeter curWaterHeight = waterHeight;

    if(lowHeight <= curWaterHeight && curWaterHeight < highHeight){
      
      LedIndex newPosTimes10 = mapScale(curWaterHeight, lowHeight, highHeight, dir == 1 ? start : end, dir == 1 ? end : start, 10);
      LedIndex newPos = newPosTimes10/10;

      uint8_t bri = dir == 1 ? 255 : 0;
      for(LedIndex j = start; j < newPos; j++){
        glassLeds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
      
      uint8_t afterTheDecimalPoint = dir == 1 ? (newPosTimes10 % 10) : (10 - (newPosTimes10 % 10)) ;
      glassLeds[newPos].setHSV(ledsHSV[newPos].hue, ledsHSV[newPos].sat, afterTheDecimalPoint*255/10);
      
      bri = dir == 1 ? 0 : 255;
      for(LedIndex j = newPos + 1; j <= end; j++){
        glassLeds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
    }else{
      uint8_t bri = curWaterHeight < lowHeight ? 0 : 255;
      for(LedIndex j = start; j <= end; j++){
        glassLeds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
    }
  }
  juansBlend(glassLeds, glassSegment[0].start, glassSegment[nbGlassSegments-1].end);
  
  waterHeight -= getWaterAmount();

  if(waterHeight < 0){
    cur_state = GLASS_STAYS_EMPTY;
  }
}

uint8_t dropletBrightness = 0;
int dropletDir = 1;
void dropletAnimation(uint8_t stop){
  if(dropletBrightness == 255){
    dropletDir = -1;
  }
  if(dropletBrightness == 0){
    dropletDir = 1;
    if(stop){
      return;
    }
  }
  dropletBrightness += dropletDir;
  for(LedIndex j = droplet.start; j < droplet.end; j++){
    restLeds[j].setHSV(random8(lowHue, highHue+1), 255, dropletBrightness);
  }
  juansBlend(restLeds, droplet.start, droplet.end);
}

void randUpToAndBlend(CRGB* ledsArray, LedIndex start, LedIndex end, uint8_t hueFrom, uint8_t hueTo){
  for(size_t i = start; i < end; i++){
    ledsArray[i].setHSV(random8(hueFrom, hueTo), 255, 255);
  }
  juansBlend(ledsArray, start, end);
}

void juansBlend(CRGB* ledsArray, LedIndex start, LedIndex end){
  uint8_t prev;
  if(end - start => 1){
    prev = ledsArray[start];
  }
  uint8_t cur;
  if(end - start => 2){
    cur = ledsArray[start + 1];
  }
  uint8_t next;
  for(size_t i = start + 1; i < (end - 1); i++){
    next = ledsArray[i+1];
    ledsArray[i] = (prev + cur + next)/3;
    prev = cur;
    cur = next;
  }
}

MilliMeter getWaterAmount(){
  MilliMeter waterAmount = 0;
  MilliSecond deltaTime;
  while(waterAmount == 0){
    deltaTime = millis() - previousWaterUpdateTime;
    waterAmount = (waterSpeed*deltaTime)/1000;
  }
  previousWaterUpdateTime = millis();
  return waterAmount;
}