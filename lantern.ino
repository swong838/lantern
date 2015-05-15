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
    MID_B = 75,
    HIGH_B = 120,
    MAX_B = 180;

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
uint32_t    raw_red = pixels.Color(MAX_B, 0, 0),
            hot_yellow = pixels.Color(MAX_B, MAX_B, HIGH_B),
            whitish_yellow = pixels.Color(MAX_B, HIGH_B, MID_B),
            dim_blue = pixels.Color(LOW_B, MID_B, HIGH_B);

/* operation mode*/
int mode = 0,
    prevMode = 0;

unsigned long getElapsedTime() {
    return millis() - startTime;
}

void setColor(int mode) {
    int i;
    uint32_t targetColor;
    switch(mode) {
        case FLARE:
            targetColor = hot_yellow;
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
    int newBrightness;
    switch(mode) {
        case FLARE:
            newBrightness = random(MID_B, MAX_B);
            break;
        case GUTTER:
            newBrightness = random(0, LOW_B);      //15-65
            break;
        default:
            newBrightness = random(HIGH_B, MAX_B);     //180-230
            break;
    }
    //newBrightness = 100;
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
    //return newMode;
    return STEADY;//[][][]
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
    duration = 1000;    //[][][]
    return duration;
}

unsigned long getPulseDuration(int mode) {
    int min = 0;
    int max = 0;
    unsigned long pulse = 0;
    switch(mode) {
        case (FLARE || GUTTER):
            pulse = random(125, 150);
            break;
        default:
            pulse = random(200, 300);
            break;
    }

    pulse = 200;//[][][]
    return pulse;
}

void doIlluminate(uint8_t targetBrightness, uint8_t currentBrightness) {
    uint8_t newBrightness;
    uint8_t delta = ((currentBrightness - targetBrightness) / pulseDuration);
    newBrightness = currentBrightness - delta;

    if (newBrightness <= 0 || newBrightness >= MAX_B || newBrightness == currentBrightness) {
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
    if (currentTime - prevTime > TICK_RATE) {
        prevTime = currentTime;
        return true;
    }
    else {
        return false;
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
    setColor(mode);
}

void loop() {
    uint8_t currentBrightness,
            targetBrightness;
    if (clocktick()) {
        currentBrightness = pixels.getBrightness();
        if (currentModeDuration <= 0) {
            prevMode = mode;
            mode = getNewMode();
            currentModeDuration = getModeDuration(mode);
            pulseDuration = 0;
            setColor(mode);
        }
        if (pulseDuration <= 0) {
            pulseDuration = getPulseDuration(mode);
            targetBrightness = getNewTargetBrightness(mode);
        }
        if(targetBrightness != currentBrightness) {
            doIlluminate(targetBrightness, currentBrightness);
        }
        currentModeDuration--;
        pulseDuration--;
    }

    // check for button input


}
