#include "GranularDevice.h"

GranularDevice::GranularDevice()
{
    grains.resize(128); // Max 128 grains
}

void GranularDevice::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    // 4-second circular buffer for granular processing
    maxBufferSize = static_cast<int>(4.0 * spec.sampleRate);
    grainBuffer.setSize(spec.numChannels, maxBufferSize);
    grainBuffer.clear();

    writePosition = 0;
}

void GranularDevice::reset()
{
    grainBuffer.clear();
    writePosition = 0;

    for (auto& grain : grains)
        grain.active = false;
}

void GranularDevice::process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters)
{
    auto grainSize = parameters.getRawParameterValue("granular_grainsize")->load();
    auto density = parameters.getRawParameterValue("granular_density")->load();
    auto pitch = parameters.getRawParameterValue("granular_pitch")->load();
    auto spread = parameters.getRawParameterValue("granular_spread")->load();
    auto mix = parameters.getRawParameterValue("granular_mix")->load();

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // Store dry signal for mixing
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // Write incoming audio to grain buffer
    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            grainBuffer.setSample(ch, writePosition, buffer.getSample(ch, sample));
        }
        writePosition = (writePosition + 1) % maxBufferSize;
    }

    // Calculate grain parameters
    float grainSizeSamples = (grainSize / 1000.0f) * static_cast<float>(currentSampleRate);
    float playbackSpeed = std::pow(2.0f, pitch / 12.0f); // Convert semitones to speed ratio
    int activeGrains = static_cast<int>(density);

    // Clear output buffer for grain rendering
    buffer.clear();

    // Process grains
    for (int i = 0; i < activeGrains; ++i)
    {
        auto& grain = grains[i];

        // Spawn new grain if needed
        if (!grain.active)
        {
            grain.active = true;
            grain.age = 0.0f;
            grain.lifetime = grainSizeSamples;
            grain.speed = playbackSpeed;

            // Random position with spread control
            float spreadAmount = spread * maxBufferSize * 0.5f;
            float randomOffset = random.nextFloat() * spreadAmount - (spreadAmount * 0.5f);
            grain.position = static_cast<float>(writePosition) + randomOffset;

            while (grain.position < 0.0f)
                grain.position += maxBufferSize;
            while (grain.position >= maxBufferSize)
                grain.position -= maxBufferSize;

            grain.amplitude = 1.0f / std::sqrt(static_cast<float>(activeGrains));
        }

        // Render grain
        if (grain.active)
        {
            for (int sample = 0; sample < numSamples; ++sample)
            {
                if (grain.age < grain.lifetime)
                {
                    float envelope = getGrainEnvelope(grain.age / grain.lifetime);

                    for (int ch = 0; ch < numChannels; ++ch)
                    {
                        float grainSample = readFromBuffer(ch, grain.position);
                        buffer.addSample(ch, sample, grainSample * envelope * grain.amplitude);
                    }

                    grain.position += grain.speed;
                    while (grain.position >= maxBufferSize)
                        grain.position -= maxBufferSize;
                    while (grain.position < 0.0f)
                        grain.position += maxBufferSize;

                    grain.age += 1.0f;
                }
                else
                {
                    grain.active = false;
                    break;
                }
            }
        }
    }

    // Mix dry and wet signals
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float dry = dryBuffer.getSample(ch, sample);
            float wet = buffer.getSample(ch, sample);
            buffer.setSample(ch, sample, dry * (1.0f - mix) + wet * mix);
        }
    }
}

float GranularDevice::getGrainEnvelope(float position)
{
    // Hann window envelope
    return 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * position));
}

float GranularDevice::readFromBuffer(int channel, float position)
{
    int index1 = static_cast<int>(std::floor(position));
    int index2 = (index1 + 1) % maxBufferSize;
    float frac = position - std::floor(position);

    float sample1 = grainBuffer.getSample(channel, index1);
    float sample2 = grainBuffer.getSample(channel, index2);

    // Linear interpolation
    return sample1 + frac * (sample2 - sample1);
}
