#include <Adafruit_NeoPixel.h>

/* LED info */
#define NUMPIXELS   2
#define RED         200
#define GREEN       145
#define BLUE        0
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* chip IO */
#define PIXEL_PIN   1
#define BUTTON_PIN  2

/* time */
#define RUNTIME     14400000
unsigned long       startTime;


void setup() {
    // trinket setup
    #if defined (__AVR_ATtiny85__)
      if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif

    // init neopixel library
    pixels.begin(); 
    startTime = millis();
}

void updatePixels() {
    for(int i=0;i<NUMPIXELS;i++){
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
        pixels.show();
    }

}

void getBaseLuminance(
}


void loop() {

}