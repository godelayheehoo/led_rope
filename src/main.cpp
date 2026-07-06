#include "constants.h"
#include <Arduino.h>
#include <FastLED.h>
#include <SoftwareSerial.h>

#ifndef DEBUG
#define DEBUG
#endif

// Define the SoftwareSerial interface for MIDI RX.
// MIDI standard uses 31250 baud.
// We configure TX to -1 (unused) because we only receive MIDI input.
SoftwareSerial midiSerial(MIDI_RX_PIN, -1);

// LED Strip buffer
CRGB leds[NUM_LEDS];

// Pulse data structure representing a travelling light pulse
struct Pulse {
  float position;
  float direction;  // 1.0f for positive index direction
  float brightness; // Intensity factor [0.0, 1.0]
  bool isDownbeat;  // True if spawned on Beat 1 (downbeat)
  bool active;      // True if the pulse is currently rendering
};

// Pulse management
const int MAX_PULSES = 8;
Pulse pulses[MAX_PULSES];

// Animation speed: a pulse launched on beat 1 reaches the end in one measure
// (96 MIDI clocks)
const float SPEED_PER_CLOCK = static_cast<float>(NUM_LEDS) / 96.0f;
const float TRAIL_DECAY = 3.0f; // Width/fade speed of the pulse trail (in LEDs)

// Playback and timing state
volatile uint32_t clockCount = 0;
volatile unsigned long lastClockTime = 0;
const unsigned long CLOCK_TIMEOUT = 500; // ms
bool isPlaying = false;

// Flash overlay state (Brief green pulse on Start, red pulse on Stop)
float flashIntensity = 0.0f;
CRGB flashColor = CRGB::Black;
const float FLASH_DECAY_RATE = 0.05f; // Fades out in ~20 frames
const float PULSE_DECAY_RATE_STOPPED =
    0.03f; // Natural decay rate of active pulses when paused

// Timing for the continuous rendering loop
unsigned long lastFrameTime = 0;
const unsigned long FRAME_INTERVAL = 16; // ~60 FPS (16ms per frame)

// Spawn a new pulse on the strip
void spawnPulse(bool isDownbeat) {
  for (int i = 0; i < MAX_PULSES; i++) {
    if (!pulses[i].active) {
      pulses[i].position = 0.0f;
      pulses[i].direction = 1.0f;
      pulses[i].brightness = isDownbeat ? 1.0f : 0.5f; // Downbeat is brighter
      pulses[i].isDownbeat = isDownbeat;
      pulses[i].active = true;
      break;
    }
  }
}

// Move active pulses forward
void updatePulses() {
  for (int i = 0; i < MAX_PULSES; i++) {
    if (pulses[i].active) {
      pulses[i].position += SPEED_PER_CLOCK * pulses[i].direction;
      if (pulses[i].position >= NUM_LEDS) {
        pulses[i].active = false;
      }
    }
  }
}

// Render pulses and flash overlays onto the strip
void renderPulses() {
  // 1. Calculate the background flash color if any
  CRGB baseColor = CRGB::Black;
  if (flashIntensity > 0.0f) {
    baseColor = flashColor;
    baseColor.nscale8(static_cast<uint8_t>(flashIntensity * 255.0f));
  }

  // 2. Set all LEDs to the base color
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = baseColor;
  }

  // 3. Render active pulses additively
  for (int pIdx = 0; pIdx < MAX_PULSES; pIdx++) {
    if (!pulses[pIdx].active)
      continue;

    float p = pulses[pIdx].position;
    float brightness = pulses[pIdx].brightness;

    // Define colors: downbeat is bright white, standard beats are blue/cyan
    CRGB pulseColor;
    if (pulses[pIdx].isDownbeat) {
      pulseColor = LED_COLOR_DOWNBEAT; // Downbeat accent
    } else {
      pulseColor = LED_COLOR_STANDARD; // Standard beat visualizer
    }

    // Scale the pulse base color by the current pulse brightness
    pulseColor.nscale8(static_cast<uint8_t>(brightness * 255.0f));

    // Render pulse onto the strip using a sub-pixel profile:
    // Head has a linear fade in from +1 LED, trail has an exponential decay
    // behind
    int startLED = static_cast<int>(p) - 15;
    int endLED = static_cast<int>(p) + 2;

    if (startLED < 0)
      startLED = 0;
    if (endLED > NUM_LEDS)
      endLED = NUM_LEDS;

    for (int i = startLED; i < endLED; i++) {
      float dist = p - static_cast<float>(i);
      float factor = 0.0f;

      if (dist < -1.0f) {
        factor = 0.0f;
      } else if (dist >= -1.0f && dist < 0.0f) {
        // Smooth linear rise to the head
        factor = 1.0f + dist;
      } else {
        // Exponential decay trail
        factor = exp(-dist / TRAIL_DECAY);
      }

      if (factor > 0.01f) {
        CRGB contrib = pulseColor;
        contrib.nscale8(static_cast<uint8_t>(factor * 255.0f));
        leds[i] += contrib; // Additive blending
      }
    }
  }
}

