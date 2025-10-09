// Example: How to use the MatrixOrientation library for pixel art
// This shows the recommended workflow for displaying images and animations

#include <Arduino.h>
#include <FastLED.h>
#include "MatrixOrientation.h"

#define DATA_PIN 8
#define BRIGHTNESS 64

// LED array and matrix library
CRGB leds[1024];
MatrixOrientation matrix;

// Your pixel art as a simple 32x32 2D array
CRGB pixelArt[32][32];

// Alternative: Use 1D array for easier memory management
// CRGB pixelArt[1024];  // Flat array: index = y * 32 + x

void setup() {
    Serial.begin(115200);
    
    // Initialize FastLED
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, 1024);
    FastLED.setBrightness(BRIGHTNESS);
    
    // Option 1: Use default configuration (all panels serpentine, no rotation)
    matrix.begin();
    
    // Option 2: Custom configuration
    /*
    PanelConfig config;
    config.matrixWidth = 2;
    config.matrixHeight = 2;
    
    // Set panel order (if your physical wiring is different)
    config.panelOrder[0] = 0;  // Top-left is physical panel 0
    config.panelOrder[1] = 1;  // Top-right is physical panel 1
    config.panelOrder[2] = 2;  // Bottom-left is physical panel 2
    config.panelOrder[3] = 3;  // Bottom-right is physical panel 3
    
    // Set rotation per panel (if panels are mounted rotated)
    config.panelRotation[0] = 0;    // No rotation
    config.panelRotation[1] = 0;
    config.panelRotation[2] = 0;
    config.panelRotation[3] = 0;
    
    // Set serpentine mode per panel
    config.serpentine[0] = true;
    config.serpentine[1] = true;
    config.serpentine[2] = true;
    config.serpentine[3] = true;
    
    matrix.begin(config);
    */
}

void loop() {
    // WORKFLOW: Create your pixel art in the 2D array, then render it
    
    // Example 1: Draw a simple pattern
    drawSmileyFace();
    matrix.render(pixelArt, leds);  // Transform to LED strip
    FastLED.show();                  // Update LEDs
    delay(2000);
    
    // Example 2: Draw a gradient
    drawGradient();
    matrix.render(pixelArt, leds);
    FastLED.show();
    delay(2000);
    
    // Example 3: Animation frame
    static uint8_t frame = 0;
    drawAnimationFrame(frame++);
    matrix.render(pixelArt, leds);
    FastLED.show();
    delay(50);
}

// Example: Draw a smiley face
void drawSmileyFace() {
    // Clear canvas
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            pixelArt[y][x] = CRGB::Black;
        }
    }
    
    // Draw face (yellow circle)
    for (int y = 8; y < 24; y++) {
        for (int x = 8; x < 24; x++) {
            int dx = x - 16;
            int dy = y - 16;
            if (dx*dx + dy*dy < 64) {  // Circle radius 8
                pixelArt[y][x] = CRGB::Yellow;
            }
        }
    }
    
    // Draw eyes (black)
    pixelArt[12][12] = CRGB::Black;
    pixelArt[12][20] = CRGB::Black;
    
    // Draw smile
    for (int x = 12; x <= 20; x++) {
        pixelArt[18][x] = CRGB::Black;
    }
}

// Example: Draw a gradient
void drawGradient() {
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            uint8_t hue = (x * 256 / 32 + y * 256 / 32) / 2;
            pixelArt[y][x] = CHSV(hue, 255, 255);
        }
    }
}

// Example: Animation frame
void drawAnimationFrame(uint8_t frame) {
    // Moving dot
    int x = (frame % 32);
    int y = 16;
    
    // Clear
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            pixelArt[y][x] = CRGB::Black;
        }
    }
    
    // Draw dot
    pixelArt[y][x] = CRGB::Red;
}

// Example: Load from bitmap (for future use)
/*
void loadBitmap(const uint8_t* bitmap, CRGB color) {
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            int byteIndex = (y * 32 + x) / 8;
            int bitIndex = (y * 32 + x) % 8;
            
            if (bitmap[byteIndex] & (1 << bitIndex)) {
                pixelArt[y][x] = color;
            } else {
                pixelArt[y][x] = CRGB::Black;
            }
        }
    }
}
*/
