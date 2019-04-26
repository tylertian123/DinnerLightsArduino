#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define LED_COUNT 300

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB | NEO_KHZ800);

public class Color {
public:
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    uint8_t r, g, b;

    operator uint32_t() const {
        return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
    }
}

void setup() {

}

void loop() {

}
