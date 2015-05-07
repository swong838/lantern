//#include <Adafruit_NeoPixel.h>

/* LED info */
#define NUMPIXELS   2
#define RED         200
#define GREEN       145
#define BLUE        0
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* chip IO */
#define PIXEL_PIN   1
#define BUTTON_PIN  2

/* time */
#define RUNTIME     14400000    // 4 hours
unsigned long       startTime;

/* modes */

#define STEADY      0
#define FLARE       1
#define GUTTER      2

void setup() {
    randomSeed(analogRead(0));

    // trinket setup
    #if defined (__AVR_ATtiny85__)
      if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif

    // init neopixel library
    //pixels.begin(); 
    startTime = millis();

    
}

long getElapsedTime() {
    return millis() - startTime;
}

void updatePixels() {
    for(int i=0; i < NUMPIXELS; i++) {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        //pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
        //pixels.show();
    }
}

void getTargetBrightness(int mode) {

}


// return a code for a new mode
// 0 = steady
// 1 = flare
// 2 = gutter
int getNewMode() {
    int newMode = 0;
    long diceRoll = random(100);
    //long age = getElapsedTime();
    // age affects the frequency of gutter mode
    if(diceRoll < 90) {
        newMode = STEADY;
    }
    else if(diceRoll < 97) {
        newMode = FLARE;
    }
    else {
        newMode = GUTTER;
    }
    return newMode;
}

// figure out how long to stay in this mode
long getModeDuration(int mode) {
    long min = 0;
    long max = 0;
    switch(mode){
        case FLARE:
            min = 3000;
            max = 6000;
            break;
        case GUTTER:
            min = 2000;
            max = 20000;
            break;
        default:
            min = 10000;
            max = 60000;
            break;
    }
    return random(min, max);
}


void loop() {
    //static int ageCheck = 0;
    static int mode = 0;
    static long currentModeDuration = random(3000, 300000);
    static int currentActionDuration = 500;

    if(millis() - startTime < currentModeDuration) {
        // run fadeTo w/ current mode
    }
    else {
        mode = getNewMode();
        currentModeDuration = getModeDuration(mode);
    }



    // check for button input
    // check elapsed time




    //ageCheck = (ageCheck++ % 100);
}
