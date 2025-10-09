#include "MatrixOrientation.h"

MatrixOrientation::MatrixOrientation() {
    // Initialize with default configuration
    config.matrixWidth = 2;
    config.matrixHeight = 2;
    
    for (int i = 0; i < NUM_PANELS; i++) {
        config.panelOrder[i] = i;      // Identity mapping by default
        config.panelRotation[i] = 0;   // No rotation by default
        config.serpentine[i] = true;   // Serpentine enabled by default
    }
    
    // WLED-style defaults
    config.startCorner = TOP_LEFT;     // Data starts at top-left
    config.panelLayout = HORIZONTAL;   // Panels arranged horizontally
    config.panelSerpentine = false;    // No panel zigzag by default
    
    // Set default panel colors for testing
    panelColors[0] = CRGB::Red;      // Panel 0 - Red
    panelColors[1] = CRGB::Green;    // Panel 1 - Green
    panelColors[2] = CRGB::Blue;     // Panel 2 - Blue
    panelColors[3] = CRGB::Yellow;   // Panel 3 - Yellow
}

void MatrixOrientation::begin() {
    // Initialize with default settings
    Serial.println("MatrixOrientation initialized");
    Serial.printf("Panel layout: %dx%d (%dx%d total)\n", 
                  config.matrixWidth, config.matrixHeight,
                  config.matrixWidth * PANEL_SIZE, config.matrixHeight * PANEL_SIZE);
    Serial.printf("Panel colors: Red, Green, Blue, Yellow\n");
    
    for (int i = 0; i < NUM_PANELS; i++) {
        Serial.printf("Panel %d: order=%d, rotation=%d, serpentine=%s\n",
                      i, config.panelOrder[i], config.panelRotation[i],
                      config.serpentine[i] ? "yes" : "no");
    }
}

void MatrixOrientation::begin(const PanelConfig& customConfig) {
    config = customConfig;
    begin();  // Call the regular begin to print config
}

PanelConfig MatrixOrientation::getConfig() const {
    return config;
}

uint8_t MatrixOrientation::getPhysicalPanelIndex(uint8_t logicalPanelX, uint8_t logicalPanelY) {
    // Apply WLED-style layout transformations
    uint8_t panelX = logicalPanelX;
    uint8_t panelY = logicalPanelY;
    
    // Apply start corner transformation
    switch (config.startCorner) {
        case TOP_RIGHT:
            panelX = config.matrixWidth - 1 - logicalPanelX;
            break;
        case BOTTOM_LEFT:
            panelY = config.matrixHeight - 1 - logicalPanelY;
            break;
        case BOTTOM_RIGHT:
            panelX = config.matrixWidth - 1 - logicalPanelX;
            panelY = config.matrixHeight - 1 - logicalPanelY;
            break;
        default: // TOP_LEFT
            break;
    }
    
    // Calculate panel index based on layout direction
    uint8_t panelIndex;
    if (config.panelLayout == HORIZONTAL) {
        // Horizontal layout: left-to-right, then down
        panelIndex = panelY * config.matrixWidth + panelX;
        
        // Apply panel serpentine (zigzag rows)
        if (config.panelSerpentine && (panelY % 2 == 1)) {
            panelIndex = panelY * config.matrixWidth + (config.matrixWidth - 1 - panelX);
        }
    } else {
        // Vertical layout: top-to-bottom, then right
        panelIndex = panelX * config.matrixHeight + panelY;
        
        // Apply panel serpentine (zigzag columns)
        if (config.panelSerpentine && (panelX % 2 == 1)) {
            panelIndex = panelX * config.matrixHeight + (config.matrixHeight - 1 - panelY);
        }
    }
    
    // Map through panelOrder for additional flexibility
    if (panelIndex < NUM_PANELS) {
        return config.panelOrder[panelIndex];
    }
    
    return 0;
}

