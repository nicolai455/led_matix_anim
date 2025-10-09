#include <Arduino.h>
#include <FastLED.h>
#include "MatrixOrientation.h"
#include "ConfigManager.h"

// LED Matrix Configuration - 4x 16x16 panels → 32x32 total
#define DATA_PIN 8
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 64
#define LOOP_DELAY_MS 1000

// LED array - 4 panels of 16x16 = 1024 LEDs
CRGB leds[1024];

// Matrix orientation library instance
MatrixOrientation matrix;

// Configuration manager
ConfigManager configManager;

// Function to disable the onboard LED
void disableOnboardLED() {
  Serial.println("Disabling onboard LED...");
  pinMode(97, OUTPUT);
  digitalWrite(97, LOW);
  Serial.println("Onboard LED disabled");
}

// Function to setup hardware components
void setupHardware() {
  Serial.println("Setting up hardware components...");
  
  // Check PSRAM
  if (psramInit()) {
    Serial.print("PSRAM initialized: ");
    Serial.print(ESP.getPsramSize() / 1024);
    Serial.println(" KB");
  } else {
    Serial.println("PSRAM not available (OK for this project)");
  }

  // Disable onboard LED
  disableOnboardLED();
  
  Serial.println("Hardware setup complete.");
}

// Function to initialize the LED matrix
void initializeLEDMatrix() {
  const uint16_t numLeds = 1024; // 4 panels × 16×16 = 1024 LEDs

  Serial.println("Initializing LED matrix...");
  Serial.printf("Number of LEDs: %u\n", numLeds);

  // Use WS2812B with GRB color order
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, numLeds);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.println("LED matrix ready!");
}

// Configure panel setup
PanelConfig createPanelConfig() {
  PanelConfig config;
  
  // Matrix dimensions (2x2 panels)
  config.matrixWidth = 2;
  config.matrixHeight = 2;
  
  // Panel order - which physical panel is at each logical position
  // Adjust these if your panels are wired in a different order
  config.panelOrder[0] = 0;  // Top-left is physical panel 0
  config.panelOrder[1] = 1;  // Top-right is physical panel 1
  config.panelOrder[2] = 2;  // Bottom-left is physical panel 2
  config.panelOrder[3] = 3;  // Bottom-right is physical panel 3
  
  // Panel rotation - adjust if panels are mounted rotated
  // Valid values: 0, 90, 180, 270 (degrees)
  config.panelRotation[0] = 0;
  config.panelRotation[1] = 0;
  config.panelRotation[2] = 0;
  config.panelRotation[3] = 0;
  
  // Serpentine wiring - true if panel uses zigzag wiring pattern
  // Set to false if panel uses straight row-by-row wiring
  config.serpentine[0] = true;
  config.serpentine[1] = true;
  config.serpentine[2] = true;
  config.serpentine[3] = true;
  
  return config;
}

void setup() {
  Serial.begin(115200);
  delay(2000);  // Give serial monitor time to connect

  Serial.println("\n\n=== WS2812B LED Matrix Setup ===");
  Serial.println("Hardware: ESP32-S3 N16R8 (16MB Flash, 8MB OPI PSRAM)");
  Serial.println("LED Panels: WS2812B flexible panels (4× 16×16 → 32×32)");
  Serial.println("");

  // Setup hardware components
  setupHardware();

  // Initialize LED matrix
  initializeLEDMatrix();
  
  // Setup configuration (handles loading with automatic fallback)
  PanelConfig config = configManager.setup();
  
  // Initialize matrix orientation library with loaded config
  matrix.begin(config);

  Serial.println("LED matrix initialized successfully!");
}

void loop() {
  // Draw animated test pattern with different modes
  matrix.drawPanelTestPattern(leds);
  FastLED.show();
  delay(LOOP_DELAY_MS);
}