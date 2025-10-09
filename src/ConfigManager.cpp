#include "ConfigManager.h"

ConfigManager::ConfigManager() {
    // Constructor
}

PanelConfig ConfigManager::setup() {
    Serial.println("\n=== Configuration Setup ===");
    
    PanelConfig config;
    bool configLoaded = false;
    
    // Priority 1: Try loading from config file (always check file first)
    if (isFilesystemReady()) {
        Serial.println("📄 Loading configuration from file...");
        configLoaded = loadDefaultConfig(config);
        
        if (configLoaded) {
            Serial.println("✓ Configuration loaded from file");
        }
    }
    
    // Priority 2: Use hardcoded defaults if file not found
    if (!configLoaded) {
        Serial.println("⚙️  File not found, using hardcoded defaults...");
        applyDefaults(config);
    }
    
    // If config was loaded from file, animation and LED settings are already loaded in loadConfigFromFile()
    // If not loaded from file, defaults are already set in applyDefaults()

    // Print animation settings
    Serial.println("\n=== Animation Settings ===");
    Serial.printf("Default Animation: %s\n", defaultAnimationName.c_str());
    Serial.printf("Auto Cycle: %d ms\n", defaultAutoCycleMs);
    Serial.printf("FS Animation Path: %s\n", defaultFsAnimationPath.c_str());
    Serial.println("=========================");
    
    // Print LED hardware settings
    Serial.println("\n=== LED Hardware Settings ===");
    Serial.printf("Data Pin: %d\n", ledDataPin);
    Serial.printf("Brightness: %d\n", ledBrightness);
    Serial.printf("LED Type: %s\n", ledType.c_str());
    Serial.printf("Color Order: %s\n", ledColorOrder.c_str());
    Serial.println("=============================");

    // Print final configuration
    Serial.println("\n=== Final Configuration ===");
    printConfig(config);
    Serial.println("===========================\n");
    
    return config;
}

bool ConfigManager::validateConfig(const PanelConfig& config) {
    // Validate matrix dimensions
    if (config.matrixWidth == 0 || config.matrixWidth > 10) {
        Serial.printf("⚠ Invalid matrixWidth: %d (must be 1-10)\n", config.matrixWidth);
        return false;
    }
    if (config.matrixHeight == 0 || config.matrixHeight > 10) {
        Serial.printf("⚠ Invalid matrixHeight: %d (must be 1-10)\n", config.matrixHeight);
        return false;
    }
    
    // Validate panel order (no duplicates, all in range)
    bool used[NUM_PANELS] = {false};
    for (int i = 0; i < NUM_PANELS; i++) {
        if (config.panelOrder[i] >= NUM_PANELS) {
            Serial.printf("⚠ Invalid panelOrder[%d]: %d (must be 0-%d)\n", 
                         i, config.panelOrder[i], NUM_PANELS - 1);
            return false;
        }
        if (used[config.panelOrder[i]]) {
            Serial.printf("⚠ Duplicate panelOrder: %d appears multiple times\n", 
                         config.panelOrder[i]);
            return false;
        }
        used[config.panelOrder[i]] = true;
    }
    
    // Validate panel rotation (must be 0, 90, 180, or 270)
    for (int i = 0; i < NUM_PANELS; i++) {
        uint8_t rot = config.panelRotation[i];
        if (rot != 0 && rot != 90 && rot != 180 && rot != 270) {
            Serial.printf("⚠ Invalid panelRotation[%d]: %d (must be 0, 90, 180, or 270)\n", 
                         i, rot);
            return false;
        }
    }
    
    return true;
}

void ConfigManager::applyDefaults(PanelConfig& config) {
    // Apply safe default values
    Serial.println("Applying default configuration values...");
    
    // Panel configuration defaults
    config.matrixWidth = 2;
    config.matrixHeight = 2;
    
    for (int i = 0; i < NUM_PANELS; i++) {
        config.panelOrder[i] = i;
        config.panelRotation[i] = 0;
        config.serpentine[i] = true;
    }
    
    // WLED-style defaults
    config.startCorner = 0;      // TOP_LEFT
    config.panelLayout = 0;      // HORIZONTAL
    config.panelSerpentine = false;
    
    // Animation defaults
    defaultAnimationName = "TestPattern";
    defaultAutoCycleMs = 0;
    defaultFsAnimationPath = "/animations/example.lfx";
    
    // LED hardware defaults
    ledDataPin = 8;
    ledBrightness = 128;
    ledType = "WS2812B";
    ledColorOrder = "GRB";
}

