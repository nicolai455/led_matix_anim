#ifndef IFRAME_SOURCE_H
#define IFRAME_SOURCE_H

#include <Arduino.h>
#include <FastLED.h>

class IFrameSource {
public:
    virtual ~IFrameSource() {}
    virtual uint16_t getFrameCount() const = 0;
    virtual void getFrameInto(uint16_t frameIndex, CRGB* ledsOut) = 0; // fills 32x32 into ledsOut in matrix order
};

#endif // IFRAME_SOURCE_H


