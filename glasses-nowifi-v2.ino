// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>

// This #include statement was automatically added by the Particle IDE.
#include <SparkIntervalTimer.h>

// This #include statement was automatically added by the Particle IDE.
#include <FastLED.h>



// This #include statement was automatically added by the Particle IDE.
#include <SparkIntervalTimer.h>

// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>


// This #include statement was automatically added by the Particle IDE.
#include "FastLED/FastLED.h"

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

IntervalTimer myTimer;		// 3 for the Core
volatile bool  onLine = true;

#include <math.h>
FASTLED_USING_NAMESPACE;

#define LED_PIN  3

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 64


// RGB Shades color order (Green/Red/Blue)
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

// Global maximum brightness value, maximum 255
#define MAXBRIGHTNESS 72
#define STARTBRIGHTNESS 102

// Cycle time (milliseconds between pattern changes)
#define cycleTime 15000

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

#define bufl 100

char buf[bufl] ="HELLO";
 
    
// Global variables
boolean effectInit = false; // indicates if a pattern has been recently switched
uint16_t effectDelay = 0; // time between automatic effect changes
unsigned long effectMillis = 0; // store the time of last effect function run
unsigned long cycleMillis = 0; // store the time of last effect change
unsigned long currentMillis; // store current loop's millis value
unsigned long hueMillis; // store time of last hue change
unsigned long eepromMillis; // store time of last setting change
byte currentEffect = 0; // index to the currently running effect
boolean autoCycle = true; // flag for automatic effect changes
boolean eepromOutdated = false; // flag for when EEPROM may need to be updated
byte currentBrightness = STARTBRIGHTNESS; // 0-255 will be scaled to 0-MAXBRIGHTNESS

