#ifndef RAINBOW_ANIMATION_H
#define RAINBOW_ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>
#include "Animation.h"

class RainbowAnimation : public Animation {
private:
    uint8_t hueOffset;
    uint8_t timeOffset;
public:
    RainbowAnimation() : hueOffset(0), timeOffset(0) {}
    void setup() override {
        hueOffset = 0;
        timeOffset = 0;
    }
    void renderFrame(CRGB buffer[TOTAL_SIZE][TOTAL_SIZE], uint32_t frameTime) override {
        // Smooth animated rainbow
        unsigned long time = frameTime / 20; // Faster animation (was /50)

        for (uint8_t y = 0; y < TOTAL_SIZE; y++) {
            for (uint8_t x = 0; x < TOTAL_SIZE; x++) {
                // Create smooth flowing rainbow
                uint8_t hue = (x * 4 + y * 2 + time) & 0xFF;

                // Simple brightness - no complex calculations that could cause black bars
                uint8_t brightness = 255;

                buffer[y][x] = CHSV(hue, 255, brightness);
            }
        }
    }
    const char* getName() const override { return "Rainbow"; }
};

#endif // RAINBOW_ANIMATION_H


