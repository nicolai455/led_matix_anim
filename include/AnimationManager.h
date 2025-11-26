#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <Arduino.h>
#include <FastLED.h>
#include "Animation.h"
#include "MatrixOrientation.h"

#define MAX_ANIMATIONS 16

class AnimationManager {
private:
    Animation* animations[MAX_ANIMATIONS];
    uint8_t animationCount;
    int8_t currentIndex;
    unsigned long lastSwitchMs;
    uint32_t autoCycleMs; // 0 = disabled

    // Frame buffer for 2D coordinate rendering
    CRGB frameBuffer[TOTAL_SIZE][TOTAL_SIZE];

    // Matrix orientation for coordinate transformation
    MatrixOrientation* matrix;

public:
    AnimationManager(MatrixOrientation* matrixPtr);

    void setAutoCycle(uint32_t intervalMs); // 0 disables

    bool registerAnimation(Animation* animation);
    uint8_t getCount() const;

    bool switchTo(uint8_t index);
    bool switchToByName(const char* name);
    int8_t getCurrentIndex() const;
    const char* getCurrentName() const;

    void setup();
    void loop(CRGB* leds);
};

#endif // ANIMATION_MANAGER_H