const char Font[][5] PROGMEM = {
{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}, // 32 <space>
{0b00000000, 0b00000000, 0b00010111, 0b00000000, 0b00000000}, // 33 !
{0b00000000, 0b00000011, 0b00000000, 0b00000011, 0b00000000}, // 34 "
{0b00001010, 0b00011111, 0b00001010, 0b00011111, 0b00001010}, // 35 #
{0b00010010, 0b00010101, 0b00011111, 0b00010101, 0b00001001}, // 36 $
{0b00010001, 0b00001000, 0b00000100, 0b00000010, 0b00010001}, // 37 %
{0b00001010, 0b00010101, 0b00001010, 0b00010000, 0b00000000}, // 38 &
{0b00000000, 0b00000000, 0b00000011, 0b00000000, 0b00000000}, // 39 '
{0b00000000, 0b00000000, 0b00001110, 0b00010001, 0b00000000}, // 40 (
{0b00000000, 0b00010001, 0b00001110, 0b00000000, 0b00000000}, // 41 )
{0b00010001, 0b00001010, 0b00011111, 0b00001010, 0b00010001}, // 42 *
{0b00000100, 0b00000100, 0b00011111, 0b00000100, 0b00000100}, // 43 +
{0b00000000, 0b00010000, 0b00001000, 0b00000000, 0b00000000}, // 44 ,
{0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b00000100}, // 45 -
{0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000}, // 46 .
{0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001}, // 47 /
{0b00001110, 0b00010001, 0b00010101, 0b00010001, 0b00001110}, // 48 0
{0b00000000, 0b00010010, 0b00011111, 0b00010000, 0b00000000}, // 49 1
{0b00010010, 0b00011001, 0b00010101, 0b00010101, 0b00010010}, // 50 2
{0b00010101, 0b00010101, 0b00010101, 0b00010101, 0b00001010}, // 51 3
{0b00000111, 0b00000100, 0b00000100, 0b00011111, 0b00000100}, // 52 4
{0b00010111, 0b00010101, 0b00010101, 0b00010101, 0b00001001}, // 53 5
{0b00001110, 0b00010101, 0b00010101, 0b00010101, 0b00001000}, // 54 6
{0b00000001, 0b00000001, 0b00011001, 0b00000101, 0b00000011}, // 55 7
{0b00001010, 0b00010101, 0b00010101, 0b00010101, 0b00001010}, // 56 8
{0b00000010, 0b00010101, 0b00010101, 0b00010101, 0b00001110}, // 57 9
{0b00000000, 0b00000000, 0b00001010, 0b00000000, 0b00000000}, // 58 :
{0b00000000, 0b00010000, 0b00001010, 0b00000000, 0b00000000}, // 59 ;
{0b00000000, 0b00000100, 0b00001010, 0b00010001, 0b00000000}, // 60 <
{0b00001010, 0b00001010, 0b00001010, 0b00001010, 0b00001010}, // 61 =
{0b00000000, 0b00010001, 0b00001010, 0b00000100, 0b00000000}, // 62 >
{0b00000010, 0b00000001, 0b00010101, 0b00000101, 0b00000010}, // 63 ?
{0b00011111, 0b00010001, 0b00010101, 0b00010101, 0b00010111}, // 64 @
{0b00011110, 0b00000101, 0b00000101, 0b00000101, 0b00011110}, // 65 A
{0b00011111, 0b00010101, 0b00010101, 0b00010101, 0b00001010}, // 66 B
{0b00001110, 0b00010001, 0b00010001, 0b00010001, 0b00010001}, // 67 C
{0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00001110}, // 68 D
{0b00011111, 0b00010101, 0b00010101, 0b00010001, 0b00010001}, // 69 E
{0b00011111, 0b00000101, 0b00000101, 0b00000001, 0b00000001}, // 70 F
{0b00001110, 0b00010001, 0b00010101, 0b00010101, 0b00001101}, // 71 G
{0b00011111, 0b00000100, 0b00000100, 0b00000100, 0b00011111}, // 72 H
{0b00010001, 0b00010001, 0b00011111, 0b00010001, 0b00010001}, // 73 I
{0b00001000, 0b00010000, 0b00010000, 0b00010001, 0b00001111}, // 74 J
{0b00011111, 0b00000100, 0b00001010, 0b00010001, 0b00010001}, // 75 K
{0b00011111, 0b00010000, 0b00010000, 0b00010000, 0b00010000}, // 76 L
{0b00011111, 0b00000010, 0b00000100, 0b00000010, 0b00011111}, // 77 M
{0b00011111, 0b00000010, 0b00000100, 0b00001000, 0b00011111}, // 78 N
{0b00001110, 0b00010001, 0b00010001, 0b00010001, 0b00001110}, // 79 O
{0b00011111, 0b00000101, 0b00000101, 0b00000101, 0b00000010}, // 80 P
{0b00001110, 0b00010001, 0b00010001, 0b00001001, 0b00010110}, // 81 Q
{0b00011111, 0b00000101, 0b00000101, 0b00001101, 0b00010010}, // 82 R
{0b00010010, 0b00010101, 0b00010101, 0b00010101, 0b00001001}, // 83 S
{0b00000001, 0b00000001, 0b00011111, 0b00000001, 0b00000001}, // 84 T
{0b00001111, 0b00010000, 0b00010000, 0b00010000, 0b00001111}, // 85 U
{0b00000011, 0b00001100, 0b00010000, 0b00001100, 0b00000011}, // 86 V
{0b00000111, 0b00011000, 0b00000110, 0b00011000, 0b00000111}, // 87 W
{0b00010001, 0b00001010, 0b00000100, 0b00001010, 0b00010001}, // 88 X
{0b00000001, 0b00000010, 0b00011100, 0b00000010, 0b00000001}, // 89 Y
{0b00010001, 0b00011001, 0b00010101, 0b00010011, 0b00010001}, // 90 Z
{0b00000000, 0b00000000, 0b00011111, 0b00010001, 0b00010001}, // 91 [
{0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000}, // 92 slash
{0b00010001, 0b00010001, 0b00011111, 0b00000000, 0b00000000}, // 93 ]
{0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100}, // 94 ^
{0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000}, // 95 _
{0b00011111, 0b00010001, 0b00010001, 0b00010001, 0b00011111}}; // 96 <block>








//MQTT client("realtime.ngi.ibm.com", 1883, callback);

boolean glassesOn=true;

const char string0[] PROGMEM = "HELLO WORLD  ";
const char string1[] PROGMEM = "RGB SHADES!  ";
const char string2[] PROGMEM = "macetech.com  ";

const char * const stringArray[] PROGMEM = {
  string0,
  string1,
  string2
};

typedef void (*functionList)(); // definition for list of effect function pointers
extern const byte numEffects;


// Hue time (milliseconds between hue increments)
#define hueTime 30

// Helper functions for an two-dimensional XY matrix of pixels.
// Simple 2-D demo code is included as well.
//
//     XY(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//             No error checking is performed on the ranges of x and y.
//
//     XYsafe(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//             Error checking IS performed on the ranges of x and y, and an
//             index of "-1" is returned.  Special instructions below
//             explain how to use this without having to do your own error
//             checking every time you use this function.  
//             This is a slightly more advanced technique, and 
//             it REQUIRES SPECIAL ADDITIONAL setup, described below.


