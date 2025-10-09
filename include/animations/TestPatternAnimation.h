#ifndef TEST_PATTERN_ANIMATION_H
#define TEST_PATTERN_ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>
#include "Animation.h"
#include "MatrixOrientation.h"

class TestPatternAnimation : public Animation {
private:
    MatrixOrientation* matrix;
public:
    explicit TestPatternAnimation(MatrixOrientation* m) : matrix(m) {}
    void setup() override {}
    void loop(CRGB* leds) override { matrix->drawPanelTestPattern(leds); }
    const char* getName() const override { return "TestPattern"; }
};

#endif // TEST_PATTERN_ANIMATION_H


