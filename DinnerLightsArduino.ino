#include <Adafruit_NeoPixel.h>

constexpr int LED_PIN = 6;
constexpr int LED_COUNT = 300;

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB | NEO_KHZ800);

class Color {
public:
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    uint8_t r, g, b;

    operator uint32_t() const {
        return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
    }
};

uint16_t time = 0;

Color generator0(uint16_t time, uint16_t led) {
    return Color(0, 0, 0);
}

Color (*const generators[])(uint16_t, uint16_t) = {
    &generator0,
};

constexpr size_t MODE_COUNT = sizeof(generators) / sizeof(generators[0]);
size_t mode = 0;

unsigned long last;
constexpr int LOOP_RATE = 50;
constexpr unsigned long LOOP_DELAY = (1000 / LOOP_RATE);

void setup() {
    leds.begin();
    leds.clear();
    leds.show();
    last = millis();
}

void loop() {
    for(uint16_t i = 0; i < LED_COUNT; i ++) {
        leds.setPixelColor(i, static_cast<uint32_t>(generators[mode](time, i)));
    }
    time += 0x80;
    leds.show();

    if(millis() < last + LOOP_DELAY) {
        delay(last + LOOP_DELAY - millis());
    }
    last = millis();
}