uint16_t MatrixOrientation::getLEDIndex(uint8_t x, uint8_t y) {
    if (x >= TOTAL_SIZE || y >= TOTAL_SIZE) {
        return 0; // Invalid coordinates
    }
    
    // Determine which logical panel this coordinate belongs to
    uint8_t logicalPanelX = x / PANEL_SIZE;
    uint8_t logicalPanelY = y / PANEL_SIZE;
    
    // Map logical panel to physical panel using WLED-style layout
    uint8_t physicalPanel = getPhysicalPanelIndex(logicalPanelX, logicalPanelY);
    
    // Get local coordinates within the panel
    uint8_t localX = x % PANEL_SIZE;
    uint8_t localY = y % PANEL_SIZE;
    
    // Apply panel rotation (based on physical panel's rotation setting)
    uint8_t rotatedX = localX;
    uint8_t rotatedY = localY;
    
    switch (config.panelRotation[physicalPanel]) {
        case 90:
            rotatedX = PANEL_SIZE - 1 - localY;
            rotatedY = localX;
            break;
        case 180:
            rotatedX = PANEL_SIZE - 1 - localX;
            rotatedY = PANEL_SIZE - 1 - localY;
            break;
        case 270:
            rotatedX = localY;
            rotatedY = PANEL_SIZE - 1 - localX;
            break;
        default: // 0 degrees
            break;
    }
    
    // Calculate LED index within the physical panel
    uint16_t panelStartIndex = physicalPanel * (PANEL_SIZE * PANEL_SIZE);
    uint16_t ledIndex;
    
    // Apply serpentine pattern if enabled for this physical panel
    if (config.serpentine[physicalPanel] && (rotatedY % 2 == 1)) {
        // Odd row: right to left (serpentine)
        ledIndex = panelStartIndex + rotatedY * PANEL_SIZE + (PANEL_SIZE - 1 - rotatedX);
    } else {
        // Even row or non-serpentine: left to right
        ledIndex = panelStartIndex + rotatedY * PANEL_SIZE + rotatedX;
    }
    
    return ledIndex;
}

void MatrixOrientation::getMatrixCoords(uint16_t ledIndex, uint8_t& x, uint8_t& y) {
    if (ledIndex >= TOTAL_LEDS) {
        x = y = 0;
        return;
    }
    
    // Determine which panel this LED belongs to
    uint8_t panel = ledIndex / (PANEL_SIZE * PANEL_SIZE);
    uint16_t panelLedIndex = ledIndex % (PANEL_SIZE * PANEL_SIZE);
    
    // Get local coordinates within the panel (accounting for serpentine pattern)
    uint8_t localY = panelLedIndex / PANEL_SIZE;
    uint8_t localX;
    
    if (config.serpentine[panel] && (localY % 2 == 1)) {
        // Odd row: right to left (serpentine)
        localX = PANEL_SIZE - 1 - (panelLedIndex % PANEL_SIZE);
    } else {
        // Even row or non-serpentine: left to right
        localX = panelLedIndex % PANEL_SIZE;
    }
    
    // Apply reverse rotation
    uint8_t rotatedX = localX;
    uint8_t rotatedY = localY;
    
    switch (config.panelRotation[panel]) {
        case 90:
            rotatedX = localY;
            rotatedY = PANEL_SIZE - 1 - localX;
            break;
        case 180:
            rotatedX = PANEL_SIZE - 1 - localX;
            rotatedY = PANEL_SIZE - 1 - localY;
            break;
        case 270:
            rotatedX = PANEL_SIZE - 1 - localY;
            rotatedY = localX;
            break;
        default: // 0 degrees
            break;
    }
    
    // Convert to global coordinates
    uint8_t panelX = panel % 2;
    uint8_t panelY = panel / 2;
    x = panelX * PANEL_SIZE + rotatedX;
    y = panelY * PANEL_SIZE + rotatedY;
}