// Params for width and height
const uint8_t kMatrixWidth = 16;

const uint8_t kMatrixHeight = 5;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;
// Set 'kMatrixSerpentineLayout' to false if your pixels are 
// laid out all running the same way, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//     .----<----<----<----'
//     |
//     5 >  6 >  7 >  8 >  9
//                         |
//     .----<----<----<----'
//     |
//    10 > 11 > 12 > 13 > 14
//                         |
//     .----<----<----<----'
//     |
//    15 > 16 > 17 > 18 > 19
//
// Set 'kMatrixSerpentineLayout' to true if your pixels are 
// laid out back-and-forth, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//                         |
//     9 <  8 <  7 <  6 <  5
//     |
//     |
//    10 > 11 > 12 > 13 > 14
//                        |
//                        |
//    19 < 18 < 17 < 16 < 15
//
// Bonus vocabulary word: anything that goes one way 
// in one row, and then backwards in the next row, and so on
// is call "boustrophedon", meaning "as the ox plows."


// This function will return the right 'led index number' for 
// a given set of X and Y coordinates on your matrix.  
// IT DOES NOT CHECK THE COORDINATE BOUNDARIES.  
// That's up to you.  Don't pass it bogus values.
//
// Use the "XY" function like this:
//
//    for( uint8_t x = 0; x < kMatrixWidth; x++) {
//      for( uint8_t y = 0; y < kMatrixHeight; y++) {
//      
//        // Here's the x, y to 'led index' in action: 
//        leds[ XY( x, y) ] = CHSV( random8(), 255, 255);
//      
//      }
//    }
//
//



// Once you've gotten the basics working (AND NOT UNTIL THEN!)
// here's a helpful technique that can be tricky to set up, but 
// then helps you avoid the needs for sprinkling array-bound-checking
// throughout your code.
//
// It requires a careful attention to get it set up correctly, but
// can potentially make your code smaller and faster.
//
// Suppose you have an 8 x 5 matrix of 40 LEDs.  Normally, you'd
// delcare your leds array like this:
//    CRGB leds[40];
// But instead of that, declare an LED buffer with one extra pixel in
// it, "leds_plus_safety_pixel".  Then declare "leds" as a pointer to
// that array, but starting with the 2nd element (id=1) of that array: 
//    CRGB leds_with_safety_pixel[41];
//    const CRGB* leds( leds_plus_safety_pixel + 1);
// Then you use the "leds" array as you normally would.
// Now "leds[0..N]" are aliases for "leds_plus_safety_pixel[1..(N+1)]",
// AND leds[-1] is now a legitimate and safe alias for leds_plus_safety_pixel[0].
// leds_plus_safety_pixel[0] aka leds[-1] is now your "safety pixel".
//
// Now instead of using the XY function above, use the one below, "XYsafe".
//
// If the X and Y values are 'in bounds', this function will return an index
// into the visible led array, same as "XY" does.
// HOWEVER -- and this is the trick -- if the X or Y values
// are out of bounds, this function will return an index of -1.
// And since leds[-1] is actually just an alias for leds_plus_safety_pixel[0],
// it's a totally safe and legal place to access.  And since the 'safety pixel'
// falls 'outside' the visible part of the LED array, anything you write 
// there is hidden from view automatically.
// Thus, this line of code is totally safe, regardless of the actual size of
// your matrix:
//    leds[ XYsafe( random8(), random8() ) ] = CHSV( random8(), 255, 255);
//
// The only catch here is that while this makes it safe to read from and
// write to 'any pixel', there's really only ONE 'safety pixel'.  No matter
// what out-of-bounds coordinates you write to, you'll really be writing to
// that one safety pixel.  And if you try to READ from the safety pixel,
// you'll read whatever was written there last, reglardless of what coordinates
// were supplied.

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* leds( leds_plus_safety_pixel + 1);



// test

CRGBPalette16 currentPalette(RainbowColors_p); // global palette storage

typedef void (*functionList)(); // definition for list of effect function pointers
extern const byte numEffects;

// Increment the global hue value for functions that use it
byte cycleHue = 0;
byte cycleHueCount = 0;
void hueCycle(byte incr) {
    cycleHueCount = 0;
    cycleHue+=incr;
}

