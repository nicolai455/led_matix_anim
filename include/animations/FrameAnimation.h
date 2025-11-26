#ifndef FRAME_ANIMATION_H
#define FRAME_ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>
#include "Animation.h"
#include "frame_io/IFrameSource.h"

class FrameAnimation : public Animation {
private:
    IFrameSource* source;
    uint16_t frameCount;
    uint16_t current;
    uint16_t frameDelayMs;
    unsigned long lastMs;
    CRGB currentFrame[TOTAL_SIZE * TOTAL_SIZE]; // Store current frame data
public:
    FrameAnimation(IFrameSource* src, uint16_t delayMs)
        : source(src), frameCount(0), current(0), frameDelayMs(delayMs), lastMs(0) {
        memset(currentFrame, 0, sizeof(currentFrame));
    }

    void setup() override {
        frameCount = source ? source->getFrameCount() : 0;
        current = 0;
        lastMs = 0;
    }

    void renderFrame(CRGB buffer[TOTAL_SIZE][TOTAL_SIZE], uint32_t frameTime) override {
        if (!source || frameCount == 0) return;

        // Handle frame timing - only advance frame when delay has passed
        if (lastMs == 0 || frameTime - lastMs >= frameDelayMs) {
            source->getFrameInto(current, currentFrame);
            current = (current + 1) % frameCount;
            lastMs = frameTime;
        }

        // Convert stored 1D frame data to 2D buffer (row-major order)
        for (uint8_t y = 0; y < TOTAL_SIZE; y++) {
            for (uint8_t x = 0; x < TOTAL_SIZE; x++) {
                uint16_t index = y * TOTAL_SIZE + x;
                buffer[y][x] = currentFrame[index];
            }
        }
    }

    const char* getName() const override { return "Frames"; }
};

#endif // FRAME_ANIMATION_H