bool ConfigManager::savePanelConfig(const PanelConfig& config) {
    // Validate before saving
    if (!validateConfig(config)) {
        Serial.println("✗ Cannot save invalid configuration");
        return false;
    }
    
    preferences.begin(NAMESPACE, false);  // Read/write mode
    
    bool success = true;
    
    // Save matrix dimensions
    success &= preferences.putUChar("matrixWidth", config.matrixWidth);
    success &= preferences.putUChar("matrixHeight", config.matrixHeight);
    
    // Save panel order
    success &= preferences.putBytes("panelOrder", config.panelOrder, NUM_PANELS);
    
    // Save panel rotation
    success &= preferences.putBytes("panelRot", config.panelRotation, NUM_PANELS);
    
    // Save serpentine settings (convert bool array to byte array)
    uint8_t serpentineBytes[NUM_PANELS];
    for (int i = 0; i < NUM_PANELS; i++) {
        serpentineBytes[i] = config.serpentine[i] ? 1 : 0;
    }
    success &= preferences.putBytes("serpentine", serpentineBytes, NUM_PANELS);
    
    preferences.end();
    
    if (success) {
        Serial.println("✓ Panel configuration saved to NVS");
    } else {
        Serial.println("✗ Failed to save panel configuration");
    }
    
    return success;
}

bool ConfigManager::loadPanelConfig(PanelConfig& config) {
    preferences.begin(NAMESPACE, true);  // Read-only mode
    
    // Check if config exists
    if (!preferences.isKey("matrixWidth")) {
        preferences.end();
        Serial.println("No saved configuration found");
        return false;
    }
    
    // Load matrix dimensions
    config.matrixWidth = preferences.getUChar("matrixWidth", 2);
    config.matrixHeight = preferences.getUChar("matrixHeight", 2);
    
    // Load panel order
    preferences.getBytes("panelOrder", config.panelOrder, NUM_PANELS);
    
    // Load panel rotation
    preferences.getBytes("panelRot", config.panelRotation, NUM_PANELS);
    
    // Load serpentine settings
    uint8_t serpentineBytes[NUM_PANELS];
    preferences.getBytes("serpentine", serpentineBytes, NUM_PANELS);
    for (int i = 0; i < NUM_PANELS; i++) {
        config.serpentine[i] = (serpentineBytes[i] != 0);
    }
    
    preferences.end();
    
    Serial.println("✓ Panel configuration loaded from NVS");
    return true;
}

