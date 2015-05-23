#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
/* LED info */
const int       
    NUMPIXELS = 3,
/* chip IO */
    PIXEL_PIN = 1,
/* modes */
    STEADY = 0,
    FLARE = 1,
    GUTTER = 2,
    TICK_RATE = 10,  
    LOW_B = 45,
    LOWISH_B = 75,
    MID_B = 75,
    HIGH_B = 180,
    MAX_B = 225;

/* time */
const long
    RUNTIME = 14400000,             // 4 hours
    EXPIRE = millis() + RUNTIME;    // go out at this time

long
    startTime = 0,                  // start
    prevTime = 0,                   // last clock tick()
    currentModeExpiry = 0;        // time remaining in current mode

/* instantiate Pixel object */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* operation mode*/
int mode = 0,
    pulseDuration = 0,              // time remaining in current doIlluminate() action
    targetBrightness = 0,
    currentBrightness = 0,
    gutterPct = 3,
    flarePct = 7,
    steadyPct = 100 - gutterPct - flarePct;

uint32_t getColorAndBrightnessFromRed(int red) {
    int green, blue;
    red = red % MAX_B;
    switch(mode) {
        case FLARE:
            green = red * .75;
            blue = red * .75;
            break;
        case GUTTER:
            blue = red * .5;
            green = blue * .4;
            red = blue * .45;
            break;
        default:
            green = red * .76;
            blue = red * .61;
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

int getNewKeyBrightness() {
    int newBrightness;
    switch(mode) {
        case FLARE:
            newBrightness = random(MID_B, MAX_B);
            break;
        case GUTTER:
            newBrightness = random(LOW_B, LOWISH_B); 
            break;
        default:
            newBrightness = random(HIGH_B, MAX_B);
            break;
    }
    return newBrightness;
}

void setModeRatios(){
    long remainingLife = RUNTIME - (millis() - startTime);
    if(remainingLife < (RUNTIME/8)) {
        flarePct = 5;
        gutterPct = 25;
        steadyPct = 100 - flarePct - gutterPct;
    }
    else if(remainingLife < (RUNTIME/4)) {
        flarePct = 3;
        gutterPct = 7;
        steadyPct = 100 - flarePct - gutterPct;
    }
    else if (remainingLife < 0) {
        gutterPct = 90;
        flarePct = 0;
        steadyPct = 100 - flarePct - gutterPct;
    }
}

// return a code for a new mode
int getNewMode() {
    int newMode = 0;
    int diceRoll = random(0, 100);
    if (diceRoll < steadyPct) {
        newMode = STEADY;
    }
    else if (diceRoll < flarePct) {
        newMode = FLARE;
    }
    else {
        newMode = GUTTER;
    }
    return newMode;
}

// figure out how long to stay in this mode
long getModeDuration() {
    long duration = 0;
    switch(mode) {
        case FLARE:
            duration = random(500, 2000);
            break;
        case GUTTER:
            duration = random(2000, 5000);
            break;
        default:
            duration = random(18000, 30000);
            break;
    }
    return duration;
}

long getPulseDuration() {
    int pulse;
    switch(mode) {
        case FLARE:
            pulse = random(3, 6);
            break;
        case GUTTER:
            pulse = random(4, 8);
            break;    
        default:
            pulse = random(20, 50);
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
    long currentTime = millis();
    if (currentTime - prevTime > TICK_RATE) {
        prevTime = currentTime;
        if (currentModeExpiry <= currentTime) {
            setModeRatios();
            mode = getNewMode();
            currentModeExpiry = getModeDuration() + currentTime;
            pulseDuration = 0;
        }        
        if (pulseDuration <= 0 || targetBrightness == currentBrightness) {
            pulseDuration = getPulseDuration();
            targetBrightness = getNewKeyBrightness();
        }
        doIlluminate();
        pulseDuration--;
    }
}