// Fade every LED in the array by a specified amount
void fadeAll(byte fadeIncr) {
  for (byte i = 0; i < NUM_LEDS; i++) {
    leds[i] = leds[i].fadeToBlackBy(fadeIncr);
  }
}

// Shift all pixels by one, right or left (0 or 1)
void scrollArray(byte scrollDir) {
  
    byte scrollX = 0;
    for (byte x = 1; x < kMatrixWidth; x++) {
      if (scrollDir == 0) {
        scrollX = kMatrixWidth - x;
      } else if (scrollDir == 1) {
        scrollX = x - 1;
      }
      
      for (byte y = 0; y < kMatrixHeight; y++) {
        leds[XY(scrollX,y)] = leds[XY(scrollX + scrollDir*2 - 1,y)];
      }
    }
  
}


// Pick a random palette from a list
void selectRandomPalette() {

  switch(random8(8)) {
    case 0:
    currentPalette = CloudColors_p;
    break;
    
    case 1:
    currentPalette = LavaColors_p;
    break;
    
    case 2:
    currentPalette = OceanColors_p;
    break;
    
    case 4:
    currentPalette = ForestColors_p;
    break;
    
    case 5:
    currentPalette = RainbowColors_p;
    break;
    
    case 6:
    currentPalette = PartyColors_p;
    break;
    
    case 7:
    currentPalette = HeatColors_p;
    break;
  }

}




// test



uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}

// top

// Helper functions for a two-dimensional XY matrix of pixels.
// Special credit to Mark Kriegsman
//
// 2014-10-18 - Special version for RGB Shades Kickstarter
//              https://www.kickstarter.com/projects/macetech/rgb-led-shades
//              2014-10-18 - code version 2c (local table, holes are r/w), 
//              by Mark Kriegsman 
//
//              This special 'XY' code lets you program the RGB Shades
//              as a plain 16x5 matrix.  
//
//              Writing to and reading from the 'holes' in the layout is 
//              also allowed; holes retain their data, it's just not displayed.
//
//              You can also test to see if you're on or off the layout
//              like this
//                if( XY(x,y) > LAST_VISIBLE_LED ) { ...off the layout...}
//
//              X and Y bounds checking is also included, so it is safe
//              to just do this without checking x or y in your code:
//                leds[ XY(x,y) ] == CRGB::Red;
//              All out of bounds coordinates map to the first hidden pixel.
//
//     XY(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//
// Params for width and height
//const uint8_t kMatrixWidth = 16;
//const uint8_t kMatrixHeight = 5;

// Pixel layout
//
//      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
//   +------------------------------------------------
// 0 |  .  0  1  2  3  4  5  6  7  8  9 10 11 12 13  .
// 1 | 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14
// 2 | 30 31 32 33 34 35 36  .  . 37 38 39 40 41 42 43
// 3 | 57 56 55 54 53 52 51  .  . 50 49 48 47 46 45 44
// 4 |  . 58 59 60 61 62  .  .  .  . 63 64 65 66 67  .

//#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
//CRGB leds[ NUM_LEDS ];


// This function will return the right 'led index number' for 
// a given set of X and Y coordinates on your RGB Shades. 
// This code, plus the supporting 80-byte table is much smaller 
// and much faster than trying to calculate the pixel ID with code.
#define LAST_VISIBLE_LED 67

// Fade every LED in the array by a specified amount
//void fadeAll(byte fadeIncr) {
//  for (byte i = 0; i < LAST_VISIBLE_LED; i++) {
//    leds[i] = leds[i].fadeToBlackBy(fadeIncr);
//  }
//}


uint8_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t ShadesTable[] = {
     68,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 69,
     29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14,
     30, 31, 32, 33, 34, 35, 36, 70, 71, 37, 38, 39, 40, 41, 42, 43,
     57, 56, 55, 54, 53, 52, 51, 72, 73, 50, 49, 48, 47, 46, 45, 44,
     74, 58, 59, 60, 61, 62, 75, 76, 77, 78, 63, 64, 65, 66, 67, 79
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = ShadesTable[i];
  return j;
}


// Assorted useful functions and variables

//byte cycleHue = 0;
//byte cycleHueCount = 0;

//CRGBPalette16 currentPalette(RainbowColors_p); // global palette storage
//void hueCycle(byte incr) {
//    cycleHueCount = 0;
//    cycleHue+=incr;
//}



// bot 

//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

