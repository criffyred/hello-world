#pragma once

#include <JuceHeader.h>

/**
 * Modulation System - Manages LFOs, envelopes, and random modulators
 *
 * Features (simplified for prototype):
 * - 4 modulator slots
 * - LFO, Random, and ADSR envelope types
 * - Can be mapped to any parameter (future enhancement)
 */
class ModulationSystem
{
public:
    ModulationSystem();

    void prepare(double sampleRate);
    void update(int numSamples, double sampleRate);

    // Get modulation values (0.0 to 1.0)
    float getLFO1() const { return lfo1Value; }
    float getLFO2() const { return lfo2Value; }
    float getRandom1() const { return random1Value; }
    float getRandom2() const { return random2Value; }

private:
    struct LFO
    {
        float phase = 0.0f;
        float frequency = 1.0f; // Hz
        float value = 0.0f;

        void update(double sampleRate, int numSamples)
        {
            float increment = frequency / static_cast<float>(sampleRate);
            phase += increment * static_cast<float>(numSamples);
            while (phase >= 1.0f)
                phase -= 1.0f;

            // Sine wave
            value = 0.5f + 0.5f * std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
        }
    };

    LFO lfo1, lfo2;
    juce::Random random;
    float random1Value = 0.5f;
    float random2Value = 0.5f;
    float lfo1Value = 0.0f;
    float lfo2Value = 0.0f;

    int sampleCounter = 0;
    int randomUpdateInterval = 44100; // Update random every second

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationSystem)
};