void MatrixOrientation::render(CRGB pixelArt[TOTAL_SIZE][TOTAL_SIZE], CRGB* leds) {
    // Render 2D pixel art array to LED strip
    for (uint8_t y = 0; y < TOTAL_SIZE; y++) {
        for (uint8_t x = 0; x < TOTAL_SIZE; x++) {
            uint16_t ledIndex = getLEDIndex(x, y);
            leds[ledIndex] = pixelArt[y][x];
        }
    }
}

void MatrixOrientation::render(CRGB* pixelArt, CRGB* leds) {
    // Render flat 1D array (row-major: index = y * TOTAL_SIZE + x)
    for (uint8_t y = 0; y < TOTAL_SIZE; y++) {
        for (uint8_t x = 0; x < TOTAL_SIZE; x++) {
            uint16_t ledIndex = getLEDIndex(x, y);
            uint16_t pixelIndex = y * TOTAL_SIZE + x;
            leds[ledIndex] = pixelArt[pixelIndex];
        }
    }
}

void MatrixOrientation::setPanelRotation(uint8_t panel, uint8_t rotation) {
    if (panel < NUM_PANELS && (rotation == 0 || rotation == 90 || rotation == 180 || rotation == 270)) {
        config.panelRotation[panel] = rotation;
        Serial.printf("Panel %d rotation set to %d degrees\n", panel, rotation);
    }
}

void MatrixOrientation::setPanelOrder(uint8_t position, uint8_t physicalPanel) {
    if (position < NUM_PANELS && physicalPanel < NUM_PANELS) {
        config.panelOrder[position] = physicalPanel;
        Serial.printf("Position %d mapped to physical panel %d\n", position, physicalPanel);
    }
}

void MatrixOrientation::setSerpentine(uint8_t panel, bool enabled) {
    if (panel < NUM_PANELS) {
        config.serpentine[panel] = enabled;
        Serial.printf("Panel %d serpentine: %s\n", panel, enabled ? "enabled" : "disabled");
    }
}

uint8_t MatrixOrientation::getPanelNumber(uint8_t x, uint8_t y) {
    if (x >= TOTAL_SIZE || y >= TOTAL_SIZE) {
        return 0;
    }
    
    uint8_t panelX = x / PANEL_SIZE;
    uint8_t panelY = y / PANEL_SIZE;
    return panelY * 2 + panelX;
}

uint8_t MatrixOrientation::getPanelNumber(uint16_t ledIndex) {
    if (ledIndex >= TOTAL_LEDS) {
        return 0;
    }
    
    return ledIndex / (PANEL_SIZE * PANEL_SIZE);
}

void MatrixOrientation::setPanelColor(uint8_t panel, CRGB color) {
    if (panel < NUM_PANELS) {
        panelColors[panel] = color;
    }
}

CRGB MatrixOrientation::getPanelColor(uint8_t panel) {
    if (panel < NUM_PANELS) {
        return panelColors[panel];
    }
    return CRGB::Black;
}

void MatrixOrientation::setPixel(CRGB* leds, uint8_t x, uint8_t y, CRGB color) {
    uint16_t index = getLEDIndex(x, y);
    if (index < TOTAL_LEDS) {
        leds[index] = color;
    }
}

