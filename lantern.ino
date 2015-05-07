#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
/* LED info */
const int   NUMPIXELS = 2,
/* chip IO */
            PIXEL_PIN = 1,
/* modes */
            STEADY = 0,
            FLARE = 1,
            GUTTER = 2,
            MAX_BRIGHTNESS_DELTA = 4;

/* time */
const long RUNTIME = 14400000;    // 4 hours
unsigned long startTime;

/* instantiate Pixel object */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint32_t whitish_yellow = pixels.Color(250, 230, 210);

long getElapsedTime() {
    return millis() - startTime;
}

void setColor(uint32_t targetColor) {
    for(int i=0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, targetColor);
        pixels.show();
    }
}

void updateBrightness(int brightness) {
    pixels.setBrightness(brightness);
    pixels.show();
}

int getNewTargetBrightness(int mode) {
    switch(mode){
        case FLARE:
            return (150 + rand() % 90);     //150-240
            break;
        case GUTTER:
            return (30 + rand() % 50);      //30-80
            break;
        default:
            return (180 + rand() % 20);     //180-200
            break;
    }
}


// return a code for a new mode
// 0 = steady
// 1 = flare
// 2 = gutter
int getNewMode() {
    int newMode = 0;
    int diceRoll = rand() % 100;
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
            min = 180000;
            max = 300000;
            break;
    }
    return (min + rand() % max);
}

void setup() {
    srand(analogRead(0));
#if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
    // init neopixel library
    pixels.begin(); 
    startTime = millis();
    setColor(whitish_yellow);
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
