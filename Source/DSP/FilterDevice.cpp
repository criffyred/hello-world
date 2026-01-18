#include "FilterDevice.h"

FilterDevice::FilterDevice()
{
}

void FilterDevice::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    filterLeft.prepare(monoSpec);
    filterRight.prepare(monoSpec);

    filterLeft.reset();
    filterRight.reset();
}

void FilterDevice::reset()
{
    filterLeft.reset();
    filterRight.reset();
}

void FilterDevice::process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters)
{
    auto frequency = parameters.getRawParameterValue("filter_frequency")->load();
    auto resonance = parameters.getRawParameterValue("filter_resonance")->load();
    auto morph = parameters.getRawParameterValue("filter_morph")->load();
    auto decay = parameters.getRawParameterValue("filter_decay")->load();

    int numSamples = buffer.getNumSamples();

    // Set filter parameters
    filterLeft.setCutoffFrequency(frequency);
    filterRight.setCutoffFrequency(frequency);

    // Map resonance to Q factor (higher resonance = higher Q, with decay affecting it)
    float q = resonance * (1.0f + decay * 2.0f);
    filterLeft.setResonance(q);
    filterRight.setResonance(q);

    // Determine filter type based on morph parameter
    // 0.0 = lowpass, 0.5 = bandpass, 1.0 = highpass
    juce::dsp::StateVariableTPTFilterType filterType;
    if (morph < 0.33f)
        filterType = juce::dsp::StateVariableTPTFilterType::lowpass;
    else if (morph < 0.66f)
        filterType = juce::dsp::StateVariableTPTFilterType::bandpass;
    else
        filterType = juce::dsp::StateVariableTPTFilterType::highpass;

    filterLeft.setType(filterType);
    filterRight.setType(filterType);

    // Process each channel
    if (buffer.getNumChannels() > 0)
    {
        auto* leftData = buffer.getWritePointer(0);
        for (int i = 0; i < numSamples; ++i)
            leftData[i] = filterLeft.processSample(0, leftData[i]);
    }

    if (buffer.getNumChannels() > 1)
    {
        auto* rightData = buffer.getWritePointer(1);
        for (int i = 0; i < numSamples; ++i)
            rightData[i] = filterRight.processSample(0, rightData[i]);
    }
}
