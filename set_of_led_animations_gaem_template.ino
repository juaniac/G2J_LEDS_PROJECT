#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    202  //249 jules
#define BRIGHTNESS  200  //64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.

int compteur;
int duree_sequence = 1200; //secondes    1200 pour 20 min  300 pour 5 min
long previousMillis;
long previousMillis2;
long previousMillis3;

/*      initialisation interrupteurs      */
#define red_button 12 
#define next_button 11 
bool interr_red = false;


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


extern CRGBPalette16 myfrancecolor;    //ajout gael
extern const TProgmemPalette16 myfrancecolor_p PROGMEM;  //ajout gael

extern CRGBPalette16 red;    //ajout gael
extern const TProgmemPalette16 red_p PROGMEM;  //ajout gael      

extern CRGBPalette16 white;    //ajout gael
extern const TProgmemPalette16 white_p PROGMEM;  //ajout gael

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    pinMode(red_button,INPUT_PULLUP);
    pinMode(next_button,INPUT_PULLUP);
    Serial.begin(9600);
}


void loop()
{
    ChangePalettePeriodically();
    
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex);
    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{   /*  
    long secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand == 0)  { currentPalette = ForestColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand ==  900)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
       // if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
       // if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
       // if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
       // if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
       // if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
       //if( secondHand == 10)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
       if( secondHand == 900)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
       if( secondHand == 1800)  { currentPalette = LavaColors_p;           currentBlending = LINEARBLEND; }
       if( secondHand == 2700)  { currentPalette = OceanColors_p;           currentBlending = LINEARBLEND; }
       // if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        //if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 4500)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }*/
  
  /*
  Serial.print("compteur = ");
  Serial.println(compteur);
  Serial.print("  red_button = ");
  Serial.println(interr_red);
   Serial.print("  next_button = ");
  Serial.println(digitalRead(next_button));
  */

  switch(compteur){
      case 0 :
        currentPalette = RainbowColors_p;         
        currentBlending = LINEARBLEND;
        break;
      case 1 :
        currentPalette = ForestColors_p;         
        currentBlending = LINEARBLEND;
        break;
      case 2 :
        currentPalette = LavaColors_p;         
        currentBlending = LINEARBLEND;
        break;
      case 3 :
        currentPalette = OceanColors_p;         
        currentBlending = LINEARBLEND;  
        break; 
      case 4 :
        currentPalette = myRedWhiteBluePalette_p;          
        currentBlending = LINEARBLEND;     
        break; 
      case 5 :
        currentPalette = myfrancecolor_p;         
        currentBlending = LINEARBLEND;  
        break; 
      case 6 :   //cas spécial si bouton rouge pressé
        currentPalette = red_p;         
        currentBlending = LINEARBLEND;  
        break; 
      
    /*  case 7 :
        currentPalette = mydbzcolor_p;          
        currentBlending = LINEARBLEND;     
        break; */
} 


if ( millis()/1000 - previousMillis >= duree_sequence) {
    previousMillis = millis()/1000;   
    compteur ++;
}

if ((compteur > 5) and (interr_red == false)){   //mamaaa
     compteur = 0;
   }

if ( millis()/1000 - previousMillis3 >= 0.02) {
  previousMillis3 = millis()/1000;
  if ( digitalRead(red_button) == 0){
    interr_red = !interr_red;
    compteur = 6; //on peut rentrer dans le cas spécial et mamaaa ne nous dérangera pas pcq on dépasse compteur=5
  }
}

if ( millis()/1000 - previousMillis2 >= 0.02) {
  previousMillis2 = millis()/1000;
  if ( digitalRead(next_button) == 0){
    compteur++;
  }
}

}// void palette



// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::Red;
    currentPalette[4] = CRGB::Orange;
    currentPalette[8] = CRGB::Red;
    currentPalette[12] = CRGB::Orange;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::RoyalBlue,
    CRGB::Navy, 
    CRGB::Navy,
    CRGB::DarkViolet, // black
    
    CRGB::DarkViolet,
    CRGB::DodgerBlue,
    CRGB::Purple,
    CRGB::Navy,  //black
    
    CRGB::Navy,
    CRGB::DarkViolet,
    CRGB::Purple,
    CRGB::DodgerBlue,
    CRGB::Navy,
    CRGB::RoyalBlue,
    CRGB::DarkViolet, //black
    CRGB::Purple,  //black

    
};
                                                               
const TProgmemPalette16 myfrancecolor_p PROGMEM =
{
    CRGB::White,
    CRGB::White,
    CRGB::White,  
    
    CRGB::White,
    CRGB::White,
    CRGB::MediumBlue,
    CRGB::MediumBlue,
    CRGB::MediumBlue,
    CRGB::MediumBlue, 
    CRGB::MediumBlue,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red, 
    CRGB::Red,  
};

const TProgmemPalette16 white_p PROGMEM =
{
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,
    CRGB::White,  
};

const TProgmemPalette16 red_p PROGMEM =
{
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
};

// Additional notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
