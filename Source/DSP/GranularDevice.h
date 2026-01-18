#pragma once

#include <JuceHeader.h>

/**
 * Granular Device (Mosaic) - Grain sculpting processor
 *
 * Features:
 * - Grain size control (1-500ms)
 * - Up to 128 grains
 * - Pitch shifting (±36 semitones)
 * - Grain spread/randomization
 * - 4-second circular buffer
 */
class GranularDevice
{
public:
    GranularDevice();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters);

private:
    struct Grain
    {
        float position = 0.0f;
        float speed = 1.0f;
        float amplitude = 0.0f;
        float age = 0.0f;
        float lifetime = 0.0f;
        bool active = false;
    };

    std::vector<Grain> grains;
    juce::AudioBuffer<float> grainBuffer;
    int writePosition = 0;
    int maxBufferSize = 0;
    double currentSampleRate = 44100.0;
    juce::Random random;

    float getGrainEnvelope(float position);
    float readFromBuffer(int channel, float position);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GranularDevice)
};