void MatrixOrientation::drawLine(CRGB* leds, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, CRGB color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1, y = y1;
    
    while (true) {
        setPixel(leds, x, y, color);
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void MatrixOrientation::drawRect(CRGB* leds, uint8_t x, uint8_t y, uint8_t width, uint8_t height, CRGB color) {
    // Top and bottom lines
    for (uint8_t i = 0; i < width; i++) {
        setPixel(leds, x + i, y, color);
        setPixel(leds, x + i, y + height - 1, color);
    }
    // Left and right lines
    for (uint8_t i = 0; i < height; i++) {
        setPixel(leds, x, y + i, color);
        setPixel(leds, x + width - 1, y + i, color);
    }
}

void MatrixOrientation::drawFilledRect(CRGB* leds, uint8_t x, uint8_t y, uint8_t width, uint8_t height, CRGB color) {
    for (uint8_t i = 0; i < height; i++) {
        for (uint8_t j = 0; j < width; j++) {
            setPixel(leds, x + j, y + i, color);
        }
    }
}

void MatrixOrientation::drawLargeNumber(CRGB* leds, uint8_t x, uint8_t y, uint8_t number, CRGB color) {
    // Simple 8x8 number patterns
    const uint8_t numbers[10][8] = {
        {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C}, // 0
        {0x08, 0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x3E}, // 1
        {0x3C, 0x42, 0x02, 0x04, 0x08, 0x10, 0x20, 0x7E}, // 2
        {0x3C, 0x42, 0x02, 0x1C, 0x02, 0x02, 0x42, 0x3C}, // 3
        {0x04, 0x0C, 0x14, 0x24, 0x44, 0x7E, 0x04, 0x04}, // 4
        {0x7E, 0x40, 0x40, 0x7C, 0x02, 0x02, 0x42, 0x3C}, // 5
        {0x3C, 0x42, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C}, // 6
        {0x7E, 0x02, 0x04, 0x08, 0x10, 0x10, 0x10, 0x10}, // 7
        {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C}, // 8
        {0x3C, 0x42, 0x42, 0x42, 0x3E, 0x02, 0x42, 0x3C}  // 9
    };
    
    if (number > 9) return;
    
    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 8; col++) {
            if (numbers[number][row] & (0x80 >> col)) {
                setPixel(leds, x + col, y + row, color);
            }
        }
    }
}

void MatrixOrientation::drawArrow(CRGB* leds, uint8_t x, uint8_t y, uint8_t direction, CRGB color) {
    // Draw a 5x5 arrow pointing in the specified direction
    // direction: 0=up, 1=right, 2=down, 3=left
    
    switch (direction) {
        case 0: // Up arrow
            setPixel(leds, x + 2, y, color);     // tip
            setPixel(leds, x + 1, y + 1, color); // sides
            setPixel(leds, x + 3, y + 1, color);
            setPixel(leds, x, y + 2, color);
            setPixel(leds, x + 4, y + 2, color);
            setPixel(leds, x + 2, y + 3, color); // shaft
            setPixel(leds, x + 2, y + 4, color);
            break;
            
        case 1: // Right arrow
            setPixel(leds, x + 4, y + 2, color); // tip
            setPixel(leds, x + 3, y + 1, color); // sides
            setPixel(leds, x + 3, y + 3, color);
            setPixel(leds, x + 2, y, color);
            setPixel(leds, x + 2, y + 4, color);
            setPixel(leds, x + 1, y + 2, color); // shaft
            setPixel(leds, x, y + 2, color);
            break;
            
        case 2: // Down arrow
            setPixel(leds, x + 2, y + 4, color); // tip
            setPixel(leds, x + 1, y + 3, color); // sides
            setPixel(leds, x + 3, y + 3, color);
            setPixel(leds, x, y + 2, color);
            setPixel(leds, x + 4, y + 2, color);
            setPixel(leds, x + 2, y + 1, color); // shaft
            setPixel(leds, x + 2, y, color);
            break;
            
        case 3: // Left arrow
            setPixel(leds, x, y + 2, color);     // tip
            setPixel(leds, x + 1, y + 1, color); // sides
            setPixel(leds, x + 1, y + 3, color);
            setPixel(leds, x + 2, y, color);
            setPixel(leds, x + 2, y + 4, color);
            setPixel(leds, x + 3, y + 2, color); // shaft
            setPixel(leds, x + 4, y + 2, color);
            break;
    }
}

