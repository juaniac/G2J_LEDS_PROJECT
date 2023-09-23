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
const SegmentHeights segmentHeights[] = {{0, 56}, {27, 56}, {27, 54}, {48, 54}, {48, 63}, {48, 63} ,{48, 54}, {27, 54}, {27, 56}, {0, 56}};

struct HSV{
  uint8_t hue;
  uint8_t sat;
};
HSV ledsHSV[nbLeds];

struct Bubble{
  MilliMeter height;
  MilliMeterPerSecond speed;
  uint8_t segmentId;
};
#define maxBubbles 10
Bubble* bubbles[maxBubbles];
#define createBubbleRate 100
#define initialBubbleSpeed (3*waterSpeed)
#define BubbleLength 30
MilliSecond previousCreatedBubbleTime;

MilliMeter waterHeight = 0;
MilliSecond previousWaterUpdateTime;
MilliMeterPerSecond waterSpeed = 80;
#define maxWaveHeight 20

void setup() {
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, nbLeds).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  Serial.begin(9600);

  for(size_t i = 0; i < nbLeds; i++){
    leds[i].setRGB(0,0,0);
  }
  FastLED.show();
  delay(3000);

  previousWaterUpdateTime = millis();
  previousCreatedBubbleTime = millis();
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
  if(waterHeight > CMtoMM(63) + maxWaveHeight){return;}

  for(size_t i = 0; i < nbLeds; i++){
    ledsHSV[i] = {150, 255};
  }

  for (size_t i = 0; i < nbSegments; i++){
    int dir = segments[i].dir;
    LedIndex start = segments[i].start;
    LedIndex end = segments[i].end;

    MilliMeter lowHeight = CMtoMM(segmentHeights[i].lowHeight);
    MilliMeter highHeight = CMtoMM(segmentHeights[i].highHeight);
  
    int waveHeight = beatsin16(90, 0, 2*maxWaveHeight, 0, (i%4)*65535/4) - maxWaveHeight; 
    MilliMeter curWaterHeight = max(0, int(waterHeight + waveHeight));

    
    for(size_t j = 0; j < maxBubbles; j++){
      Bubble* bubble = bubbles[j];
      if(bubble != NULL && bubble->segmentId == i){
        MilliMeter bubbleHeight = bubble->height;

        if(bubbleHeight > highHeight || bubbleHeight > curWaterHeight){
          free(bubble);
          bubbles[j] = NULL;
          Serial.print(j);
          Serial.println(" delete Bubble");
          Serial.println(bubbles[j] == NULL ? "BUBBLE NULL" : "BUBBLE NOT NULL");
          Serial.println();
          
          //printBubbleArray();
        }else{
          
          bubbleHeight = max(0, int(bubbleHeight + beatsin16(90, 0, 2*10) - 10));

          LedIndex bottomIndexTimes10 = constrain(mapScale(bubbleHeight, lowHeight, highHeight, dir == 1 ? start : end, dir == 1 ? end : start, 10), start*10, end*10);
          LedIndex bottomIndex = bottomIndexTimes10/10;
          LedIndex topIndexTimes10 = constrain(mapScale(bubbleHeight + BubbleLength, lowHeight, highHeight, dir == 1 ? start : end, dir == 1 ? end : start, 10), start*10, end*10);
          LedIndex topIndex = topIndexTimes10/10;

          uint8_t topAfterTheDecimalPoint = dir == 1 ? (10 - (topIndexTimes10 % 10)) : (topIndexTimes10 % 10) ;
          //leds[topIndex].setHSV(ledsHSV[topIndex].hue, topAfterTheDecimalPoint*255/10, 255);
          ledsHSV[topIndex].sat = topAfterTheDecimalPoint*255/10;

          uint8_t bottomAfterTheDecimalPoint = dir == 1 ? (bottomIndexTimes10 % 10) : (10 - (bottomIndexTimes10 % 10)) ;
          //leds[bottomIndex].setHSV(ledsHSV[bottomIndex].hue, bottomAfterTheDecimalPoint*255/10, 255);
          ledsHSV[bottomIndex].sat = bottomAfterTheDecimalPoint*255/10;

          for(LedIndex k = bottomIndex + dir; dir == 1 ? k < topIndex : k > topIndex; k+= dir){
            //leds[k].setHSV(ledsHSV[k].hue, 0, 255);
            ledsHSV[k].sat = 0;
          }             
        } 
      }
    }
    

    if(lowHeight <= curWaterHeight && curWaterHeight < highHeight){
      
      LedIndex newPosTimes10 = mapScale(curWaterHeight, lowHeight, highHeight, dir == 1 ? start : end, dir == 1 ? end : start, 10);
      LedIndex newPos = newPosTimes10/10;

      uint8_t bri = dir == 1 ? 255 : 0;
      for(LedIndex j = start; j < newPos; j++){
        leds[j].setHSV(ledsHSV[j].hue, ledsHSV[j].sat, bri);
      }
      
      uint8_t afterTheDecimalPoint = dir == 1 ? (newPosTimes10 % 10) : (10 - (newPosTimes10 % 10)) ;
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
  updateHeights();
}

long mapScale(long x, long in_min, long in_max, long out_min, long out_max, long scale) {
  return (x - in_min) * (out_max - out_min) * scale / (in_max - in_min) + out_min * scale;
}

void updateHeights(){

  MilliMeter waterAmount = 0;
  MilliSecond deltaTime;
  while(waterAmount == 0){
    deltaTime = millis() - previousWaterUpdateTime;
    waterAmount = (waterSpeed*deltaTime)/1000;
  }
  previousWaterUpdateTime = millis();
  waterHeight += waterAmount;

  for(size_t i = 0; i < maxBubbles; i++){
    Bubble* bubble = bubbles[i];
    if(bubble != NULL){
      bubble->height += (bubble->speed*deltaTime)/1000;
    }else if(millis() - previousCreatedBubbleTime > createBubbleRate){
      Serial.println(bubble == NULL ? "BUBBLE NULL" : "BUBBLE NOT NULL");

      while(bubble == NULL){bubble = calloc(1, sizeof(Bubble));}

      bubble->segmentId = random16(0, nbSegments);
      bubble->height = CMtoMM(segmentHeights[bubble->segmentId].lowHeight);
      bubble->speed = initialBubbleSpeed;
      bubbles[i] = bubble;

      Serial.print(i);
      Serial.println(" create Bubble");
      Serial.println();
      //printBubbleArray();

      previousCreatedBubbleTime = millis();
    }
  }
}

void printBubbleArray(){
  Serial.println("BUBBLE ARRAY:");
  for(size_t i = 0; i < maxBubbles; i++){
    Serial.print(i);
    if(bubbles[i] == NULL){
      Serial.println(" EMPTY");
    }else{
      Serial.println(" A BUBBLE");
    }
  }
  Serial.println();
}