// Triple Sine Waves
void threeSine() {

  static byte sineOffset = 0; // counter for current position of sine waves

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {

      // Calculate "sine" waves with varying periods
      // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
      byte sinDistanceR = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 9 + x * 16)), 2);
      byte sinDistanceG = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 10 + x * 16)), 2);
      byte sinDistanceB = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 11 + x * 16)), 2);

      leds[XY(x, y)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
    }
  }

  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle

}

float sq(float a){
    return a*a;
}
// RGB Plasma
void plasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector / 256);
  int yOffset = sin8(plasVector / 256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
      leds[XY(x, y)] = CHSV(color, 255, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}


// Scanning pattern left/right, uses global hue cycle
void rider() {

  static byte riderPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    riderPos = 0;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(cycleHue, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic

}


// Shimmering noise, uses global hue cycle
void glitter() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, random8(5) * 63);
    }
  }

}


// Fills saturated colors into the array from alternating directions
void colorFill() {

  static byte currentColor = 0;
  static byte currentRow = 0;
  static byte currentDirection = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 45;
    currentColor = 0;
    currentRow = 0;
    currentDirection = 0;
    currentPalette = RainbowColors_p;
  }

  // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
  if (!(currentDirection & 1)) {
    effectDelay = 45; // slower since vertical has fewer pixels
    for (byte x = 0; x < kMatrixWidth; x++) {
      byte y = currentRow;
      if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
  if (currentDirection & 1) {
    effectDelay = 20; // faster since horizontal has more pixels
    for (byte y = 0; y < kMatrixHeight; y++) {
      byte x = currentRow;
      if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  currentRow++;

  // detect when a fill is complete, change color and direction
  if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
    currentRow = 0;
    currentColor += random8(3, 6);
    if (currentColor > 15) currentColor -= 16;
    currentDirection++;
    if (currentDirection > 3) currentDirection = 0;
    effectDelay = 300; // wait a little bit longer after completing a fill
  }


}

// Emulate 3D anaglyph glasses
void threeDee() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 50;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x < 7) {
        leds[XY(x, y)] = CRGB::Blue;
      } else if (x > 8) {
        leds[XY(x, y)] = CRGB::Red;
      } else { 
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  leds[XY(6, 0)] = CRGB::Black;
  leds[XY(9, 0)] = CRGB::Black;

}

// Emulate 3D anaglyph glasses
void fillAll( int col ) {


  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
        leds[XY(x, y)] = col;
    
    }
  }

 
}


// Random pixels scroll sideways, uses current hue
#define rainDir 0





void sideRain() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
  }

  scrollArray(rainDir);
  byte randPixel = random8(kMatrixHeight);
  for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth - 1) * rainDir, y)] = CRGB::Black;
  leds[XY((kMatrixWidth - 1)*rainDir, randPixel)] = CHSV(cycleHue, 255, 255);

}


void confetti() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
  }

  // scatter random colored pixels at several random coordinates
  for (byte i = 0; i < 4; i++) {
    leds[XY(random16(kMatrixWidth), random16(kMatrixHeight))] = ColorFromPalette(currentPalette, random16(255), 255); //CHSV(random16(255), 255, 255);
    random16_add_entropy(1);
  }

}


// Draw slanting bars scrolling across the array, uses current hue
void slantBars() {

  static byte slantPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, quadwave8(x * 32 + y * 32 + slantPos));
    }
  }

  slantPos -= 4;

}
#define NORMAL 0
#define RAINBOW 1
#define charSpacing 2


#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))



// Determine flash address of text string
unsigned int currentStringAddress = 0;
void selectFlashString(byte string) {
  currentStringAddress = pgm_read_word(&stringArray[string]);
}

// Fetch font character bitmap from flash
byte charBuffer[5] = {0};
void loadCharBuffer(byte character) {
Serial.print("LCB char:");
Serial.println(character);
  byte mappedCharacter = character;
  if (mappedCharacter >= 32 && mappedCharacter <= 95) {
    mappedCharacter -= 32; // subtract font array offset
  } else if (mappedCharacter >= 97 && mappedCharacter <= 122) {
    mappedCharacter -= 64; // subtract font array offset and convert lowercase to uppercase
  } else {
    mappedCharacter = 96; // unknown character block
  }
  
  for (byte i = 0; i < 5; i++) {
    charBuffer[i] = pgm_read_byte(Font[mappedCharacter]+i);
  }
  
}

// Fetch a character value from a text string in flash
char loadStringChar(byte string, byte character) {
    Serial.print("LSC string:");
        Serial.print(string);
               Serial.print(" char ");
                      Serial.println(character);
//  return (char) pgm_read_byte(currentStringAddress + character);
return 'X';
}

