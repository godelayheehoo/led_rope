#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <FastLED.h>

// Enable debug messages over Hardware Serial
#define DEBUG

// LED Rope Configuration
/**
 * @brief Number of physical LEDs on the strip.
 * Acceptable range: 1 to 300 (limited by ATmega328P RAM).
 */
const int NUM_LEDS = 100;

/**
 * @brief Digital pin on the Arduino connected to the LED data line.
 * Acceptable range: Valid Arduino digital output pins (typically 2-13).
 */
const int LED_PIN = 6;

// MIDI Configuration
/**
 * @brief Digital pin used as SoftwareSerial RX for incoming MIDI.
 * Acceptable range: Valid Arduino digital input pins supporting change
 * interrupts (typically 2 or 3).
 */
const int MIDI_RX_PIN = 3;

// LED Color Configuration
/**
 * @brief Color for standard beat pulses.
 */
const CRGB LED_COLOR_STANDARD = CRGB(0, 150, 255);

/**
 * @brief Color for the downbeat (Beat 1) pulse.
 */
const CRGB LED_COLOR_DOWNBEAT = CRGB(255, 0, 200);

/**
 * @brief Color for the Start (play) flash overlay.
 */
//
const CRGB LED_COLOR_PLAY_FLASH = CRGB::Green;

// Visualizer Parameters
/**
 * @brief Width/fade speed of the pulse trail (in LEDs).
 * A larger value creates a longer, more drawn-out comet tail.
 * Acceptable range: 0.5 to 10.0.
 */
const float TRAIL_DECAY = 6.0f;

/**
 * @brief Maximum number of simultaneous active pulses rendering on the strip.
 * Acceptable range: 1 to 16.
 */
const int MAX_PULSES = 8;

/**
 * @brief Number of LEDs to advance per MIDI clock pulse.
 */
const int LEDS_PER_PULSE = 1;

/**
 * @brief Time in milliseconds before the MIDI clock is considered
 * inactive/timed out. Acceptable range: 100 to 2000 ms.
 */
const unsigned long CLOCK_TIMEOUT = 500;

/**
 * @brief Fade-out rate of the Start overlay flash per frame.
 * A higher value causes the flash to disappear faster.
 * Acceptable range: 0.01 to 0.2.
 */
const float FLASH_DECAY_RATE = 0.05f;

/**
 * @brief Decay rate of active pulses per frame when playback is paused/stopped.
 * A higher value causes existing pulses to fade out faster.
 * Acceptable range: 0.01 to 0.1.
 */
const float PULSE_DECAY_RATE_STOPPED = 0.03f;

/**
 * @brief The target frame interval in milliseconds (e.g. 16ms = ~60 FPS).
 * Acceptable range: 8 to 50 ms.
 */
const unsigned long FRAME_INTERVAL = 16;

#endif // CONSTANTS_H
