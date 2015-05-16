#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
/* LED info */
const int       
    NUMPIXELS = 3,
/* chip IO */
    PIXEL_PIN = 0,
/* modes */
    STEADY = 0,
    FLARE = 1,
    GUTTER = 2,
    TICK_RATE = 10;

const int  
    LOW_B = 25,
    LOWISH_B = 40,
    MID_B = 75,
    HIGH_B = 120,
    MAX_B = 180;

/* time */
const unsigned long
            RUNTIME = 14400000,             // 4 hours
            EXPIRE = millis() + RUNTIME;    // go out at this time

unsigned long
    startTime = 0,                  // start
    prevTime = 0,                   // last clock tick()
    currentModeExpiry = 0;        // time remaining in current mode

/* instantiate Pixel object */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* colors! */

uint32_t    raw_red = pixels.Color(MAX_B, 0, 0),
            hot_yellow = pixels.Color(MAX_B, MAX_B, HIGH_B),
            whitish_yellow = pixels.Color(MAX_B, HIGH_B, MID_B),
            dim_blue = pixels.Color(LOW_B, MID_B, HIGH_B);


/* operation mode*/
int mode = 0,
    prevMode = 0,
    pulseDuration = 0,              // time remaining in current doIlluminate() action
    targetBrightness = 0,
    currentBrightness = 0;

unsigned long getElapsedTime() {
    return millis() - startTime;
}

uint32_t getColorAndBrightnessFromRed(int red) {
    int green, blue;
    red = red % MAX_B;
    switch(mode) {
        case FLARE:
            green = red * .75;
            blue = red * .75;
            break;
        case GUTTER:
            blue = red % LOW_B;
            green = (blue * .6);
            red = (blue * .5);
            break;
        default:
            green = red * .66;
            blue = red * .41;
            break;
    }
    return pixels.Color(red, green, blue);
}

void applyLighting(uint32_t targetColor) {
    int i;
    for(i=0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, targetColor);
    }
    pixels.show();
}

int getNewKeyBrightness(int mode) {
    int newBrightness;
    switch(mode) {
        case FLARE:
            newBrightness = random(MID_B, MAX_B);
            break;
        case GUTTER:
            newBrightness = random(LOW_B, LOWISH_B);      
            break;
        default:
            newBrightness = random(MID_B, MAX_B);
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
    int diceRoll = random(0, 100);
    //long age = getElapsedTime();
    // age affects the frequency of gutter mode
    if (diceRoll < 90) {
        newMode = STEADY;
    }
    else if (diceRoll < 97) {
        newMode = FLARE;
    }
    else {
        newMode = GUTTER;
    }
    return newMode;
}

// figure out how long to stay in this mode
unsigned long getModeDuration(int mode) {
    unsigned long duration = 0;
    switch(mode) {
        case FLARE:
            duration = random(3000, 6000);
            break;
        case GUTTER:
            duration = random(2000, 5000);
            break;
        default:
            duration = random(180000, 300000);
            break;
    }
    duration = 10000;    //[][][]
    return duration;
}

unsigned long getPulseDuration(int mode) {
    int pulse;
    switch(mode) {
        case (FLARE || GUTTER):
            pulse = random(2, 7);
            break;
        default:
            pulse = random(15, 25);
            break;
    }
    return pulse;
}

void doIlluminate() {
    int delta = ((currentBrightness - targetBrightness) / pulseDuration) + 1,
        newRed = (currentBrightness - delta) % MAX_B;
    if (newRed <= 0 || newRed >= MAX_B || newRed == currentBrightness) {
        pulseDuration = 0;
        return;
    }
    else {
        currentBrightness = newRed;
        applyLighting(getColorAndBrightnessFromRed(newRed));
    }
}

void setup() {
    randomSeed(analogRead(0));
#if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
    // init neopixel library
    pixels.begin(); 
    startTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    if (currentTime - prevTime > TICK_RATE) {
        prevTime = currentTime;
        if (currentModeExpiry <= currentTime) {
            prevMode = mode;
            mode = getNewMode();
            currentModeExpiry = getModeDuration(mode);
            pulseDuration = 0;
        }
        
        if (pulseDuration <= 0) {
            pulseDuration = getPulseDuration(mode);
            targetBrightness = getNewKeyBrightness(mode);
        }
        if(targetBrightness != currentBrightness) {
            doIlluminate();
        }
        
        pulseDuration--;
    }
}
