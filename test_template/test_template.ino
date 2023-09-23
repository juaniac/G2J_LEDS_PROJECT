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

struct DATA{
  int int1;
  int int2;
  int int3;
};
DATA* ptrData = NULL;

void loop(){
  while(ptrData == NULL){ptrData = calloc(1, sizeof(DATA));}
  dealy(5000);
  free(ptrData);
  ptrData = NULL;
  dealy(5000);
  Serial.println("okay");
} 