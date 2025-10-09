#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "MatrixOrientation.h"

class ConfigManager {
private:
    Preferences preferences;
    const char* NAMESPACE = "led_matrix";
    const char* DEFAULT_CONFIG_PATH = "/config/panel_config.json";
    
    // Validation helpers
    bool validateConfig(const PanelConfig& config);
    void applyDefaults(PanelConfig& config);
    
public:
    ConfigManager();
    
    // Setup and load configuration with automatic fallback
    // Priority: 1. NVS, 2. File, 3. Defaults
    // Returns the loaded configuration (always valid)
    PanelConfig setup();
    
    // Save configuration to NVS
    bool savePanelConfig(const PanelConfig& config);
    
    // Load configuration from NVS
    bool loadPanelConfig(PanelConfig& config);
    
    // Load configuration from file (with validation and fallback)
    bool loadConfigFromFile(const char* path, PanelConfig& config);
    
    // Load default configuration from /config/panel_config.json
    bool loadDefaultConfig(PanelConfig& config);
    
    // Check if config exists in NVS
    bool hasConfig();
    
    // Check if filesystem is mounted
    bool isFilesystemReady();
    
    // Clear all configuration
    void clearConfig();
    
    // Export config as JSON string (for debugging/backup)
    String exportConfigJSON(const PanelConfig& config);
    
    // Import config from JSON string
    bool importConfigJSON(const String& json, PanelConfig& config);
    
    // Print current config to Serial
    void printConfig(const PanelConfig& config);
};

#endif // CONFIG_MANAGER_H
