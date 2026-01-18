#pragma once

#include <JuceHeader.h>

/**
 * Filter Device - 48-band resonant filter bank (Ring mode)
 *
 * Features:
 * - Ring mode: 48-band resonant filter bank for spectral filtering/mangling
 * - Morph: Transitions from lowpass -> bandpass -> highpass character
 * - Resonance: Controls Q factor of filter bands
 * - Decay: Affects resonance behavior and band distribution
 * - Bypass mode: Pass audio through unprocessed
 *
 * This creates the signature S-4 sound mangling through spectral filtering
 */
class FilterDevice
{
public:
    FilterDevice();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters);

private:
    static constexpr int NUM_BANDS = 48;

    // 48-band filter bank (one set per channel for stereo)
    struct FilterBankChannel
    {
        std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_BANDS> bands;
    };

    std::array<FilterBankChannel, 2> filterBanks; // Stereo

    double currentSampleRate = 44100.0;

    // Calculate frequency for each band (logarithmically spaced from 20Hz to 20kHz)
    float getBandFrequency(int bandIndex);

    // Calculate band gain based on morph parameter (LP/BP/HP transition)
    float getBandGain(int bandIndex, float morph, float frequency, float centerFreq);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterDevice)
};
