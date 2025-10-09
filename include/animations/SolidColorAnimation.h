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
    void loop(CRGB* leds) override {
        for (int i = 0; i < TOTAL_LEDS; i++) {
            leds[i] = color;
        }
    }
    const char* getName() const override { return "Solid"; }
};

#endif // SOLID_COLOR_ANIMATION_H


