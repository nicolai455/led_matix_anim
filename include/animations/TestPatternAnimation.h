#ifndef TEST_PATTERN_ANIMATION_H
#define TEST_PATTERN_ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>
#include "Animation.h"

class TestPatternAnimation : public Animation {
public:
    TestPatternAnimation() {}
    void setup() override {}

    void renderFrame(CRGB buffer[TOTAL_SIZE][TOTAL_SIZE], uint32_t frameTime) override {
        // Clear buffer
        for (uint8_t y = 0; y < TOTAL_SIZE; y++) {
            for (uint8_t x = 0; x < TOTAL_SIZE; x++) {
                buffer[y][x] = CRGB::Black;
            }
        }

        // Fill each quadrant with panel colors (2x2 layout)
        // Top-left: Red
        for (uint8_t y = 0; y < 16; y++) {
            for (uint8_t x = 0; x < 16; x++) {
                buffer[y][x] = CRGB::Red;
            }
        }
        // Top-right: Green
        for (uint8_t y = 0; y < 16; y++) {
            for (uint8_t x = 16; x < 32; x++) {
                buffer[y][x] = CRGB::Green;
            }
        }
        // Bottom-left: Blue
        for (uint8_t y = 16; y < 32; y++) {
            for (uint8_t x = 0; x < 16; x++) {
                buffer[y][x] = CRGB::Blue;
            }
        }
        // Bottom-right: Yellow
        for (uint8_t y = 16; y < 32; y++) {
            for (uint8_t x = 16; x < 32; x++) {
                buffer[y][x] = CRGB::Yellow;
            }
        }

        // Draw corner arrows (white) - simplified version
        // Top-left corner arrows
        for (uint8_t i = 0; i <= 5; i++) {
            buffer[0][i] = CRGB::White;  // right arrow
            buffer[i][0] = CRGB::White;  // down arrow
            if (i > 0) buffer[i][i] = CRGB::White;  // diagonal
        }

        // Top-right corner arrows
        for (uint8_t i = 0; i <= 5; i++) {
            buffer[0][31-i] = CRGB::White;  // left arrow
            buffer[i][31] = CRGB::White;    // down arrow
            if (i > 0) buffer[i][31-i] = CRGB::White;  // diagonal
        }

        // Bottom-left corner arrows
        for (uint8_t i = 0; i <= 5; i++) {
            buffer[31][i] = CRGB::White;    // right arrow
            buffer[31-i][0] = CRGB::White;  // up arrow
            if (i > 0) buffer[31-i][i] = CRGB::White;  // diagonal
        }

        // Bottom-right corner arrows
        for (uint8_t i = 0; i <= 5; i++) {
            buffer[31][31-i] = CRGB::White;  // left arrow
            buffer[31-i][31] = CRGB::White;  // up arrow
            if (i > 0) buffer[31-i][31-i] = CRGB::White;  // diagonal
        }

        // Mark top-left start corner with cyan
        buffer[0][0] = CRGB::Cyan;

        // Simple horizontal arrow in top-left panel (magenta)
        for (uint8_t x = 3; x < 13; x++) {
            buffer[7][x] = CRGB::Magenta;
            buffer[8][x] = CRGB::Magenta;
        }
        // Arrow head pointing right
        buffer[5][11] = CRGB::Magenta;
        buffer[6][12] = CRGB::Magenta;
        buffer[7][13] = CRGB::Magenta;
        buffer[8][13] = CRGB::Magenta;
        buffer[9][12] = CRGB::Magenta;
        buffer[10][11] = CRGB::Magenta;

        // Simple serpentine snake pattern (white)
        // Right segment
        for (uint8_t x = 14; x <= 17; x++) {
            buffer[14][x] = CRGB::White;
        }
        // Down segment 1
        for (uint8_t y = 14; y <= 16; y++) {
            buffer[y][17] = CRGB::White;
        }
        // Left segment
        for (uint8_t x = 14; x <= 17; x++) {
            buffer[16][x] = CRGB::White;
        }
        // Down segment 2
        for (uint8_t y = 16; y <= 18; y++) {
            buffer[y][14] = CRGB::White;
        }
        // Right segment 2
        for (uint8_t x = 14; x <= 17; x++) {
            buffer[18][x] = CRGB::White;
        }
    }

    const char* getName() const override { return "TestPattern"; }
};

#endif // TEST_PATTERN_ANIMATION_H