// Process incoming MIDI bytes and drive playback and visual changes
void handleMidiByte(uint8_t byte) {
  if (byte == 0xF8) { // MIDI Timing Clock
    isPlaying = true;
    lastClockTime = millis();
    updatePulses();

    // Every 24 clocks is a quarter note beat
    if (clockCount % 24 == 0) {
      bool isDownbeat = (clockCount == 0);
      spawnPulse(isDownbeat);
    }

    clockCount++;
    if (clockCount >= 96) {
      clockCount = 0;
    }
  } else if (byte == 0xFA) { // MIDI Start
    clockCount = 0;

    // Clear existing active pulses
    for (int i = 0; i < MAX_PULSES; i++) {
      pulses[i].active = false;
    }

    // Trigger green flash overlay
    flashColor = CRGB::Green;
    flashIntensity = 1.0f;
    isPlaying = true;
  } else if (byte == 0xFC) { // MIDI Stop
    if (isPlaying) {
      // Trigger red flash overlay
      flashColor = CRGB::Red;
      flashIntensity = 1.0f;
      isPlaying = false;
    }
  } else if (byte == 0xFB) { // MIDI Continue
    // Resume overlay or state if needed
    isPlaying = true;
  }
}

void setup() {
  // Initialize the FastLED strip config
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

#ifdef DEBUG
  // Flash the entire LED strip 3 times at startup (orange flash)
  for (int flash = 0; flash < 3; flash++) {
    fill_solid(leds, NUM_LEDS, CRGB(255, 100, 0));
    FastLED.show();
    delay(200);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(200);
  }
#endif

  // Initialize the MIDI serial input
  midiSerial.begin(31250);
  pinMode(MIDI_RX_PIN, INPUT_PULLUP);

  // Clear all pulses initially
  for (int i = 0; i < MAX_PULSES; i++) {
    pulses[i].active = false;
  }
}

void loop() {
  // 1. Read and parse incoming MIDI data as fast as possible
  while (midiSerial.available() > 0) {
    uint8_t byte = midiSerial.read();
    handleMidiByte(byte);
  }

  // 2. Render and refresh the visualizer at a fixed frame rate (~60 FPS)
  unsigned long currentMillis = millis();
  if (currentMillis - lastFrameTime >= FRAME_INTERVAL) {
    lastFrameTime = currentMillis;

    // Decay the flash overlay intensity
    if (flashIntensity > 0.0f) {
      flashIntensity -= FLASH_DECAY_RATE;
      if (flashIntensity < 0.0f) {
        flashIntensity = 0.0f;
      }
    }

    // If the clock has stopped (timeout), decay active pulses so they fade naturally
    bool isClockActive = (millis() - lastClockTime < CLOCK_TIMEOUT);
    if (!isClockActive) {
      for (int i = 0; i < MAX_PULSES; i++) {
        if (pulses[i].active) {
          pulses[i].brightness -= PULSE_DECAY_RATE_STOPPED;
          if (pulses[i].brightness <= 0.0f) {
            pulses[i].active = false;
          }
        }
      }
    }

    // Render pulses and update the LED strip
    renderPulses();
    FastLED.show();
  }
}
