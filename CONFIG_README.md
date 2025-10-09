# LED Matrix Configuration System

This project includes a comprehensive configuration system that allows you to easily modify settings without recompiling the code.

## Configuration Files

### Runtime Configuration (NVS)
Settings are stored in the ESP32's non-volatile storage (NVS) and persist across reboots.

### Debug Configuration File (SPIFFS)
A JSON configuration file can be stored in the ESP32's SPIFFS filesystem for easy debugging and deployment.

## File Structure

```
led-matrix/
├── config/
│   └── debug_config.json          # Main configuration file
├── src/
│   ├── Config.cpp                 # Configuration class implementation
│   ├── MatrixOrientation.cpp      # Matrix orientation library
│   └── main.cpp                   # Main application
├── include/
│   ├── Config.h                   # Configuration class header
│   └── MatrixOrientation.h        # Matrix orientation header
└── tools/
    └── upload_config_to_spiffs.ps1 # Upload script for Windows
```

## Configuration File Format

The configuration file uses JSON format with the following structure:

```json
{
  "hardware": {
    "data_pin": 8,
    "led_type": 0,
    "color_order": 0,
    "brightness": 64
  },
  "display": {
    "panel_size": 16,
    "panels_across": 2,
    "panels_down": 2
  },
  "test_pattern": {
    "corner_marker_length": 5,
    "diagonal_length": 6
  },
  "timing": {
    "setup_timeout_ms": 5000,
    "loop_delay_ms": 1000
  },
  "matrix_orientation": {
    "panel_mapping": [0, 1, 2, 3],
    "panel_rotations": [0, 0, 0, 0]
  },
  "system": {
    "auto_save": true,
    "debug_mode": true
  }
}
```

### Configuration Categories

- **hardware**: LED strip configuration (pin, type, color order, brightness)
- **display**: Panel layout configuration (size, arrangement)
- **test_pattern**: Test pattern settings (marker lengths)
- **timing**: Timing-related settings (timeouts, delays)
- **matrix_orientation**: Panel mapping and rotation settings
- **system**: System-wide settings (auto-save, debug mode)

## Usage

### 1. Edit Configuration File

Modify `config/debug_config.json` with your desired settings:

```bash
# Edit the JSON file
code config/debug_config.json
# or
nano config/debug_config.json
```

### 2. Upload Configuration to ESP32

Use the provided PowerShell script to upload the configuration file:

```powershell
# Upload configuration file to SPIFFS
.\tools\upload_config_to_spiffs.ps1
```

Or manually using esptool:

```bash
# Upload the config file to SPIFFS
python -m esptool --chip esp32s3 --port COM7 --baud 460800 \
  --before default_reset --after hard_reset \
  write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB \
  0x290000 spiffs.bin
```

### 3. Flash the Firmware

Build and flash the firmware as usual:

```bash
platformio run --target upload
```

## Runtime Configuration

### Interactive Configuration Mode

When the ESP32 boots, you can enter configuration modes:

1. **Setup Mode** (type `setup` within 5 seconds):
   - Configure matrix orientation (panel mapping and rotations)
   - Commands: `map`, `rot`, `show`, `apply`, `save`, `exit`

2. **Config Mode** (type `config` within 5 seconds):
   - Modify any configuration parameter
   - Commands: `set`, `get`, `show`, `save`, `reset`, `list`, `help`

### Configuration Commands

#### Setup Mode Commands
```
map a b c d     - Set panel mapping (TL, TR, BL, BR)
rot r0 r1 r2 r3 - Set panel rotations (0, 90, 180, 270)
show            - Display current configuration
apply           - Apply changes and show test pattern
save            - Save configuration to NVS
exit            - Exit setup mode
```

#### Config Mode Commands
```
set <param> <value>  - Set a parameter value
get <param>          - Get a parameter value
show [category]      - Show all or category-specific config
save                 - Save configuration to NVS
reset                - Reset to defaults
list                 - List all parameters
help                 - Show help
exit                 - Exit configuration mode
```

## Configuration Parameters

### Hardware Parameters
- `data_pin`: GPIO pin for LED data (0-39)
- `led_type`: LED strip type (0=WS2812B, 1=WS2813)
- `color_order`: Color order (0=GRB, 1=RGB)
- `brightness`: LED brightness (0-255)

### Display Parameters
- `panel_size`: Size of each panel in pixels (8-32)
- `panels_across`: Number of panels horizontally (1-4)
- `panels_down`: Number of panels vertically (1-4)

### Test Pattern Parameters
- `corner_marker_length`: Length of corner L markers (1-10)
- `diagonal_length`: Length of 45-degree diagonals (1-10)

### Timing Parameters
- `setup_timeout_ms`: Setup mode timeout in milliseconds (1000-30000)
- `loop_delay_ms`: Main loop delay in milliseconds (10-10000)

### System Parameters
- `auto_save`: Automatically save configuration changes (true/false)

## Best Practices

1. **Start with the debug config file** for initial setup
2. **Use NVS for permanent settings** that should persist across reboots
3. **Use the config file for debugging** and easy deployment
4. **Test configuration changes** in setup mode before saving
5. **Backup your configuration** by copying the JSON file

## Troubleshooting

### Configuration Not Loading
- Check that SPIFFS is properly mounted
- Verify the config file exists at `/config.json` in SPIFFS
- Check JSON syntax in the configuration file

### Setup Mode Issues
- Ensure you're typing commands within the timeout period
- Check that parameter values are within valid ranges
- Use `show` command to verify current configuration

### SPIFFS Upload Issues
- Ensure the ESP32 is in flash mode (GPIO0 pulled low)
- Check COM port and baud rate settings
- Verify esptool.py is installed (`pip install esptool`)

## Examples

### Example 1: Configure for Different Panel Size
```json
{
  "display": {
    "panel_size": 32,
    "panels_across": 1,
    "panels_down": 1
  }
}
```

### Example 2: High Brightness Setup
```json
{
  "hardware": {
    "brightness": 128
  },
  "timing": {
    "loop_delay_ms": 500
  }
}
```

### Example 3: Custom Panel Orientation
```json
{
  "matrix_orientation": {
    "panel_mapping": [0, 3, 1, 2],
    "panel_rotations": [0, 180, 90, 270]
  }
}
```