char loadStringChar2(char string[], byte character) {
    Serial.print("LSC string:");
        Serial.print(string);
               Serial.print(" char ");
                      Serial.println(character);
//  return (char) pgm_read_byte(currentStringAddress + character);
return string[character];
}





#define NORMAL 0
#define RAINBOW 1
#define charSpacing 2
// Scroll a text string
void scrollText(byte message, byte style, int fgColor, int bgColor) {
  static byte currentMessageChar = 0;
  static byte currentCharColumn = 0;
  static byte paletteCycle = 0;
  static CRGB currentColor;
  static byte bitBuffer[16] = {0};
  static byte bitBufferPointer = 0;
  static char mymessage[] ="I LOVE YOU"; 

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 35;
    currentMessageChar = 0;
    currentCharColumn = 0;
    selectFlashString(message);
    loadCharBuffer(loadStringChar2(mymessage, currentMessageChar));
    currentPalette = RainbowColors_p;
    for (byte i = 0; i < kMatrixWidth; i++) bitBuffer[i] = 0;
  }

  paletteCycle += 15;

  if (currentCharColumn < 5) { // characters are 5 pixels wide
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = charBuffer[currentCharColumn]; // character
  } else {
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = 0; // space
  }

  CRGB pixelColor;
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < 5; y++) { // characters are 5 pixels tall
      if (bitRead(bitBuffer[(bitBufferPointer + x) % kMatrixWidth], y) == 1) {
        if (style == RAINBOW) {
          pixelColor = ColorFromPalette(currentPalette, paletteCycle+y*16, 255);
        } else {
          pixelColor = CRGB::Red ; // fgColor;
        }
      } else {
        pixelColor = bgColor;
      }
      leds[XY(x, y)] = pixelColor;
    }
  }

  currentCharColumn++;
  if (currentCharColumn > (4 + charSpacing)) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar2(mymessage, currentMessageChar);
    if (nextChar == 0) { // null character at end of strong
      currentMessageChar = 0;
      nextChar = loadStringChar2(mymessage, currentMessageChar);
    }
    loadCharBuffer(nextChar);
  }

  bitBufferPointer++;
  if (bitBufferPointer > 15) bitBufferPointer = 0;

}

void scrollText2(char  mymessage[], byte style, int fgColor, int bgColor) {
  static byte currentMessageChar = 0;
  static byte currentCharColumn = 0;
  static byte paletteCycle = 0;
  static CRGB currentColor;
  static byte bitBuffer[16] = {0};
  static byte bitBufferPointer = 0;
  //static char mymessage[] ="I LOVE YOU"; 

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 40;
    currentMessageChar = 0;
    currentCharColumn = 0;
    //selectFlashString(message);
    loadCharBuffer(loadStringChar2(mymessage, currentMessageChar));
    currentPalette = RainbowColors_p;
    for (byte i = 0; i < kMatrixWidth; i++) bitBuffer[i] = 0;
  }

  paletteCycle += 15;

  if (currentCharColumn < 5) { // characters are 5 pixels wide
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = charBuffer[currentCharColumn]; // character
  } else {
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = 0; // space
  }

  CRGB pixelColor;
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < 5; y++) { // characters are 5 pixels tall
      if (bitRead(bitBuffer[(bitBufferPointer + x) % kMatrixWidth], y) == 1) {
        if (style == RAINBOW) {
          pixelColor = ColorFromPalette(currentPalette, paletteCycle+y*16, 255);
        } else {
          pixelColor = fgColor;
        }
      } else {
        pixelColor = bgColor;
      }
      leds[XY(x, y)] = pixelColor;
    }
  }

  currentCharColumn++;
  if (currentCharColumn > (4 + charSpacing)) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar2(mymessage, currentMessageChar);
    if (nextChar == 0) { // null character at end of strong
      currentMessageChar = 0;
      nextChar = loadStringChar2(mymessage, currentMessageChar);
    }
    loadCharBuffer(nextChar);
  }

  bitBufferPointer++;
  if (bitBufferPointer > 15) bitBufferPointer = 0;

}



void scrollTextZero() {
  //scrollText(0, NORMAL, CRGB::Red, CRGB::Black);
  scrollText2("NERD ", RAINBOW, 0, CRGB::Black);
}

void scrollTextOne() {
  // scrollText(1, RAINBOW, 0, CRGB::Black);
  scrollText2("CAP IBM ", RAINBOW, 0, CRGB::Black);
}

