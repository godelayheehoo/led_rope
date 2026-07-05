#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <FastLED.h>

// Enable debug messages over Hardware Serial
#define DEBUG

// LED Rope Configuration
const int NUM_LEDS = 100;
const int LED_PIN = 6;

// MIDI Configuration
const int MIDI_RX_PIN = 2;

// LED Color Configuration
const CRGB LED_COLOR_STANDARD = CRGB(0, 150, 255);
const CRGB LED_COLOR_DOWNBEAT = CRGB(255, 255, 255);

// Note Pulse Configuration
const int MAX_NOTE_PULSES = 24;
const uint8_t NOTE_PULSE_BRIGHTNESS_SCALE = 85; // 1/3rd of 255
const float NOTE_PULSE_DECAY_RATE_STOPPED = 0.03f;

#endif // CONSTANTS_H