void MatrixOrientation::drawPanelTestPattern(CRGB* leds) {
    // Fill each panel with its background color
    fillAllPanels(leds);
    
    // Draw corner arrows (white)
    // TL corner
    for (int x = 0; x <= 5; x++) setPixel(leds, x, 0, CRGB::White);
    for (int y = 0; y <= 5; y++) setPixel(leds, 0, y, CRGB::White);
    for (int i = 1; i <= 5; i++) setPixel(leds, i, i, CRGB::White);
    
    // TR corner
    for (int x = 26; x <= 31; x++) setPixel(leds, x, 0, CRGB::White);
    for (int y = 0; y <= 5; y++) setPixel(leds, 31, y, CRGB::White);
    for (int i = 1; i <= 5; i++) setPixel(leds, 31 - i, i, CRGB::White);
    
    // BL corner
    for (int x = 0; x <= 5; x++) setPixel(leds, x, 31, CRGB::White);
    for (int y = 26; y <= 31; y++) setPixel(leds, 0, y, CRGB::White);
    for (int i = 1; i <= 5; i++) setPixel(leds, i, 31 - i, CRGB::White);
    
    // BR corner
    for (int x = 26; x <= 31; x++) setPixel(leds, x, 31, CRGB::White);
    for (int y = 26; y <= 31; y++) setPixel(leds, 31, y, CRGB::White);
    for (int i = 26; i <= 30; i++) setPixel(leds, i, i, CRGB::White);
    
    // Mark start corner with CYAN pixel at actual corner
    switch (config.startCorner) {
        case TOP_LEFT:
            setPixel(leds, 0, 0, CRGB::Cyan);
            break;
        case TOP_RIGHT:
            setPixel(leds, 31, 0, CRGB::Cyan);
            break;
        case BOTTOM_LEFT:
            setPixel(leds, 0, 31, CRGB::Cyan);
            break;
        case BOTTOM_RIGHT:
            setPixel(leds, 31, 31, CRGB::Cyan);
            break;
    }
    
    // Show panel layout direction with magenta arrow in top-left panel
    if (config.panelLayout == HORIZONTAL) {
        // Horizontal: draw arrow pointing right in center of top-left panel
        // Arrow body (horizontal line)
        for (int x = 3; x < 13; x++) {
            setPixel(leds, x, 7, CRGB::Magenta);
            setPixel(leds, x, 8, CRGB::Magenta);
        }
        // Arrow head (pointing right)
        setPixel(leds, 11, 5, CRGB::Magenta);
        setPixel(leds, 12, 6, CRGB::Magenta);
        setPixel(leds, 13, 7, CRGB::Magenta);
        setPixel(leds, 13, 8, CRGB::Magenta);
        setPixel(leds, 12, 9, CRGB::Magenta);
        setPixel(leds, 11, 10, CRGB::Magenta);
    } else {
        // Vertical: draw arrow pointing down in center of top-left panel
        // Arrow body (vertical line)
        for (int y = 3; y < 13; y++) {
            setPixel(leds, 7, y, CRGB::Magenta);
            setPixel(leds, 8, y, CRGB::Magenta);
        }
        // Arrow head (pointing down)
        setPixel(leds, 5, 11, CRGB::Magenta);
        setPixel(leds, 6, 12, CRGB::Magenta);
        setPixel(leds, 7, 13, CRGB::Magenta);
        setPixel(leds, 8, 13, CRGB::Magenta);
        setPixel(leds, 9, 12, CRGB::Magenta);
        setPixel(leds, 10, 11, CRGB::Magenta);
    }
    
    // Show panel serpentine with white snake in center
    if (config.panelSerpentine) {
        // Draw snake pattern: goes right, down, left, down
        // Right segment
        for (int x = 14; x <= 17; x++) {
            setPixel(leds, x, 14, CRGB::White);
        }
        // Down segment 1
        for (int y = 14; y <= 16; y++) {
            setPixel(leds, 17, y, CRGB::White);
        }
        // Left segment
        for (int x = 14; x <= 17; x++) {
            setPixel(leds, x, 16, CRGB::White);
        }
        // Down segment 2
        for (int y = 16; y <= 18; y++) {
            setPixel(leds, 14, y, CRGB::White);
        }
        // Right segment 2
        for (int x = 14; x <= 17; x++) {
            setPixel(leds, x, 18, CRGB::White);
        }
    }
}

