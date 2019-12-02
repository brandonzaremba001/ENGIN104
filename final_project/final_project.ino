#include "LPD8806.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

//##############################################################

// Number of RGB LEDs in strand:
int nLEDs = 32;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// Global Variables
int level = 0;
bool mode = true;
int rpm = 100; //NOT acctual RPM
int rando = 8;
bool endlevel = false;
bool winner = true;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

void setup() {
  // put your setup code here, to run once:
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  // Start up the LED strip
  strip.begin();

  // Button Setup
  pinMode(4,INPUT);
  digitalWrite(4,HIGH);

  // level LEDs
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);

  // Update the strip, to start they are all 'off'
  strip.show();

  // Start up pattern
  intro();
}

void loop() {
  // Reset indicator lights
  for(int j = 8;j <= 12; j++) {
    digitalWrite(j,LOW);}

  // Set winner parameter
  winner = true;

  //  Run level loop
  for(level = 0; level < 5; level++) {

    // Generate Random "target" location
    rando = random(1,nLEDs - 1);
    
    // set break condition
    endlevel = false;
    while(endlevel == false) {
    
      //Set level variables
      if(level == 0) {        // Level 1
        digitalWrite(8,HIGH);
        mode = true;
        rpm = 75;}
      if(level == 1) {        // Level 2
        digitalWrite(9,HIGH);
        mode = true;
        rpm = 50;}
      if(level == 2) {        // Level 3
        digitalWrite(10,HIGH);
        mode = false;
        rpm = 75;}
      if(level == 3) {        // Level 4
        digitalWrite(11,HIGH);
        mode = false;
        rpm = 75;}
      if(level == 4) {        // Winning Graphics
        digitalWrite(12,HIGH); 
        rainbow(3);
        rainbowCycle(0);  // make it go through the cycle fairly fast
        break;}

      // Play level
      game(strip.Color(127, 127, 250), rpm, rando);}

    // Loser Condition
    if(winner == false) {
      break;}
  }
  //  This point reached with winning game
  //  or satisfying the loser condition
  //  Resets game to level 0
}


//            MAIN BODY OF GAME CODE

void game(uint32_t c, uint8_t wait,int rand) {
  int i;
  int randp = rand + 1;
  int randm = rand - 1;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);


  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states

    // Set game LED
    strip.setPixelColor(rand,strip.Color(127,   0,   0));
    // Set game Mode LEDs
    if(mode == true) {
      strip.setPixelColor(randm,strip.Color(127,   20,   0));
      strip.setPixelColor(randp,strip.Color(127,   20,   0));
    }

    strip.show();               // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!

    // Button Interupt
    if(digitalRead(4) == LOW) {
      if(mode == true && i>= randm && i<= randp) {
        win();
        break;}
      if(mode == false && i == rand) {
        win();
        break;}
      else {
        lose();
        break;}
        
    }
    delay(wait);
  }
  strip.show(); // Refresh to turn off last pixel
}

//                  Intro graphic
void intro() {
  colorWipe(strip.Color(  0, 127,   0), 50);
}

//                  Winning graphic
void win() {
  theaterChase(strip.Color(  0, 127,   0), 50); // Green
  endlevel = true; 
}

//                  Loosing graphic
void lose() {
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  winner = false;
  endlevel = true;
}


/*    THE FOLLOWING CODE WAS TAKEN FROM THE ADAFRUIT 
 *    LIBRRARY FOR THE LPD8806 LED LIGHT STRAND:
 *    https://github.com/adafruit/LPD8806
 */  

//                        Theatre-style crawling lights.

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
//                Fill the dots progressively along the strip.

void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

//                Rainbow pattern for winning pattern

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}
// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}
uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}
