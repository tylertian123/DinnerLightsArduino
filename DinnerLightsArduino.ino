#include <Adafruit_NeoPixel.h>

constexpr int LED_PIN = 6;
constexpr int LED_COUNT = 78;

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB | NEO_KHZ800);

class Color {
public:
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    uint8_t r, g, b;

    Color(const uint32_t& f) {
        r = (f & 0x00FF0000) >> 16;
        g = (f & 0x0000FF00) >> 8;
        b = (f & 0x000000FF);
    }

    operator uint32_t() const {
        return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
    }

    Color applyBrightness(uint8_t percentage) const {
        return Color(
            r * percentage / 100,
            g * percentage / 100,
            b * percentage / 100
        );
    }
    Color applyBrightness255(uint8_t brightness) const {
        return Color(
            r * brightness / 255,
            g * brightness / 255,
            b * brightness / 255
        );
    }
};

uint16_t time = 0;
uint8_t brightness = 80;

Color generator0(uint16_t time, uint16_t led) {
    time += led * 0x400 % 0x10000;

    if(time < 0x6000) {
        return Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(0xA000, time / 0x60, 0xA0));
    }
    else if(time < 0x8000) {
        return Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(0xA000, 0xFF, 0xA0));
    }
    else if(time < 0xE000) {
        return Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(0xA000, (0xDFFF - time) / 0x60, 0xA0));
    }
    else {
        return Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::ColorHSV(0xA000, 0x00, 0xA0));
    }
}

constexpr int TIME_PER_LED = (0x7C00 / (LED_COUNT / 2));
Color generator1(uint16_t time, uint16_t led) {
    uint16_t timeWhenOn;
    if(led < LED_COUNT / 2) {
        timeWhenOn = led * TIME_PER_LED;
    }
    else {
        timeWhenOn = (LED_COUNT - led) * TIME_PER_LED;
    }
    if(time < 0x8000) {
        if(time >= timeWhenOn) {
            return Color(171, 75, 152);
        }
        else {
            return Color(0, 0, 0);
        }
    }
    else {
        if(0xFFFF - time >= timeWhenOn) {
            return Color(171, 75, 152);
        }
        else {
            return Color(0, 0, 0);
        }
    }
}

Color generatorOff(uint16_t time, uint16_t led) {
    return Color(0, 0, 0);
}

Color (*const generators[])(uint16_t, uint16_t) = {
    &generator0,
    &generator1,
    &generatorOff,
};
const uint16_t timeIncrements[] = {
    0x100,
    0x80,
    0x00,
};

constexpr size_t MODE_COUNT = sizeof(generators) / sizeof(generators[0]);
size_t mode = 0;

unsigned long last;
constexpr int LOOP_RATE = 50;
constexpr unsigned long LOOP_DELAY = (1000 / LOOP_RATE);

constexpr int MODE_BUTTON_PIN = 5;
constexpr int BRIGHTNESS_UP_BUTTON_PIN = 4;
constexpr int BRIGHTNESS_DOWN_BUTTON_PIN = 3;

void setup() {
    Serial.begin(9600);
    leds.begin();
    leds.clear();
    leds.show();

    pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BRIGHTNESS_UP_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BRIGHTNESS_DOWN_BUTTON_PIN, INPUT_PULLUP);
    last = millis();
}

void loop() {
    // Poll buttons
    if(!digitalRead(MODE_BUTTON_PIN)) {
        time = 0;
        mode ++;
        if(mode >= MODE_COUNT) {
            mode = 0;
        }
        
        delay(10);
        while(!digitalRead(MODE_BUTTON_PIN));
    }
    if(!digitalRead(BRIGHTNESS_UP_BUTTON_PIN)) {
        brightness += 10;
        if(brightness > 100) {
            brightness -= 100;
        }
        
        delay(10);
        while(!digitalRead(BRIGHTNESS_UP_BUTTON_PIN));
    }
    if(!digitalRead(BRIGHTNESS_DOWN_BUTTON_PIN)) {
        if(brightness >= 10) {
            brightness -= 10;
        }
        else {
            brightness += 90; // +100 to loop over and -10
        }
        
        delay(10);
        while(!digitalRead(BRIGHTNESS_DOWN_BUTTON_PIN));
    }
    // Generate colors
    for(uint16_t i = 0; i < LED_COUNT; i ++) {
        leds.setPixelColor(i, static_cast<uint32_t>(generators[mode](time, i).applyBrightness(brightness)));
    }
    time += timeIncrements[mode];
    leds.show();
    // Maintain constant refresh rate if possible
    if(millis() < last + LOOP_DELAY) {
        delay(last + LOOP_DELAY - millis());
    }
    last = millis();
}
