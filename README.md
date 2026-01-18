# Torso S-4 VST Plugin

A VST3 plugin that mimics the Torso Electronics S-4 Sculpting Sampler for use in Ableton Live and other DAWs.

## Overview

The Torso S-4 is a professional sculpting sampler with 4 parallel stereo tracks, each containing a chain of 5 audio devices for comprehensive sound manipulation. This plugin implements a single-track prototype with all 5 audio devices in series.

## Features

### Device Chain (Signal Flow)

Each track processes audio through 5 devices in series:

1. **MATERIAL** - Tape recorder/looping OR polyphonic sampler
   - Tape mode: Live recording, looping, overdubbing (6 minutes max)
   - Poly mode: 8-voice polyphonic sampler with MIDI control
   - ADSR envelope controls for poly mode
   - Gain control

2. **GRANULAR (Mosaic)** - Grain sculpting processor
   - Grain size: 1-500ms
   - Grain density: 1-128 grains
   - Pitch shifting: ±36 semitones
   - Grain spread/randomization
   - 4-second circular buffer
   - Dry/wet mix control

3. **FILTER (Ring)** - Morphing resonant filter
   - Frequency control: 20Hz - 20kHz
   - Resonance/Q control
   - Filter morph: Lowpass → Bandpass → Highpass
   - Decay control for resonance behavior

4. **DISTORTION (Deform)** - Multi-mode color processor
   - Drive/saturation
   - Bit crushing (16-bit to 1-bit)
   - Compression
   - Noise generation
   - Tilt EQ (balance low/high frequencies)

5. **SPACE (Vast)** - Combined reverb and delay
   - Stereo delay with feedback (0-2000ms)
   - Reverb with size and damping controls
   - Freeze mode for infinite reverb
   - Dry/wet mix control

### Modulation System

Basic modulation system with 4 modulator slots:
- 2 LFOs (0.5 Hz and 2 Hz)
- 2 Random modulators

*Note: Parameter mapping for modulators is planned for future development.*

## Building the Plugin

### Prerequisites

- CMake 3.22 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)
- Git

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd hello-world

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release

# The VST3 plugin will be in:
# - Windows: build/TorsoS4_artefacts/Release/VST3/
# - macOS: build/TorsoS4_artefacts/Release/VST3/
# - Linux: build/TorsoS4_artefacts/VST3/
```

### Installing the Plugin

**Ableton Live (Windows):**
```
Copy the VST3 to: C:\Program Files\Common Files\VST3\
```

**Ableton Live (macOS):**
```
Copy the VST3 to: /Library/Audio/Plug-Ins/VST3/
or: ~/Library/Audio/Plug-Ins/VST3/
```

**Ableton Live (Linux):**
```
Copy the VST3 to: ~/.vst3/
```

## Usage

1. Load the plugin in your DAW as an audio effect or instrument
2. In **Material** mode:
   - **Tape mode**: Processes incoming audio with looping
   - **Poly mode**: Responds to MIDI input with 8-voice polyphony
3. Adjust each device's parameters to sculpt your sound
4. The signal flows through all 5 devices in series: Material → Granular → Filter → Distortion → Space
5. Use the **Master Volume** to control the final output level

## Technical Specifications

- **Sample Rate**: 24-bit / 48kHz (or host sample rate)
- **Channels**: Stereo I/O
- **Format**: VST3, Standalone
- **MIDI**: Input supported for poly mode
- **Latency**: Minimal (buffer-dependent)

## Architecture

The plugin is built using:
- **JUCE Framework** (v7.0.12) - Cross-platform audio framework
- **CMake** - Build system
- **C++17** - Programming language

### Project Structure

```
hello-world/
├── CMakeLists.txt              # Build configuration
├── Source/
│   ├── PluginProcessor.h/cpp   # Main audio processor
│   ├── PluginEditor.h/cpp      # GUI interface
│   └── DSP/                    # DSP modules
│       ├── MaterialDevice.h/cpp
│       ├── GranularDevice.h/cpp
│       ├── FilterDevice.h/cpp
│       ├── DistortionDevice.h/cpp
│       ├── SpaceDevice.h/cpp
│       └── ModulationSystem.h/cpp
└── README.md
```

## Roadmap

### Current Prototype Features ✅
- Single track with all 5 devices
- Basic modulation system
- All core DSP implementations
- Full parameter control via GUI

### Planned Enhancements 🚧
- [ ] 4 parallel tracks (full S-4 architecture)
- [ ] Modulator-to-parameter mapping system
- [ ] Scene storage and recall (128 scenes)
- [ ] Performance macros (12 mappable macros)
- [ ] Sample loading and management
- [ ] Advanced tape recording features
- [ ] MIDI CC mapping
- [ ] Preset management
- [ ] Visual feedback and metering
- [ ] Euclidean pattern generator (inspired by Torso T-1)

## Credits

Inspired by the **Torso Electronics S-4 Sculpting Sampler**.

This is an independent, unofficial recreation and is not affiliated with or endorsed by Torso Electronics.

## License

This project is provided as-is for educational and creative purposes.

## Resources

- [Torso Electronics Official Website](https://torsoelectronics.com/)
- [Sound on Sound S-4 Review](https://www.soundonsound.com/reviews/torso-electronics-s-4)
- [JUCE Framework](https://juce.com/)

---

**Note**: This is a single-track prototype. The full Torso S-4 hardware features 4 independent tracks with cross-modulation, scene management, and more advanced features that may be implemented in future versions.