bool ConfigManager::loadConfigFromFile(const char* path, PanelConfig& config) {
    if (!LittleFS.begin()) {
        Serial.println("✗ Failed to mount LittleFS");
        return false;
    }
    
    if (!LittleFS.exists(path)) {
        Serial.printf("✗ Config file not found: %s\n", path);
        return false;
    }
    
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.printf("✗ Failed to open config file: %s\n", path);
        return false;
    }
    
    // Read file contents
    String jsonString = file.readString();
    file.close();
    
    Serial.printf("📄 Loading config from: %s\n", path);
    Serial.printf("   File size: %d bytes\n", jsonString.length());
    
    // Parse JSON with validation and fallback
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        Serial.printf("✗ JSON parse error: %s\n", error.c_str());
        Serial.println("   Applying default configuration...");
        applyDefaults(config);
        return false;
    }
    
    // Start with defaults, then override with file values
    applyDefaults(config);
    
    // Load matrix dimensions (with validation)
    if (doc.containsKey("matrixWidth")) {
        uint8_t width = doc["matrixWidth"];
        if (width > 0 && width <= 10) {
            config.matrixWidth = width;
        } else {
            Serial.printf("⚠ Invalid matrixWidth in file: %d, using default: %d\n", 
                         width, config.matrixWidth);
        }
    } else {
        Serial.println("⚠ Missing 'matrixWidth', using default: 2");
    }
    
    if (doc.containsKey("matrixHeight")) {
        uint8_t height = doc["matrixHeight"];
        if (height > 0 && height <= 10) {
            config.matrixHeight = height;
        } else {
            Serial.printf("⚠ Invalid matrixHeight in file: %d, using default: %d\n", 
                         height, config.matrixHeight);
        }
    } else {
        Serial.println("⚠ Missing 'matrixHeight', using default: 2");
    }
    
    // Load panel order (with validation)
    if (doc.containsKey("panelOrder") && doc["panelOrder"].is<JsonArray>()) {
        JsonArray panelOrder = doc["panelOrder"];
        if (panelOrder.size() >= NUM_PANELS) {
            for (int i = 0; i < NUM_PANELS; i++) {
                uint8_t order = panelOrder[i];
                if (order < NUM_PANELS) {
                    config.panelOrder[i] = order;
                } else {
                    Serial.printf("⚠ Invalid panelOrder[%d]: %d, using default: %d\n", 
                                 i, order, i);
                }
            }
        } else {
            Serial.printf("⚠ panelOrder array too small (%d), using defaults\n", 
                         panelOrder.size());
        }
    } else {
        Serial.println("⚠ Missing or invalid 'panelOrder', using defaults");
    }
    
    // Load panel rotation (with validation)
    if (doc.containsKey("panelRotation") && doc["panelRotation"].is<JsonArray>()) {
        JsonArray panelRotation = doc["panelRotation"];
        if (panelRotation.size() >= NUM_PANELS) {
            for (int i = 0; i < NUM_PANELS; i++) {
                uint8_t rot = panelRotation[i];
                if (rot == 0 || rot == 90 || rot == 180 || rot == 270) {
                    config.panelRotation[i] = rot;
                } else {
                    Serial.printf("⚠ Invalid panelRotation[%d]: %d, using default: 0\n", 
                                 i, rot);
                }
            }
        } else {
            Serial.printf("⚠ panelRotation array too small (%d), using defaults\n", 
                         panelRotation.size());
        }
    } else {
        Serial.println("⚠ Missing or invalid 'panelRotation', using defaults");
    }
    
    // Load serpentine (with validation)
    if (doc.containsKey("serpentine") && doc["serpentine"].is<JsonArray>()) {
        JsonArray serpentine = doc["serpentine"];
        if (serpentine.size() >= NUM_PANELS) {
            for (int i = 0; i < NUM_PANELS; i++) {
                config.serpentine[i] = serpentine[i];
            }
        } else {
            Serial.printf("⚠ serpentine array too small (%d), using defaults\n", 
                         serpentine.size());
        }
    } else {
        Serial.println("⚠ Missing or invalid 'serpentine', using defaults");
    }
    
    // Load WLED-style layout settings
    if (doc.containsKey("startCorner")) {
        uint8_t corner = doc["startCorner"];
        if (corner <= 3) {
            config.startCorner = corner;
        } else {
            Serial.printf("⚠ Invalid startCorner: %d, using default: 0\n", corner);
        }
    }
    
    if (doc.containsKey("panelLayout")) {
        uint8_t layout = doc["panelLayout"];
        if (layout <= 1) {
            config.panelLayout = layout;
        } else {
            Serial.printf("⚠ Invalid panelLayout: %d, using default: 0\n", layout);
        }
    }
    
    if (doc.containsKey("panelSerpentine")) {
        config.panelSerpentine = doc["panelSerpentine"];
    }
    
    // Load animation settings (optional)
    if (doc.containsKey("defaultAnimation") && doc["defaultAnimation"].is<const char*>()) {
        defaultAnimationName = String((const char*)doc["defaultAnimation"]);
    }
    if (doc.containsKey("autoCycleMs")) {
        defaultAutoCycleMs = (uint32_t)doc["autoCycleMs"].as<uint32_t>();
    }
    if (doc.containsKey("fsAnimationPath") && doc["fsAnimationPath"].is<const char*>()) {
        defaultFsAnimationPath = String((const char*)doc["fsAnimationPath"]);
    }
    
    // Load LED hardware settings (optional)
    if (doc.containsKey("ledDataPin")) {
        uint8_t pin = doc["ledDataPin"];
        if (pin >= 0 && pin <= 48) {  // Valid GPIO range for ESP32-S3
            ledDataPin = pin;
        }
    }
    if (doc.containsKey("ledBrightness")) {
        uint8_t brightness = doc["ledBrightness"];
        if (brightness >= 1 && brightness <= 255) {
            ledBrightness = brightness;
        }
    }
    if (doc.containsKey("ledType") && doc["ledType"].is<const char*>()) {
        ledType = String((const char*)doc["ledType"]);
    }
    if (doc.containsKey("ledColorOrder") && doc["ledColorOrder"].is<const char*>()) {
        ledColorOrder = String((const char*)doc["ledColorOrder"]);
    }

    // Final validation
    if (!validateConfig(config)) {
        Serial.println("✗ Loaded config failed validation, using defaults");
        applyDefaults(config);
        return false;
    }
    
    Serial.println("✓ Configuration loaded and validated from file");
    return true;
}