void scrollTextTwo() {
  //scrollText(2, NORMAL, CRGB::Green, CRGB(0,0,8));
  scrollText2(buf, RAINBOW, 0, CRGB::Black);
}

// list of functions that will be displayed
functionList effectList[] = {
    
      scrollTextZero,
                               scrollTextOne,
                                scrollTextTwo,
                                threeSine,
                             threeDee,
                             plasma,
                             confetti,
                             rider,
                             glitter,
                             slantBars,
                             colorFill,
                             sideRain
                            
                            };


const byte numEffects = (sizeof(effectList)/sizeof(effectList[0]));

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client("www.sample.com", 1883, callback);
 **/
MQTT client("realtime.ngi.ibm.com", 1883, callback);

// for QoS2 MQTTPUBREL message.
// this messageid maybe have store list or array structure.

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
    String cmd ="";
    String arg="";

    int pos = message.indexOf(',');
    if (pos  >0) {
        cmd = message.substring(0,pos);
        arg = message.substring(pos+1);
        Serial.println(cmd + ":"+ arg);
    } else {
        cmd = message;
    }
    
    
    if (client.isConnected())
  {
      Serial.println("sending mqtt");
    

        client.publish("/jcglasses/output", "echo:" + message);
}


    Serial.print("Message rcvd:");
    Serial.println(cmd);
    
    if (cmd == "auto"){
        autoCycle  = true;
        Serial.println("auto");

    }

     else if (cmd == "man"){
        autoCycle  = false;
        Serial.println("man");

    } 
     else if (cmd == "next"){
          cycleMillis = currentMillis;
      if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
      effectInit = false; // trigger effect initialization when new effect is selected
     
        Serial.println("next");

    } 
     else if ((cmd == "effect") && (arg !="")){
          cycleMillis = currentMillis;
          int old = currentEffect;
          currentEffect = arg.toInt();
      if (currentEffect >= numEffects) currentEffect = old; // loop to start of effect list
      effectInit = false; // trigger effect initialization when new effect is selected
     
        Serial.println("effect");

    } 
    
     else if ((cmd == "msg") && (arg !="")){
          cycleMillis = currentMillis;
          //arg = "SQUID !";
          for (int i; i< bufl; i++) buf[i]=' ';
          arg.toCharArray(buf,min(arg.length(),bufl));
          buf[arg.length()] = ' ';
           //buf[arg.length()] = '\n';
        
          currentEffect = numEffects - 1;
          scrollText2(buf, RAINBOW, 0, CRGB::Black);
          
    
      effectInit = false; // trigger effect initialization when new effect is selected
     
        Serial.println("msg");

    } 

     else if (cmd == "bright"){
          currentBrightness += 51; // increase the brightness (wraps to lowest)
      FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));


        Serial.println("bright");

    } 
    
      else if (cmd == "on"){
          glassesOn = true;
        Serial.println("bright");

    } 

     else if (cmd == "off"){
          glassesOn = false;
          FastLED.clear();
          FastLED.show();
        Serial.println("bright");

    } 



    delay(200);
}

// Process button inputs and return button activity

#define NUMBUTTONS 2
#define MODEBUTTON 5
#define BRIGHTNESSBUTTON 4

#define BTNIDLE 0
#define BTNDEBOUNCING 1
#define BTNPRESSED 2
#define BTNRELEASED 3
#define BTNLONGPRESS 4
#define BTNLONGPRESSREAD 5

#define BTNDEBOUNCETIME 20
#define BTNLONGPRESSTIME 1000

unsigned long buttonEvents[NUMBUTTONS];
byte buttonStatuses[NUMBUTTONS];
byte buttonmap[NUMBUTTONS] = {BRIGHTNESSBUTTON, MODEBUTTON};

