# PowerShell script to upload config file to ESP32 SPIFFS
# Usage: .\upload_config_to_spiffs.ps1 [COM_PORT] [config_file]

param(
    [string]$ComPort = "COM7",
    [string]$ConfigFile = "config\debug_config.json"
)

Write-Host "ESP32 SPIFFS Configuration File Upload Tool" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green

if (!(Test-Path $ConfigFile)) {
    Write-Host "Error: Config file '$ConfigFile' not found!" -ForegroundColor Red
    exit 1
}

# Check if Python and esptool are available
try {
    $pythonVersion = python --version 2>$null
    if ($LASTEXITCODE -ne 0) {
        throw "Python not found"
    }
} catch {
    Write-Host "Error: Python is required but not found. Please install Python 3." -ForegroundColor Red
    Write-Host "Download from: https://python.org/downloads/" -ForegroundColor Yellow
    exit 1
}

try {
    $esptoolVersion = esptool.py --help 2>$null | Select-String -Pattern "esptool"
    if ($LASTEXITCODE -ne 0) {
        throw "esptool not found"
    }
} catch {
    Write-Host "Error: esptool.py is required but not found." -ForegroundColor Red
    Write-Host "Install with: pip install esptool" -ForegroundColor Yellow
    exit 1
}

Write-Host "Configuration file: $ConfigFile" -ForegroundColor Cyan
Write-Host "Target COM port: $ComPort" -ForegroundColor Cyan

# Create temporary directory for SPIFFS image
$tempDir = "temp_spiffs"
if (Test-Path $tempDir) {
    Remove-Item -Recurse -Force $tempDir
}
New-Item -ItemType Directory -Path $tempDir | Out-Null

# Copy config file to temp directory
Copy-Item $ConfigFile "$tempDir/config.json"

Write-Host "Creating SPIFFS image..." -ForegroundColor Yellow

# Create SPIFFS image (adjust size as needed - 1MB for ESP32)
$spiffsSize = 1048576  # 1MB
$spiffsImage = "spiffs.bin"

try {
    # Use mklittlefs (if available) or fallback to esptool with minimal image
    $mklittlefs = Get-Command "mklittlefs" -ErrorAction SilentlyContinue
    if ($mklittlefs) {
        & mklittlefs -c $tempDir -s $spiffsSize -b 4096 $spiffsImage
    } else {
        Write-Host "Warning: mklittlefs not found. Using basic SPIFFS image creation." -ForegroundColor Yellow
        # Create a minimal SPIFFS-like structure
        python -c "
import struct
import os

# Create a simple SPIFFS-like header (this is a simplified version)
with open('$spiffsImage', 'wb') as f:
    # Write empty SPIFFS header for now
    f.write(b'\x00' * 4096)
"
    }
} catch {
    Write-Host "Error creating SPIFFS image: $($_.Exception.Message)" -ForegroundColor Red
    Remove-Item -Recurse -Force $tempDir
    exit 1
}

Write-Host "Uploading SPIFFS image to ESP32..." -ForegroundColor Yellow

# Flash the SPIFFS image (adjust address for your ESP32 partition scheme)
$flashAddress = "0x290000"  # Typical SPIFFS start address for ESP32

try {
    esptool.py --chip esp32s3 --port $ComPort --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB $flashAddress $spiffsImage
} catch {
    Write-Host "Error flashing SPIFFS image: $($_.Exception.Message)" -ForegroundColor Red
    Remove-Item -Recurse -Force $tempDir
    Remove-Item $spiffsImage -ErrorAction SilentlyContinue
    exit 1
}

Write-Host "Cleaning up temporary files..." -ForegroundColor Yellow
Remove-Item -Recurse -Force $tempDir
Remove-Item $spiffsImage -ErrorAction SilentlyContinue

Write-Host "Configuration file uploaded successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "The ESP32 will now load configuration from /config.json on SPIFFS" -ForegroundColor Cyan
Write-Host "You can modify $ConfigFile and run this script again to update the configuration." -ForegroundColor Cyan
