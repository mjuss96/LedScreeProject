#include <Wire.h>
#include <RTClib.h>
#include <FastLED.h>

#define LED_PIN  6
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define BRIGHTNESS 255
#define FRAMES_PER_SECOND  120

// defining the height and width of the matrix
const uint8_t kMatrixWidth = 21;
const uint8_t kMatrixHeight = 14;
int x = 0;
int y = 0;

int valinta = 0;

// alustetaan RTC kello
RTC_DS1307 RTC;

// used for the idle menu
unsigned long previousButtonPressedMillis = 0;

// how long it will take for the IDLE mode to start (clock show)
const long interval_idle_delay = 30000;

CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

// helper function for XY coordinate
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( y & 0x01) {
    // Odd rows run backwards
        i = (y * kMatrixWidth) + x;
  } else {
    // Even rows run forwards
     uint8_t reverseX = (kMatrixWidth - 1) - x;
    i = (y * kMatrixWidth) + reverseX;
  }
  
  return i;
}

// defining states in ASCII
#define NoneState '9'
#define LeftState '0'
#define RightState '1'
#define DownState '2'
#define UpState '3'
#define SelectState '5'

int buttonPressed = NoneState;
int state = 0;

// reads serial from bluetooth - 
// buttonPressed variable will hold the latest state of the latest button pressed in the App.
void ReadBluetooth(){
   if(Serial.available() > 0){
      state = Serial.read();
   }

   // previousButton.. used for idle mode. If button isn't pressed for 5sec - idle mode is activated.
   if(state > 0) {
      previousButtonPressedMillis = millis();
      
      // only change buttonPressed, if different button has been pressed.
      if(state)
          buttonPressed = state;

      
     // if Back button has been pressed, go back to main menu (valinta 0)
     if(state == 54) {
        FastLED.clear();
        valinta = 0;
     }
   }

   state = 0;
}

// display word, letters are defined in !letters file!
// example usage: displayWord("sana", x, y, red, green, blue);
void displayWord(const char * letter, int start_x, int start_y, int r, int g, int b) {
  int x = start_x;
  int y = start_y;
  for (int i = 0; i < strlen(letter); i++) {
    const char what = letter[i];
    
    // jos kirjain on I tai huutomerkki jne, jätä 2 väliä
    if(what == 'i' || what == '!' || what == ':') {
      x = x-2;
    } else if( what == '1') 
      x--;
  
    displayLetter(what, x, y, r, g, b);
    x=x+4;
  }
  
  FastLED.show();
}


int now;
// setup everything necessary, start bluetooth Serial.
void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
     Serial.println("RTC is NOT running!");
     RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // necessary for random() function to work properly. Analog 3 is empty and gives random values.
  randomSeed(analogRead(3));
}

// this variable holds the current selection on the menu.
int selectorPosition = 0;
// this is where menu placement start x,y - where the text starts.
int start_x = 1;
int start_y = 6;
int displayLeds = 1;

// wrapper function for setting LED red green blue color. on the coordinate
void setLedXYRGB(int x, int y, int r, int g, int b) {
  leds[XY(x, y)].setRGB(r, g, b);
}

unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
const long interval = 500;

void displayMenu() {

  // if user presses to go right or left on the menu
  if(buttonPressed == LeftState || buttonPressed == RightState) {
    FastLED.clear();

    // if "right" button is pressed, move the selector right, else left. (left button pressed)
    if(buttonPressed == RightState) {
        selectorPosition++;
    } else { 
        selectorPosition--;
    }

    // reset button state, a little humanly delay.
    buttonPressed = NoneState;
    delay(200);
  }

  if(selectorPosition > 2)
    selectorPosition = 0;
  if(selectorPosition < 0)
    selectorPosition = 2;


  // needed for blinking the selector box
  if ((millis() - previousMillis) > interval) {
    // save the last time you blinked the LED
    previousMillis = millis();
    if(displayLeds == 1)
      displayLeds = 0;
    else
      displayLeds = 1;
  }
  
  // showing the menu here.
  if(selectorPosition == 0) {
    displayWord("anim", start_x+2, start_y, 30, 206, 33);

    // if select button is pressed while this text is showing. Show animations (valinta 3)
     if(buttonPressed == SelectState) {
        valinta = 3;
        buttonPressed = 0;
    }
  } else if(selectorPosition == 1) {
    displayWord("jump", start_x+2, start_y, 244, 22, 22);
    if(buttonPressed == SelectState) {
        valinta = 2;
        buttonPressed = 0;
    }
  } else if (selectorPosition == 2) {
    displayWord("snake", start_x, start_y, 244, 22, 233);
    if(buttonPressed == SelectState) {
        valinta = 1;
        buttonPressed = 0;
    }
  }

  // display "selector" bar 
  if(displayLeds == 1) {
    for(int i = 0; i < 3; i++) {
      setLedXYRGB(start_x+i, start_y-3, 30, 206, 33);
      setLedXYRGB(start_x+i, start_y-4, 30, 206, 33);
  
      setLedXYRGB(start_x+i+4, start_y-3, 244, 22, 22);
      setLedXYRGB(start_x+i+4, start_y-4, 244, 22, 22);
  
      setLedXYRGB(start_x+i+8, start_y-3, 244, 22, 233);
      setLedXYRGB(start_x+i+8, start_y-4, 244, 22, 233);
    }
  } else {
    // make the selected bar Flash/blink..
    for(int i = 0; i < 3; i++) {
      leds[XY((start_x+i)+selectorPosition*4, start_y-3)] = CRGB::Black;
      leds[XY((start_x+i)+selectorPosition*4, start_y-4)] = CRGB::Black;
    }
  }

 // buttonPressed = NoneState;
  FastLED.show();
}

int minute;
int hour;
void displayIdleMode() {
  // get time
  DateTime now = RTC.now();

  // only display clock if the minute has changed from previous one (so no unwanted blink effect)
  if(now.minute() != minute) {
      FastLED.clear();
      minute = now.minute(); 
      hour = now.hour();
      // show time
      int start_x = 2;
      displayNumbers(hour, 2, start_x, 4, 30, 206, 203);
      start_x+=7;
      displayWord(":", start_x+1, 4, 30, 206, 203);
      start_x+=2;
      displayNumbers(minute,2, start_x+1, 4, 0, 255, 229);
  }
}

/*
 * Ran out of time so these animations are copied from FastLED examples library 
 * https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino
 * 
 */


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// display animations/or display a selection of animations (new menu with few animations to choose from)
void animaatioValikko() {
   // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}


// main loop
void loop() {
   // keep reading for user inputs
   ReadBluetooth();
   
   // if no user input for 10sec(interval_idle_delay), go to IDLE MODE
   if (valinta != 3 && ((millis() - previousButtonPressedMillis) > interval_idle_delay)) {
      // save the last time you blinked the LED
      displayIdleMode();
      return;
   }
  
   // select what game to play.
   switch(valinta) {
     case 0:
        displayMenu();
       break;
     case 1:
        snake_loop();
        break;
     case 2:
        tRex_loop();
        break;
     case 3:
        animaatioValikko();
        break;
        
    }
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


