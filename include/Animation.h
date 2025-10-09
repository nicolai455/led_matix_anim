#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>

class Animation {
public:
    virtual ~Animation() {}

    // One-time setup to initialize internal state
    virtual void setup() = 0;

    // Render one frame into leds. Caller is responsible for FastLED.show().
    virtual void loop(CRGB* leds) = 0;

    // Unique, human-readable name for selection and diagnostics
    virtual const char* getName() const = 0;
};

#endif // ANIMATION_H


