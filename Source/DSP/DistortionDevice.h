#pragma once

#include <JuceHeader.h>

/**
 * Distortion Device (Deform) - Multi-mode color processor
 *
 * Features:
 * - Drive/distortion
 * - Bit crushing/reduction
 * - Compression
 * - Noise generation
 * - Tilt EQ (balance between low/high frequencies)
 */
class DistortionDevice
{
public:
    DistortionDevice();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters);

private:
    juce::dsp::WaveShaper<float> waveshaper;
    juce::dsp::Compressor<float> compressor;
    juce::Random random;

    // Tilt EQ filters
    juce::dsp::IIR::Filter<float> tiltLowShelfLeft, tiltLowShelfRight;
    juce::dsp::IIR::Filter<float> tiltHighShelfLeft, tiltHighShelfRight;

    double currentSampleRate = 44100.0;

    float applyBitCrush(float sample, float bits);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionDevice)
};
