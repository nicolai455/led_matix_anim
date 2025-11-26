#ifndef SOLID_COLOR_ANIMATION_H
#define SOLID_COLOR_ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>
#include "Animation.h"

class SolidColorAnimation : public Animation {
private:
    CRGB color;
public:
    explicit SolidColorAnimation(CRGB c) : color(c) {}
    void setup() override {}
    void renderFrame(CRGB buffer[TOTAL_SIZE][TOTAL_SIZE], uint32_t frameTime) override {
        for (uint8_t y = 0; y < TOTAL_SIZE; y++) {
            for (uint8_t x = 0; x < TOTAL_SIZE; x++) {
                buffer[y][x] = color;
            }
        }
    }
    const char* getName() const override { return "Solid"; }
};

#endif // SOLID_COLOR_ANIMATION_H


