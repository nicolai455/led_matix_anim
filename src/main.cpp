#include <Arduino.h>
#include <FastLED.h>
#include "MatrixOrientation.h"
#include "ConfigManager.h"
#include "AnimationManager.h"
#include "animations/TestPatternAnimation.h"
#include "animations/RainbowAnimation.h"
#include "animations/SolidColorAnimation.h"
#include "animations/FrameAnimation.h"
#include "animations/TextAnimation.h"
#include "frame_io/ProgmemFrameSource.h"
#include "frame_io/FsFrameSource.h"

// LED Matrix Configuration - 4x 16x16 panels → 32x32 total
// Hardware settings are now loaded from config file
#define TOTAL_LEDS 1024  // 4 panels × 16×16 = 1024 LEDs

// LED array - 4 panels of 16x16 = 1024 LEDs
CRGB leds[1024];

// Matrix orientation library instance
MatrixOrientation matrix;

// Configuration manager
ConfigManager configManager;

// Animation manager
AnimationManager animManager(&matrix);

// Function to disable the onboard LED
void disableOnboardLED() {
  Serial.println("Disabling onboard LED...");
  
  // ESP32-S3 onboard LED is typically on GPIO 48 (built-in LED)
  pinMode(48, OUTPUT);
  digitalWrite(48, LOW);
  
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
  Serial.println("Initializing LED matrix...");
  Serial.printf("Number of LEDs: %u\n", TOTAL_LEDS);
  
  // Get hardware settings from config
  uint8_t dataPin = configManager.getLedDataPin();
  uint8_t brightness = configManager.getLedBrightness();
  String ledType = configManager.getLedType();
  String colorOrder = configManager.getLedColorOrder();
  
  Serial.printf("Data Pin: %d\n", dataPin);
  Serial.printf("Brightness: %d\n", brightness);
  Serial.printf("LED Type: %s\n", ledType.c_str());
  Serial.printf("Color Order: %s\n", colorOrder.c_str());

  // Initialize FastLED with config values
  // Note: FastLED template requires compile-time pin, so we'll use the config value
  // but template will still use hardcoded pin. This is a limitation of FastLED.
  if (ledType == "WS2812B") {
    if (colorOrder == "GRB") {
      FastLED.addLeds<WS2812B, 8, GRB>(leds, TOTAL_LEDS);
    } else if (colorOrder == "RGB") {
      FastLED.addLeds<WS2812B, 8, RGB>(leds, TOTAL_LEDS);
    } else {
      FastLED.addLeds<WS2812B, 8, GRB>(leds, TOTAL_LEDS); // Default to GRB
    }
  } else {
    // Default to WS2812B if unknown type
    FastLED.addLeds<WS2812B, 8, GRB>(leds, TOTAL_LEDS);
  }
  
  FastLED.setBrightness(brightness);
  Serial.println("LED matrix ready!");
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

  // Register animations
  TestPatternAnimation* testAnim = new TestPatternAnimation();
  RainbowAnimation* rainbowAnim = new RainbowAnimation();
  SolidColorAnimation* solidRed = new SolidColorAnimation(CRGB::Red);
  TextAnimation* staticText = new TextAnimation("HELLO", CRGB::Green, CRGB::Black, 12, true);
  TextAnimation* scrollText = new TextAnimation("SCROLLING TEXT! ", 1, CRGB::Cyan, CRGB::Black, 12);

  animManager.registerAnimation(testAnim);
  animManager.registerAnimation(rainbowAnim);
  animManager.registerAnimation(solidRed);
  animManager.registerAnimation(staticText);
  animManager.registerAnimation(scrollText);

  // Optional: load frame animation from PROGMEM or FS (FS path from config)
  if (configManager.getFsAnimationPath().length() > 0) {
    FsFrameSource* fsSrc = new FsFrameSource(configManager.getFsAnimationPath().c_str());
    if (fsSrc->getFrameCount() > 0) {
      FrameAnimation* frameAnim = new FrameAnimation(fsSrc, 100);
      animManager.registerAnimation(frameAnim);
    } else {
      delete fsSrc;
    }
  }

  // Auto-cycle from config
  animManager.setAutoCycle(configManager.getAutoCycleMs());

  // Select default animation by name if provided
  String defaultName = configManager.getDefaultAnimation();
  if (!defaultName.isEmpty()) {
    if (!animManager.switchToByName(defaultName.c_str())) {
      animManager.setup();
    }
  } else {
    animManager.setup();
  }
}

void loop() {
  // Drive current animation
  animManager.loop(leds);
  FastLED.show();

}