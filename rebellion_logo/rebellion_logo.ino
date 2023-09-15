#include <FastLED.h>

#define LED_PIN 2 

#define BRIGHTNESS  100  //64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define nbLeds 202
CRGB leds[nbLeds];
uint8_t heat[nbLeds];

#define CoolAmount 7

#define nbLeds 202
#define nbStartingpoints 2
int startingPoints[] = {0,201};

#define nbIdenticalPairPoints 2
struct pairPoints{
  int point1;
  int point2;
};

pairPoints identicalPairPoints[] = {{25, 66}, {66, 136}};

#define nbPathAndDirections 6
struct PAD{
  int start;
  int end;
  int dir;
};
PAD pathAndDirections[] = {{0, 44, 1},{45, 66, -1},{66, 101, 1},{102, 136, -1},{136, 157, 1},{158, 201, -1}};

int startPause = 0;

#define UPDATES_PER_SECOND 240

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, nbLeds).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    Serial.begin(9600);
}

void loop()
{
    FireImproved();
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FireImproved(){
        //cooldown cells by a random amount
        for (size_t i = 0; i < nbLeds; i++){
          uint8_t newHeat = random8(CoolAmount - 5, CoolAmount + 5);
          heat[i] = (heat[i] <= newHeat) ? 0 : (heat[i] - newHeat);
        }

        //spread the fire through all paths
        for (size_t i = 0; i < nbPathAndDirections; i++){
            int dir = pathAndDirections[i].dir;
            int start = dir == 1 ? pathAndDirections[i].start : pathAndDirections[i].end;
            int end = dir == 1 ? pathAndDirections[i].end : pathAndDirections[i].start;
            for (int k = end; k != start; k -= dir){
                uint8_t acc = 0;
                int nbOfFusions = constrain((dir*(k - start)), 0, 3);

                for (int l = 1; l <= nbOfFusions; l++){
                    acc += (l*heat[k -dir*l]) / fact(nbOfFusions);
                }
                if(nbOfFusions != 0){
                    heat[k] = acc;
                }
            }
        }

        //copy identicalPoints
        for (size_t i = 0; i < nbIdenticalPairPoints; i++){
            uint16_t index1 = identicalPairPoints[i].point1;
            uint16_t index2 = identicalPairPoints[i].point2;
            uint8_t maxHeat = max(heat[index1], heat[index2]);
            heat[index1] = maxHeat;
            heat[index2] = maxHeat;
        }


        //start new fires at the starting points
          for (size_t i = 0; i < nbStartingpoints; i++){
              heat[startingPoints[i]] = random8(200, 255);
          }

        //map heat to color
        for (size_t j = 0; j < nbLeds; j++)
        {
            uint8_t curHeat = heat[j];
            //leds[j].setRGB(0, 0, interpolate0to100(curHeat, 0, 255));

            if (curHeat <= 0) { leds[j] = CRGB(0, 0, 0); }
            else if (curHeat <= 75) { leds[j] = CRGB(interpolate0to255(curHeat, 1, 75), 0, 0); }
            else if (curHeat <= 175) { leds[j] = CRGB(255, interpolate0to255(curHeat, 76, 175), 0); }
            else { leds[j] = CRGB(255, 255, interpolate0to255(curHeat, 176, 255)); }
            //if (curHeat <= 70) { leds[j].setHSV(0, 0, 0); }
            //else if (curHeat <= 170) { leds[j].setHSV(0, 255, interpolate0to255(curHeat, 71, 170)); }
            //else if (curHeat <= 220) { leds[j].setHSV(interpolate0to60(curHeat, 171, 220), 255, 255); }
            //else { leds[j].setHSV(60, 255 - interpolate0to255(curHeat, 220, 255), 255); }
        }
    }

    uint8_t interpolate0to255(uint8_t val, uint8_t min, uint8_t max){
        return (255*(val - min))/(max - min);
    }
    uint8_t interpolate0to100(uint8_t val, uint8_t min, uint8_t max){
        return (100*(val - min))/(max - min);
    }
    uint8_t interpolate0to60(uint8_t val, uint8_t min, uint8_t max){
        return (60*(val - min))/(max - min);
    }

    int fact(int k){
      return (k==1 || k==0) ? 1 : k*fact(k-1);
    }