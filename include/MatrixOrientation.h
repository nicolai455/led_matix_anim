#ifndef MATRIX_ORIENTATION_H
#define MATRIX_ORIENTATION_H

#include <Arduino.h>
#include <FastLED.h>

// Matrix configuration
#define NUM_PANELS 4
#define PANEL_SIZE 16
#define TOTAL_SIZE 32
#define TOTAL_LEDS (NUM_PANELS * PANEL_SIZE * PANEL_SIZE)

// Panel layout: 4 panels arranged as 2x2
// Panel indices: 0=top-left, 1=top-right, 2=bottom-left, 3=bottom-right
// LED indices: panel 0=[0..255], panel 1=[256..511], panel 2=[512..767], panel 3=[768..1023]

// Panel layout enums
enum PanelStartCorner {
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_LEFT = 2,
    BOTTOM_RIGHT = 3
};

enum PanelDirection {
    HORIZONTAL = 0,  // Panels arranged horizontally (left-right, then down)
    VERTICAL = 1     // Panels arranged vertically (top-bottom, then right)
};

// Configuration structure for panel setup
struct PanelConfig {
    uint8_t panelOrder[NUM_PANELS];      // Physical panel order (which physical panel is at position 0,1,2,3)
    uint8_t panelRotation[NUM_PANELS];   // Rotation per panel (0, 90, 180, 270)
    bool serpentine[NUM_PANELS];         // Whether each panel uses serpentine wiring
    uint8_t matrixWidth;                 // Width in panels (e.g., 2 for 2x2)
    uint8_t matrixHeight;                // Height in panels (e.g., 2 for 2x2)
    
    // WLED-style layout configuration
    uint8_t startCorner;                 // Where data stream starts (0=TL, 1=TR, 2=BL, 3=BR)
    uint8_t panelLayout;                 // How panels are arranged (0=horizontal, 1=vertical)
    bool panelSerpentine;                // Whether panels zigzag
};

class MatrixOrientation {
private:
    // Panel configuration
    PanelConfig config;
    
    // Panel colors for identification
    CRGB panelColors[NUM_PANELS];

public:
    MatrixOrientation();
    
    // Initialize with default settings
    void begin();
    
    // Initialize with custom configuration
    void begin(const PanelConfig& customConfig);
    
    // Get current configuration
    PanelConfig getConfig() const;
    
    // Convert matrix coordinates (x,y) to LED index
    uint16_t getLEDIndex(uint8_t x, uint8_t y);
    
    // Helper: Convert logical panel position to physical panel index (WLED-style)
    uint8_t getPhysicalPanelIndex(uint8_t logicalPanelX, uint8_t logicalPanelY);
    
    // Convert LED index to matrix coordinates
    void getMatrixCoords(uint16_t ledIndex, uint8_t& x, uint8_t& y);
    
    // Render a 2D pixel art array to the LED strip
    // pixelArt is a 32x32 array in row-major order: pixelArt[y][x]
    void render(CRGB pixelArt[TOTAL_SIZE][TOTAL_SIZE], CRGB* leds);
    
    // Render from a flat 1D array (for easier memory management)
    void render(CRGB* pixelArt, CRGB* leds);
    
    // Set panel rotation (0, 90, 180, 270 degrees)
    void setPanelRotation(uint8_t panel, uint8_t rotation);
    
    // Set panel order
    void setPanelOrder(uint8_t position, uint8_t physicalPanel);
    
    // Set serpentine mode for a panel
    void setSerpentine(uint8_t panel, bool enabled);
    
    // Get panel number for given coordinates
    uint8_t getPanelNumber(uint8_t x, uint8_t y);
    
    // Get panel number for given LED index
    uint8_t getPanelNumber(uint16_t ledIndex);
    
    // Set color for a specific panel
    void setPanelColor(uint8_t panel, CRGB color);
    
    // Get color for a specific panel
    CRGB getPanelColor(uint8_t panel);
    
    // Draw a pixel at matrix coordinates
    void setPixel(CRGB* leds, uint8_t x, uint8_t y, CRGB color);
    
    // Draw a line between two points
    void drawLine(CRGB* leds, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, CRGB color);
    
    // Draw a rectangle
    void drawRect(CRGB* leds, uint8_t x, uint8_t y, uint8_t width, uint8_t height, CRGB color);
    
    // Draw a filled rectangle
    void drawFilledRect(CRGB* leds, uint8_t x, uint8_t y, uint8_t width, uint8_t height, CRGB color);
    
    // Draw a large number (0-9) at specified position
    void drawLargeNumber(CRGB* leds, uint8_t x, uint8_t y, uint8_t number, CRGB color);
    
    // Draw an arrow pointing in a direction (0=up, 1=right, 2=down, 3=left)
    void drawArrow(CRGB* leds, uint8_t x, uint8_t y, uint8_t direction, CRGB color);
    
    // Draw panel identification pattern
    void drawPanelTestPattern(CRGB* leds);
    
    // Draw orientation markers (arrows and corners)
    void drawOrientationMarkers(CRGB* leds);
    
    // Draw diagonal verification lines
    void drawDiagonalLines(CRGB* leds);
    
    // Draw complete test pattern
    void drawCompleteTestPattern(CRGB* leds);
    
    // Clear all LEDs
    void clear(CRGB* leds);
    
    // Fill entire matrix with a single color
    void fill(CRGB* leds, CRGB color);
    
    // Fill a specific panel with its color
    void fillPanel(CRGB* leds, uint8_t panel);
    
    // Fill all panels with their respective colors
    void fillAllPanels(CRGB* leds);
};

#endif // MATRIX_ORIENTATION_H
