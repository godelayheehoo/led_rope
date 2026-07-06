# Arduino Nano MIDI LED Pulse Visualizer

An interactive LED visualizer for the Arduino Nano that drives a WS2812/WS2812B LED strip using the FastLED library. The visualizer reacts dynamically to MIDI Timing Clock and MIDI Note messages, propagating pulses outward from the start of the LED strip.

## Features

### 1. MIDI Clock Pulse Visualizer
- **Clock-Driven Motion**: Drives animation speed directly from incoming MIDI Clock (`0xF8`) bytes (using a fixed distance per clock based on 24ppq, assuming 4/4 time). A clock pulse launched on Beat 1 travels the entire strip length in exactly one measure (96 clock messages).
- **Sub-pixel Interpolation**: Floating-point pulse positions interpolate brightness between neighboring LEDs for smooth transitions.
- **Comet Trails**: Clock pulses are rendered with a bright head, linear fade-in, and a smooth exponential decay trail.
- **Downbeat Accentuation**: Beat 1 pulses are rendered in bright white, while standard beats are rendered in blue/cyan.
- **Transport Controls**: 
  - `MIDI Start` (`0xFA`): Resets the counter, clears active pulses, and flashes the strip green.
  - `MIDI Stop` (`0xFC`): Pauses new pulses, allowing existing ones to fade out naturally, and flashes the strip red.
  - `MIDI Continue` (`0xFB`): Resumes pulse propagation from the current clock count.

### 2. MIDI Note Pulse Visualizer
- **Channel-Blind Note On**: Spawns a note pulse whenever a `Note On` message (status `0x90`–`0x9F` with velocity > 0) is received, ignoring the MIDI channel.
- **12-Tone Color Wheel**: Maps the note's semitone (`note % 12`) to a distinct color on the HSV color wheel.
- **Chord Combination**: Multiple notes played together (chords) overlap and mix additively (capped at maximum brightness).
- **One LED Width**: Note pulses are rendered strictly on a single LED at a time (rounded to the nearest integer index) with no trailing comet decay.
- **Precedence Rule**: Note pulses take priority over clock pulses. If a note pulse occupies a specific LED, it overrides the clock pulse and background colors for that LED.

### 3. Natural Decay
- If the MIDI clock stops (detected via a 500ms timeout), active clock and note pulses fade out naturally rather than freezing.

---

## Hardware Pinout & Wiring

- **Arduino Nano**
- **WS2812B LED Strip**: Data line connected to pin defined by `LED_PIN`.
- **MIDI IN Circuit (Optocoupler)**: MIDI RX connected to pin defined by `MIDI_RX_PIN` via SoftwareSerial.

---

## Configuration

Customizations and pinouts are located in [include/constants.h](file:///Users/james/projects/led_rope/include/constants.h):

```cpp
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
```

---

## Getting Started

This project is built using [PlatformIO](https://platformio.org/).

### Compile & Upload

1. Install PlatformIO Core or the PlatformIO IDE extension in VS Code.
2. Connect your Arduino Nano.
3. Run the following command in the root directory to compile and upload the firmware:
   ```bash
   pio run --target upload
   ```
