#ifndef TEXT_ANIMATION_H
#define TEXT_ANIMATION_H

#include <Arduino.h>
#include <FastLED.h>
#include "Animation.h"
#include "TextRenderer.h"

class TextAnimation : public Animation {
private:
    String text;
    CRGB textColor;
    CRGB bgColor;
    int yPosition;
    bool scrolling;
    int scrollSpeed; // pixels per frame (positive = left to right)
    int currentX;
    uint16_t textWidth;
    bool centered;

public:
    // Static text (non-scrolling)
    TextAnimation(const char* displayText, CRGB color = CRGB::White, CRGB background = CRGB::Black, int y = 12, bool center = true)
        : text(displayText), textColor(color), bgColor(background), yPosition(y),
          scrolling(false), scrollSpeed(0), currentX(0), centered(center) {
        textWidth = TextRenderer::getTextWidth(displayText);
    }

    // Scrolling text
    TextAnimation(const char* displayText, int speed, CRGB color = CRGB::White, CRGB background = CRGB::Black, int y = 12)
        : text(displayText), textColor(color), bgColor(background), yPosition(y),
          scrolling(true), scrollSpeed(speed), currentX(TOTAL_SIZE), centered(false) {
        textWidth = TextRenderer::getTextWidth(displayText);
    }

    void setup() override {
        if (scrolling) {
            currentX = TOTAL_SIZE; // Start off-screen to the right
        }
    }

    void renderFrame(CRGB buffer[TOTAL_SIZE][TOTAL_SIZE], uint32_t frameTime) override {
        // Clear buffer with background color
        for (uint8_t y = 0; y < TOTAL_SIZE; y++) {
            for (uint8_t x = 0; x < TOTAL_SIZE; x++) {
                buffer[y][x] = bgColor;
            }
        }

        if (scrolling) {
            // Draw scrolling text
            TextRenderer::drawText(buffer, text.c_str(), currentX, yPosition, textColor);

            // Update scroll position
            currentX -= scrollSpeed;

            // Reset when text scrolls completely off-screen
            if (currentX < -textWidth) {
                currentX = TOTAL_SIZE;
            }
        } else {
            // Draw static text
            if (centered) {
                TextRenderer::drawCenteredText(buffer, text.c_str(), yPosition, textColor);
            } else {
                TextRenderer::drawText(buffer, text.c_str(), 0, yPosition, textColor);
            }
        }
    }

    const char* getName() const override { return "Text"; }

    // Methods to change text dynamically
    void setText(const char* newText) {
        text = newText;
        textWidth = TextRenderer::getTextWidth(newText);
        if (!scrolling) {
            currentX = centered ? (TOTAL_SIZE - textWidth) / 2 : 0;
        }
    }

    void setColor(CRGB color) { textColor = color; }
    void setBackground(CRGB color) { bgColor = color; }
    void setPosition(int y) { yPosition = y; }
    void setScrollSpeed(int speed) { scrollSpeed = speed; }
};

#endif // TEXT_ANIMATION_H
