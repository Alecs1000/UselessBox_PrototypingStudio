// ===== NeoPixel (single RGB LED) =====
// One little LED that works as the king's mood ring: solid color when he reacts,

#include "Config.h" //Change color and other attributes from the Config

// Paint the LED a solid color and remember it (the blinker reuses it).
void setColor(uint32_t c) {
  ledColor = c;
  ledOn = true;
  for (int i = 0; i < NUM_PIXELS; i++) strip.setPixelColor(i, c);
  strip.show();
}

void ledOff() {
  ledOn = false;
  strip.setBrightness(255);   // reset so the next color starts at full
  strip.clear();
  strip.show();
}

// Repaint the current color at a given brightness (0..255) - lets the light
// "breathe" with the sensor during the tantrum mind-games.
void setBrightnessNow(uint8_t b) {
  strip.setBrightness(b);
  for (int i = 0; i < NUM_PIXELS; i++) strip.setPixelColor(i, ledColor);
  strip.show();
}

// Start a non-blocking blink at the given interval (ms).
void startBlink(long intervalMs) {
  ledBlinking   = true;
  ledBlinkDelay = intervalMs;
  lastBlink     = millis();
}

void stopBlink() {
  ledBlinking = false;
}

// Call this often. It toggles the LED every ledBlinkDelay ms without using delay(),
// so the rest of the box can keep doing its thing while the light flashes.
void updateBlink() {
  if (!ledBlinking) return;
  if (millis() - lastBlink < ledBlinkDelay) return;

  lastBlink = millis();
  ledOn = !ledOn;
  if (ledOn) {
    for (int i = 0; i < NUM_PIXELS; i++) strip.setPixelColor(i, ledColor);
  } else {
    strip.clear();
  }
  strip.show();
}
