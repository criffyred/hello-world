#include "DistortionDevice.h"

DistortionDevice::DistortionDevice()
{
    // Set up waveshaper with tanh distortion
    waveshaper.functionToUse = [](float x) { return std::tanh(x); };
}

void DistortionDevice::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    waveshaper.prepare(spec);
    compressor.prepare(spec);

    // Setup compressor defaults
    compressor.setThreshold(-10.0f);
    compressor.setRatio(4.0f);
    compressor.setAttack(5.0f);
    compressor.setRelease(100.0f);

    // Setup tilt EQ filters
    tiltLowShelfLeft.prepare(monoSpec);
    tiltLowShelfRight.prepare(monoSpec);
    tiltHighShelfLeft.prepare(monoSpec);
    tiltHighShelfRight.prepare(monoSpec);
}

void DistortionDevice::reset()
{
    waveshaper.reset();
    compressor.reset();
    tiltLowShelfLeft.reset();
    tiltLowShelfRight.reset();
    tiltHighShelfLeft.reset();
    tiltHighShelfRight.reset();
}

void DistortionDevice::process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters)
{
    auto drive = parameters.getRawParameterValue("dist_drive")->load();
    auto bitcrush = parameters.getRawParameterValue("dist_bitcrush")->load();
    auto compress = parameters.getRawParameterValue("dist_compress")->load();
    auto noise = parameters.getRawParameterValue("dist_noise")->load();
    auto tilt = parameters.getRawParameterValue("dist_tilt")->load();

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // Apply drive/distortion
    if (drive > 0.01f)
    {
        float driveAmount = 1.0f + drive * 20.0f; // Up to 20x gain
        buffer.applyGain(driveAmount);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        waveshaper.process(context);

        buffer.applyGain(1.0f / std::sqrt(driveAmount)); // Compensate output
    }

    // Apply bit crushing
    if (bitcrush < 15.9f)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                data[i] = applyBitCrush(data[i], bitcrush);
            }
        }
    }

    // Apply compression
    if (compress > 0.01f)
    {
        compressor.setThreshold(-20.0f + compress * 15.0f);
        compressor.setRatio(1.0f + compress * 15.0f);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        compressor.process(context);
    }

    // Add noise
    if (noise > 0.001f)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                data[i] += (random.nextFloat() * 2.0f - 1.0f) * noise * 0.1f;
            }
        }
    }

    // Apply tilt EQ
    if (std::abs(tilt) > 0.01f)
    {
        // Tilt = -1: boost lows, cut highs
        // Tilt = +1: cut lows, boost highs
        float lowGain = -tilt * 6.0f; // dB
        float highGain = tilt * 6.0f;  // dB

        auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            currentSampleRate, 200.0f, 0.707f, juce::Decibels::decibelsToGain(lowGain));
        auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            currentSampleRate, 5000.0f, 0.707f, juce::Decibels::decibelsToGain(highGain));

        *tiltLowShelfLeft.coefficients = *lowCoeffs;
        *tiltLowShelfRight.coefficients = *lowCoeffs;
        *tiltHighShelfLeft.coefficients = *highCoeffs;
        *tiltHighShelfRight.coefficients = *highCoeffs;

        // Process left channel
        if (numChannels > 0)
        {
            auto* leftData = buffer.getWritePointer(0);
            for (int i = 0; i < numSamples; ++i)
            {
                leftData[i] = tiltLowShelfLeft.processSample(leftData[i]);
                leftData[i] = tiltHighShelfLeft.processSample(leftData[i]);
            }
        }

        // Process right channel
        if (numChannels > 1)
        {
            auto* rightData = buffer.getWritePointer(1);
            for (int i = 0; i < numSamples; ++i)
            {
                rightData[i] = tiltLowShelfRight.processSample(rightData[i]);
                rightData[i] = tiltHighShelfRight.processSample(rightData[i]);
            }
        }
    }
}

float DistortionDevice::applyBitCrush(float sample, float bits)
{
    float levels = std::pow(2.0f, bits);
    return std::round(sample * levels) / levels;
}
