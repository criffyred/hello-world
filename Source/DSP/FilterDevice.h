#pragma once

#include <JuceHeader.h>

/**
 * Filter Device (Ring) - Morphing resonant filter bank
 *
 * Features:
 * - 48-band tuned resonant filters (simplified to multi-mode filter)
 * - Morph between lowpass, bandpass, highpass
 * - Resonance/decay control
 * - Pitch shifting capability
 */
class FilterDevice
{
public:
    FilterDevice();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters);

private:
    juce::dsp::StateVariableTPTFilter<float> filterLeft;
    juce::dsp::StateVariableTPTFilter<float> filterRight;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterDevice)
};
