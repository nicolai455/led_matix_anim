#include "AnimationManager.h"

AnimationManager::AnimationManager(MatrixOrientation* matrixPtr)
    : animationCount(0), currentIndex(-1), lastSwitchMs(0), autoCycleMs(0), matrix(matrixPtr) {
    for (uint8_t i = 0; i < MAX_ANIMATIONS; i++) {
        animations[i] = nullptr;
    }
}

void AnimationManager::setAutoCycle(uint32_t intervalMs) {
    autoCycleMs = intervalMs;
}

bool AnimationManager::registerAnimation(Animation* animation) {
    if (animationCount >= MAX_ANIMATIONS) return false;
    animations[animationCount++] = animation;
    return true;
}

uint8_t AnimationManager::getCount() const { return animationCount; }

bool AnimationManager::switchTo(uint8_t index) {
    if (index >= animationCount) return false;
    currentIndex = index;
    animations[currentIndex]->setup();
    lastSwitchMs = millis();
    return true;
}

bool AnimationManager::switchToByName(const char* name) {
    for (uint8_t i = 0; i < animationCount; i++) {
        if (strcmp(animations[i]->getName(), name) == 0) {
            return switchTo(i);
        }
    }
    return false;
}

int8_t AnimationManager::getCurrentIndex() const { return currentIndex; }

const char* AnimationManager::getCurrentName() const {
    if (currentIndex < 0 || currentIndex >= animationCount) return "";
    return animations[currentIndex]->getName();
}

void AnimationManager::setup() {
    if (animationCount > 0 && currentIndex < 0) {
        uint8_t firstIndex = 0;
        switchTo(firstIndex);
    }
}

void AnimationManager::loop(CRGB* leds) {
    if (animationCount == 0 || currentIndex < 0 || !matrix) return;

    // Auto cycle if enabled
    if (autoCycleMs > 0) {
        unsigned long now = millis();
        if (now - lastSwitchMs >= autoCycleMs) {
            uint8_t next = (currentIndex + 1) % animationCount;
            switchTo(next);
        }
    }

    // Render animation into 2D frame buffer
    animations[currentIndex]->renderFrame(frameBuffer, millis());

    // Transform 2D logical coordinates to physical LED indices
    matrix->render(frameBuffer, leds);
}


