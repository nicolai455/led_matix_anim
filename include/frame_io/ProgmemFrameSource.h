#ifndef PROGMEM_FRAME_SOURCE_H
#define PROGMEM_FRAME_SOURCE_H

#include <Arduino.h>
#include <FastLED.h>
#include "IFrameSource.h"

class ProgmemFrameSource : public IFrameSource {
private:
    const CRGB* framesProgmem; // contiguous frames, each 1024 CRGB
    uint16_t frameCount;
public:
    ProgmemFrameSource(const CRGB* frames, uint16_t count)
        : framesProgmem(frames), frameCount(count) {}

    uint16_t getFrameCount() const override { return frameCount; }

    void getFrameInto(uint16_t frameIndex, CRGB* ledsOut) override {
        if (frameIndex >= frameCount) frameIndex = 0;
        const CRGB* src = framesProgmem + (frameIndex * 1024);
        // Copy from flash (PROGMEM) to RAM
        memcpy_P(ledsOut, src, 1024 * sizeof(CRGB));
    }
};

#endif // PROGMEM_FRAME_SOURCE_H


