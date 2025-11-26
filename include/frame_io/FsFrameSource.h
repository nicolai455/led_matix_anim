#ifndef FS_FRAME_SOURCE_H
#define FS_FRAME_SOURCE_H

#include <Arduino.h>
#include <FastLED.h>
#include <LittleFS.h>
#include "IFrameSource.h"

struct LfxHeader {
    char magic[4];      // "LFX1"
    uint16_t width;     // 32
    uint16_t height;    // 32
    uint16_t frames;    // number of frames
    uint8_t format;     // 0 = RGB888
} __attribute__((packed));

class FsFrameSource : public IFrameSource {
private:
    String path;
    LfxHeader header;
    bool valid;

public:
    explicit FsFrameSource(const char* filePath) : path(filePath), valid(false) {
        memset(&header, 0, sizeof(header));
        if (!LittleFS.begin()) return;
        if (!LittleFS.exists(path)) return;
        File f = LittleFS.open(path, "r");
        if (!f) return;
        if (f.readBytes((char*)&header, sizeof(header)) != sizeof(header)) { f.close(); return; }
        if (strncmp(header.magic, "LFX1", 4) != 0) { f.close(); return; }
        if (header.width != 32 || header.height != 32) { f.close(); return; }
        if (header.format != 0) { f.close(); return; }
        valid = true;
        f.close();
    }

    bool isValid() const { return valid; }

    uint16_t getFrameCount() const override { return valid ? header.frames : 0; }

    void getFrameInto(uint16_t frameIndex, CRGB* ledsOut) override {
        if (!valid) return;
        File f = LittleFS.open(path, "r");
        if (!f) return;
        size_t frameSize = 32 * 32 * 3;
        size_t offset = sizeof(LfxHeader) + (size_t)frameIndex % header.frames * frameSize;
        f.seek(offset, SeekSet);
        // Read raw RGB888 and convert to CRGB
        static uint8_t buf[1024 * 3];
        size_t n = f.read(buf, frameSize);
        f.close();
        if (n == frameSize) {
            for (int i = 0; i < 1024; i++) {
                ledsOut[i] = CRGB(buf[i*3 + 0], buf[i*3 + 1], buf[i*3 + 2]);
            }
        }
    }
};

#endif // FS_FRAME_SOURCE_H


