#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>

// Matrix dimensions for frame buffer interface
#define TOTAL_SIZE 32

class Animation {
public:
    virtual ~Animation() {}

    // One-time setup to initialize internal state
    virtual void setup() = 0;

    // Render one frame into the 2D buffer. Animation works in logical coordinates.
    // MatrixOrientation handles the transformation to physical LED indices.
    virtual void renderFrame(CRGB buffer[TOTAL_SIZE][TOTAL_SIZE], uint32_t frameTime) = 0;

    // Unique, human-readable name for selection and diagnostics
    virtual const char* getName() const = 0;
};

#endif // ANIMATION_H