void MatrixOrientation::drawOrientationMarkers(CRGB* leds) {
    // Corner markers with arrows pointing inward
    // Top-left corner - arrows pointing right and down
    drawLine(leds, 0, 0, 4, 0, CRGB::White);  // Right arrow
    drawLine(leds, 0, 0, 0, 4, CRGB::White);  // Down arrow
    
    // Top-right corner - arrows pointing left and down
    drawLine(leds, 31, 0, 27, 0, CRGB::White); // Left arrow
    drawLine(leds, 31, 0, 31, 4, CRGB::White); // Down arrow
    
    // Bottom-left corner - arrows pointing right and up
    drawLine(leds, 0, 31, 4, 31, CRGB::White); // Right arrow
    drawLine(leds, 0, 31, 0, 27, CRGB::White); // Up arrow
    
    // Bottom-right corner - arrows pointing left and up
    drawLine(leds, 31, 31, 27, 31, CRGB::White); // Left arrow
    drawLine(leds, 31, 31, 31, 27, CRGB::White); // Up arrow
}

void MatrixOrientation::drawDiagonalLines(CRGB* leds) {
    // Draw diagonal lines from corner to corner
    drawLine(leds, 0, 0, 31, 31, CRGB::White);   // Top-left to bottom-right
    drawLine(leds, 31, 0, 0, 31, CRGB::White);   // Top-right to bottom-left
}

void MatrixOrientation::drawCompleteTestPattern(CRGB* leds) {
    // Clear all LEDs
    clear(leds);
    
    // Draw panel identification numbers
    drawPanelTestPattern(leds);
    
    // Draw orientation markers
    drawOrientationMarkers(leds);
    
    // Draw diagonal verification lines
    drawDiagonalLines(leds);
}

void MatrixOrientation::clear(CRGB* leds) {
    for (int i = 0; i < TOTAL_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
}

void MatrixOrientation::fill(CRGB* leds, CRGB color) {
    for (int i = 0; i < TOTAL_LEDS; i++) {
        leds[i] = color;
    }
}

void MatrixOrientation::fillPanel(CRGB* leds, uint8_t logicalPanel) {
    if (logicalPanel >= NUM_PANELS) return;
    
    // Calculate the logical panel's position
    uint8_t panelX = logicalPanel % 2;
    uint8_t panelY = logicalPanel / 2;
    
    // Fill using coordinates (goes through transform)
    uint8_t startX = panelX * PANEL_SIZE;
    uint8_t startY = panelY * PANEL_SIZE;
    
    for (uint8_t y = 0; y < PANEL_SIZE; y++) {
        for (uint8_t x = 0; x < PANEL_SIZE; x++) {
            setPixel(leds, startX + x, startY + y, panelColors[logicalPanel]);
        }
    }
}

void MatrixOrientation::fillAllPanels(CRGB* leds) {
    // Fill each logical position with its color
    // Logical positions: 0=TL, 1=TR, 2=BL, 3=BR
    for (uint8_t panelY = 0; panelY < 2; panelY++) {
        for (uint8_t panelX = 0; panelX < 2; panelX++) {
            uint8_t logicalPanel = panelY * 2 + panelX;
            
            uint8_t startX = panelX * PANEL_SIZE;
            uint8_t startY = panelY * PANEL_SIZE;
            
            for (uint8_t y = 0; y < PANEL_SIZE; y++) {
                for (uint8_t x = 0; x < PANEL_SIZE; x++) {
                    setPixel(leds, startX + x, startY + y, panelColors[logicalPanel]);
                }
            }
        }
    }
}
