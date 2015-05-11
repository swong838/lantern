#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
/* LED info */
const int   NUMPIXELS = 3,
/* chip IO */
            PIXEL_PIN = 0,
/* modes */
            STEADY = 0,
            FLARE = 1,
            GUTTER = 2,
            MIN_PIXEL_BRIGHTNESS = 0,
            MAX_PIXEL_BRIGHTNESS = 255;
  
//
/* time */
const unsigned long
    RUNTIME = 14400000,             // 4 hours
    EXPIRE = millis() + RUNTIME;    // go out at this time

unsigned long
    startTime = 0,                  // start
    currentTime = 0,                // current time, checked vs. prevTime
    prevTime = 0,                   // last clock tick()
    pulseDuration = 0,              // time remaining in current doIlluminate() action
    currentModeDuration = 0;        // time remaining in current mode

/* instantiate Pixel object */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* colors! */
uint32_t    raw_red = pixels.Color(255,0,0),
            hot_yellow = pixels.Color(255,255, 210),
            whitish_yellow = pixels.Color(240, 235, 220),
            dim_blue = pixels.Color(35, 65, 150);

/* operation mode*/
int mode = 0,
    prevMode = 0;
uint8_t targetBrightness = 0;

unsigned long getElapsedTime() {
    return millis() - startTime;
}

void setColor(int mode) {
    int i;
    uint32_t targetColor;
    switch(mode) {
        case FLARE:
            targetColor = raw_red;
            break;
        case GUTTER:
            targetColor = dim_blue;
            break;
        default:
            targetColor = whitish_yellow;
            break;
    }
    for(i=0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, targetColor);
        pixels.show();
    }
}

uint8_t getNewTargetBrightness(int mode) {
    uint8_t newBrightness = 255;
    switch(mode) {
        case FLARE:
            newBrightness = (100 + rand() % 150);     //100-250
            break;
        case GUTTER:
            newBrightness = (15 + rand() % 50);      //15-65
            break;
        default:
            newBrightness = (180 + rand() % 50);     //180-230
            break;
    }
    return newBrightness;
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
    //if (diceRoll < 90) {
    if (diceRoll < 33) {    // [][][]
        newMode = STEADY;
    }
    //else if (diceRoll < 97) {
    else if (diceRoll < 66) {   //[][][]
        newMode = FLARE;
    }
    else {
        newMode = GUTTER;
    }
    return newMode;
}

// figure out how long to stay in this mode
unsigned long getModeDuration(int mode) {
    unsigned long min = 0;
    unsigned long max = 0;
    switch(mode) {
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
    // return (min + rand() % max);
    return 5000; //[][][]
}

int getPulseDuration(int mode) {
    int min = 0;
    int max = 0;
    int pulse = 0;
    switch(mode) {
        case (FLARE || GUTTER):
            pulse = (300 + rand() % 500);
            break;
        default:
            pulse = (1000 + rand() % 1000);
            break;
    }
    return pulse;
}

void doIlluminate() {
    uint8_t currentBrightness = pixels.getBrightness();
    uint8_t newBrightness = 0;

    int delta = ((targetBrightness - currentBrightness) / pulseDuration);
    newBrightness = currentBrightness - delta;
    if (newBrightness < MIN_PIXEL_BRIGHTNESS || newBrightness >= MAX_PIXEL_BRIGHTNESS || newBrightness == currentBrightness) {
        pulseDuration = 0;
        return;
    }
    else {
        pixels.setBrightness(newBrightness);
        pixels.show();
    }
}

bool clocktick() {
    currentTime = millis();
    if (currentTime - prevTime > 1) {
        prevTime = currentTime;
        return true;
    } else {
        return false;
    }
}

void setup() {
    srand(analogRead(0));
#if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
    // init neopixel library
    pixels.begin(); 
    startTime = millis();
    setColor(mode);
}

void loop() {
    if (clocktick()) {
        if (currentModeDuration <= 0) {
            prevMode = mode;
            mode = getNewMode();
            currentModeDuration = getModeDuration(mode);
            pulseDuration = getPulseDuration(mode);
            targetBrightness = getNewTargetBrightness(mode);
            setColor(mode);
        } else if (pulseDuration <= 0) {
            pulseDuration = getPulseDuration(mode);
            targetBrightness = getNewTargetBrightness(mode);
        }
        doIlluminate();
        currentModeDuration--;
        pulseDuration--;
    }

    // check for button input


}
