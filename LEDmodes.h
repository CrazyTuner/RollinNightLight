#define NUMPIXELS 46
#include <Adafruit_NeoPixel.h>
#define LEDPIN 12
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);



void clearStrip() {
  for( int i = 0; i<NUMPIXELS; i++){
    pixels.setPixelColor(i, 0x000000); pixels.show();
  } 
}
void LEDModes() {
  Serial.print("voidLEDModes");
  Serial.println(data.LEDState);
//########################################--------########################################
 if (data.LEDState == 1)  {
  Serial.println("LEDSTATE 1");
  for( int i = 0; i<NUMPIXELS-1; i++){
    pixels.setPixelColor(i, pixels.Color(0,50,0)); // Pixel leuchtet in der Farbe Grün
    pixels.setPixelColor(i-4, pixels.Color(0,0,0)); // Der vorherige Pixel wird abgeschaltet
    pixels.show(); // Durchführen der Pixel-Ansteuerung

    if (i==0) pixels.setPixelColor(NUMPIXELS-3, pixels.Color(0,0,0)); // Im Fall von Pixel "0" muss die vorherige (39) ausgeschaltet werden.
    if (i==0) pixels.setPixelColor(NUMPIXELS-2, pixels.Color(0,0,0)); // Im Fall von Pixel "0" muss die vorherige (39) ausgeschaltet werden.
    if (i==1) pixels.setPixelColor(NUMPIXELS-1, pixels.Color(0,0,0)); // Im Fall von Pixel "0" muss die vorherige (39) ausgeschaltet werden.
    pixels.show(); // Durchführen der Pixel-Ansteuerung
    delay (30);
  }
  clearStrip();
 }
//########################################--------########################################
 else if (data.LEDState == 2) {
  Serial.println("LEDState 2");
  for( int i = 0; i<NUMPIXELS-1; i++) {
  pixels.setPixelColor(i, pixels.Color(0,250,0));
  pixels.show();
  delay(50);
  }
  clearStrip();
 }
//########################################--------########################################
 else if (data.LEDState == 3) {
  Serial.println("LEDState 3");
  for( int i = 0; i<NUMPIXELS-1; i++) {
  pixels.setPixelColor(i, pixels.Color(25,100,43));
  pixels.show();
  delay(50);
  }
  clearStrip();
 }
//########################################--------########################################
 else if (data.LEDState == 4) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(0,255,0));
  }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 5) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(255,255,0));
 }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 6) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(0,255,255));
 }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 7) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,255));
 }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 8) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(255,0,0));
 }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 9) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(255,0,255));
 }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 10) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(255,255,0));
 }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 11) {
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(255,255,0));
 }
  pixels.show();
 }
//########################################--------########################################
 else if (data.LEDState == 12) {
  for (int j=0; j<4; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 4; q++) {
      for (int i=0; i < NUMPIXELS; i=i+4) {
        pixels.setPixelColor(i+q, pixels.Color(0,250,0));
      }
      pixels.show();
     
      delay(50);
     
      for (int i=0; i < NUMPIXELS; i=i+4) {
        pixels.setPixelColor(i+q, pixels.Color(0,0,0));
      }
    }
  }
 }
//########################################--------########################################
 else if (data.LEDState == 13) {
  for (int j=0; j<4; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 4; q++) {
      for (int i=0; i < NUMPIXELS; i=i+4) {
        pixels.setPixelColor(i+q, pixels.Color(255,0,255));
      }
      pixels.show();
     
      delay(50);
     
      for (int i=0; i < NUMPIXELS; i=i+4) {
        pixels.setPixelColor(i+q, pixels.Color(0,0,0));
      }
    }
  }
 }
//########################################--------########################################
 else if (data.LEDState == 14) {
  int Pixel = random(NUMPIXELS);
  pixels.setPixelColor(random(NUMPIXELS), pixels.Color(0,255,200));
  pixels.show();
  delay(10);
  clearStrip();
 }
//########################################--------########################################
 else if (data.LEDState == 15) {
  clearStrip();
  pixels.setPixelColor(random(NUMPIXELS), pixels.Color(255,0,200));
  pixels.show();
  delay(100);
 }
//########################################--------########################################
 else if (data.LEDState == 16) {
  for(long firstPixelHue = 0; firstPixelHue < 1*65536; firstPixelHue += 256) {
    for(int i=0; i<pixels.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show();
    delay(5);
  }
 }
//########################################--------########################################
 else if (data.LEDState == 17) {
  for( int i = 0; i<18; i++) {
    pixels.setPixelColor(i, pixels.Color(255,0,255));
    pixels.setPixelColor(i+1, pixels.Color(255,0,255));
    pixels.setPixelColor(i+19, pixels.Color(255,0,255));
    pixels.setPixelColor(i+20, pixels.Color(255,0,255));
    if(i>=2)  {
    pixels.setPixelColor(i-2, pixels.Color(0,0,0));
    pixels.setPixelColor(i+17, pixels.Color(0,0,0));
    }
    pixels.show();
    delay(10);
    
    }
    clearStrip();
 }
//########################################--------########################################
 else if (data.LEDState == 18) {
  int brightness = 128;
  //  Uncomment one of these RGB (Red, Green, Blue) values to
  //  set the base color of the flame.  The color will flickr
  //  based on the initial base color
  
  //  Regular (orange) flame:
    int r = 226, g = 121, b = 35;

  //  Purple flame:
  //  int r = 158, g = 8, b = 148;

  //  Green flame:
  //int r = 74, g = 150, b = 12;

  //  Flicker, based on our initial RGB values
  for(int i=0; i<pixels.numPixels(); i++) {
    int flicker = random(0,55);
    int r1 = r-flicker;
    int g1 = g-flicker;
    int b1 = b-flicker;
    if(g1<0) g1=0;
    if(r1<0) r1=0;
    if(b1<0) b1=0;
    pixels.setPixelColor(i,r1,g1, b1);
  }
  pixels.show();
 
  //  Adjust the delay here, if you'd like.  Right now, it randomizes the 
  //  color switch delay to give a sense of realism
 }
//########################################--------########################################




//########################################--------########################################
  else if (data.LEDState == 99)  {
    delay (30);
    clearStrip();
  }
//########################################--------########################################
 else {
  Serial.println(data.LEDState);
  for( int i = 12; i<14; i++) {
  pixels.setPixelColor(i, pixels.Color(0,0,50));
  pixels.setPixelColor(i+19, pixels.Color(0,0,50));
  pixels.show();
  delay(50);
  }
  clearStrip();
 }  
}
