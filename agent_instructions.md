#Arduino Nano MIDI Clock LED Pulse Visualizer

Create firmware for an Arduino Nano that drives a WS2812/WS2812B LED strip using the FastLED library. The project should visualize MIDI clock by displaying glowing pulses that travel outward from the center of the strip.

## Existing Project

Assume there is a constants.h file containing at least:

- NUM_LEDS
- LED_PIN

Use these values rather than hardcoding them.

## MIDI Input

The Arduino receives standard DIN MIDI.

Handle the following real-time messages:

- MIDI Clock (0xF8)
- MIDI Start (0xFA)
- MIDI Stop (0xFC)
- MIDI Continue (0xFB)

Ignore all other MIDI messages.

## Animation

Treat LED 0 as the starting point. 

Every quarter note (24 MIDI clock messages), create a pulse at starting point.

Each pulse should travel in one direction (positive index direction).

Pulses should have:

- a bright head
- a smooth fading trail
- additive blending where pulses overlap
- sub-pixel positioning using floating point coordinates for smooth movement

The animation should not jump from LED to LED.

On a play start, the entire strip should pulse green once briefly.  When a pause is detected, the entire strip should pulse red once briefly.  On a play continue message, resume the animation.

Keep track of the start of each beat, assuming 24ppq.  The on beat pulse should be brighter than the other beat pulses.


## Timing

Do not estimate BPM.

Drive the animation directly from incoming MIDI clock.

Assume 4/4 time.

A pulse launched on beat 1 should reach the end of the strip exactly one measure later (96 MIDI clocks).

Movement should therefore be computed as a fixed distance per MIDI clock rather than using elapsed milliseconds.

Rendering

Use FastLED.

Each frame:

- fade previous LEDs
- render all active pulses
- show the strip

Use floating point pulse positions and interpolate brightness between neighboring LEDs for smooth motion.

Pulse Management

Maintain a small collection of active pulses.

Each pulse should contain at least:

- position
- direction
- brightness
- active flag

Automatically deactivate pulses once they leave the strip.

## MIDI Transport

When receiving MIDI Start:

- clear existing pulses
- reset the clock counter
- begin animating

When receiving MIDI Stop:

- stop launching new pulses
- allow existing pulses to fade naturally

When receiving MIDI Continue:

- resume launching pulses from the current clock count.Code Organization

Keep the code clean and modular.

Suggested functions include:

- handleMidiByte()
- spawnPulse()
- updatePulses()
- renderPulses()
- clearFinishedPulses()

Avoid global state except where appropriate for hardware interfaces and animation state.

Prefer readable code over clever optimizations.

The resulting firmware should be lightweight, deterministic, and easy to extend with additional visual effects later.