bool ConfigManager::loadDefaultConfig(PanelConfig& config) {
    return loadConfigFromFile(DEFAULT_CONFIG_PATH, config);
}

bool ConfigManager::hasConfig() {
    preferences.begin(NAMESPACE, true);  // Read-only mode
    bool exists = preferences.isKey("matrixWidth");
    preferences.end();
    return exists;
}

bool ConfigManager::isFilesystemReady() {
    if (!LittleFS.begin()) {
        return false;
    }
    return true;
}

void ConfigManager::clearConfig() {
    preferences.begin(NAMESPACE, false);  // Read/write mode
    preferences.clear();
    preferences.end();
    Serial.println("✓ Configuration cleared");
}

String ConfigManager::exportConfigJSON(const PanelConfig& config) {
    JsonDocument doc;
    
    doc["matrixWidth"] = config.matrixWidth;
    doc["matrixHeight"] = config.matrixHeight;
    
    JsonArray panelOrder = doc["panelOrder"].to<JsonArray>();
    for (int i = 0; i < NUM_PANELS; i++) {
        panelOrder.add(config.panelOrder[i]);
    }
    
    JsonArray panelRotation = doc["panelRotation"].to<JsonArray>();
    for (int i = 0; i < NUM_PANELS; i++) {
        panelRotation.add(config.panelRotation[i]);
    }
    
    JsonArray serpentine = doc["serpentine"].to<JsonArray>();
    for (int i = 0; i < NUM_PANELS; i++) {
        serpentine.add(config.serpentine[i]);
    }
    
    // WLED-style layout
    doc["startCorner"] = config.startCorner;
    doc["panelLayout"] = config.panelLayout;
    doc["panelSerpentine"] = config.panelSerpentine;
    
    // Animation settings
    doc["defaultAnimation"] = defaultAnimationName;
    doc["autoCycleMs"] = defaultAutoCycleMs;
    doc["fsAnimationPath"] = defaultFsAnimationPath;
    
    // LED hardware settings
    doc["ledDataPin"] = ledDataPin;
    doc["ledBrightness"] = ledBrightness;
    doc["ledType"] = ledType;
    doc["ledColorOrder"] = ledColorOrder;
    
    String output;
    serializeJson(doc, output);
    return output;
}

bool ConfigManager::importConfigJSON(const String& json, PanelConfig& config) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) {
        Serial.print("✗ JSON parse error: ");
        Serial.println(error.c_str());
        return false;
    }
    
    config.matrixWidth = doc["matrixWidth"] | 2;
    config.matrixHeight = doc["matrixHeight"] | 2;
    
    JsonArray panelOrder = doc["panelOrder"];
    for (int i = 0; i < NUM_PANELS && i < panelOrder.size(); i++) {
        config.panelOrder[i] = panelOrder[i];
    }
    
    JsonArray panelRotation = doc["panelRotation"];
    for (int i = 0; i < NUM_PANELS && i < panelRotation.size(); i++) {
        config.panelRotation[i] = panelRotation[i];
    }
    
    JsonArray serpentine = doc["serpentine"];
    for (int i = 0; i < NUM_PANELS && i < serpentine.size(); i++) {
        config.serpentine[i] = serpentine[i];
    }
    
    Serial.println("✓ Configuration imported from JSON");
    return true;
}

void ConfigManager::printConfig(const PanelConfig& config) {
    Serial.println("\n=== Panel Configuration ===");
    Serial.printf("Matrix: %dx%d panels (%dx%d pixels)\n", 
                  config.matrixWidth, config.matrixHeight,
                  config.matrixWidth * PANEL_SIZE, config.matrixHeight * PANEL_SIZE);
    Serial.println();
    
    for (int i = 0; i < NUM_PANELS; i++) {
        Serial.printf("Panel %d:\n", i);
        Serial.printf("  Order:      %d\n", config.panelOrder[i]);
        Serial.printf("  Rotation:   %d°\n", config.panelRotation[i]);
        Serial.printf("  Serpentine: %s\n", config.serpentine[i] ? "Yes" : "No");
    }
    
    Serial.println("\nJSON Export:");
    Serial.println(exportConfigJSON(config));
    Serial.println("===========================\n");
}