void updateButtons() {
    pinMode(MODEBUTTON,INPUT_PULLUP);
        pinMode(BRIGHTNESSBUTTON,INPUT_PULLUP);
  for (byte i = 0; i < NUMBUTTONS; i++) {
    switch (buttonStatuses[i]) {
      case BTNIDLE:
        if (digitalRead(buttonmap[i]) == LOW) {
          buttonEvents[i] = currentMillis;
          buttonStatuses[i] = BTNDEBOUNCING;
        }
        break;

      case BTNDEBOUNCING:
        if (currentMillis - buttonEvents[i] > BTNDEBOUNCETIME) {
          if (digitalRead(buttonmap[i]) == LOW) {
            buttonStatuses[i] = BTNPRESSED;
          }
        }
        break;

      case BTNPRESSED:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNRELEASED;
        } else if (currentMillis - buttonEvents[i] > BTNLONGPRESSTIME) {
          buttonStatuses[i] = BTNLONGPRESS;
        }
        break;

      case BTNRELEASED:
        break;

      case BTNLONGPRESS:
        break;

      case BTNLONGPRESSREAD:
        if (digitalRead(buttonmap[i]) == HIGH) {
          buttonStatuses[i] = BTNIDLE;
        }
        break;
    }
  }
}

byte buttonStatus(byte buttonNum) {

  byte tempStatus = buttonStatuses[buttonNum];
  if (tempStatus == BTNRELEASED) {
    buttonStatuses[buttonNum] = BTNIDLE;
  } else if (tempStatus == BTNLONGPRESS) {
    buttonStatuses[buttonNum] = BTNLONGPRESSREAD;
  }

  return tempStatus;

}

// Interrupt normal operation to indicate that auto cycle mode has changed
void confirmBlink() {
// Interrupt normal operation to indicate that auto cycle mode has changed

  if (autoCycle) { // one blue blink, auto mode active
    fillAll(CRGB::DarkBlue);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
  } else { // two red blinks, manual mode active
    fillAll(CRGB::DarkRed);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
    fillAll(CRGB::DarkRed);
    FastLED.show();
    FastLED.delay(200);
    fillAll(CRGB::Black);
    FastLED.delay(200);
  }


}

void doButtons() {
  
  // Check the mode button (for switching between effects)
  switch (buttonStatus(0)) {

    case BTNRELEASED: // button was pressed and released quickly
      cycleMillis = currentMillis;
      if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
      effectInit = false; // trigger effect initialization when new effect is selected
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

    case BTNLONGPRESS: // button was held down for a while
      autoCycle = !autoCycle; // toggle auto cycle mode
      confirmBlink(); // one blue blink: auto mode. two red blinks: manual mode.
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

  }

  // Check the brightness adjust button
  switch (buttonStatus(1)) {

    case BTNRELEASED: // button was pressed and released quickly
      currentBrightness += 51; // increase the brightness (wraps to lowest)
      FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

    case BTNLONGPRESS: // button was held down for a while
      currentBrightness = STARTBRIGHTNESS; // reset brightness to startup value
      FastLED.setBrightness(scale8(currentBrightness, MAXBRIGHTNESS));
      eepromMillis = currentMillis;
      eepromOutdated = true;
      break;

  }
  
}

void noCon(void) {
    Particle.disconnect();
    onLine = false; 

}

void doCon(void) {
  myTimer.begin(noCon, 10000, hmSec);
    Particle.connect();
     myTimer.end();
}






// Demo that USES "XY" follows code below
int mqttCounter = 0 ;
void loop()
{
    
      if((onLine && !Particle.connected()) == true) {
      
        doCon();
    }
    
    
    
       if (!onLine && !client.isConnected())
  {
      
      // clientID, username, MD5 encoded password
          // connect to the server
        client.connect("sparkclient");
          Serial.print("connected");


        client.publish("/jcglasses/output", "I'm alive!");
        client.subscribe("/jcglasses/input");
  }
  


      currentMillis = millis(); // save the current timer value
  updateButtons();          // read, debounce, and process the buttons
  doButtons();              // perform actions based on button state
  //checkEEPROM();            // update the EEPROM if necessary

  // switch to a new effect every cycleTime milliseconds
  if (currentMillis - cycleMillis > cycleTime && autoCycle == true) {
    cycleMillis = currentMillis;
    if (++currentEffect >= numEffects) currentEffect = 0; // loop to start of effect list
    effectInit = false; // trigger effect initialization when new effect is selected
  }

  // increment the global hue value every hueTime milliseconds
  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }

  // run the currently selected effect every effectDelay milliseconds
  if (currentMillis - effectMillis > effectDelay) {
    effectMillis = currentMillis;
    effectList[currentEffect](); // run the selected effect function
    random16_add_entropy(1); // make the random values a bit more random-ish
  }

  // run a fade effect too if the confetti effect is running
  if (effectList[currentEffect] == confetti) fadeAll(1);




    if (glassesOn) FastLED.show();
    
    
        if (++mqttCounter % 1000) client.loop();


}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}


void setup() {
    Serial.begin(9600);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
   
}




