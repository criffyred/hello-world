#include "SpaceDevice.h"

SpaceDevice::SpaceDevice()
{
}

void SpaceDevice::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    // Setup delay lines (2 seconds max)
    int maxDelaySamples = static_cast<int>(2.0 * spec.sampleRate);
    delayLineLeft.prepare(spec);
    delayLineRight.prepare(spec);
    delayLineLeft.setMaximumDelayInSamples(maxDelaySamples);
    delayLineRight.setMaximumDelayInSamples(maxDelaySamples);

    // Setup reverb
    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;
    reverb.prepare(spec);

    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.33f;
    reverbParams.dryLevel = 0.67f;
    reverbParams.width = 1.0f;
    reverb.setParameters(reverbParams);
}

void SpaceDevice::reset()
{
    delayLineLeft.reset();
    delayLineRight.reset();
    reverb.reset();
    delayFeedbackLeft = 0.0f;
    delayFeedbackRight = 0.0f;
}

void SpaceDevice::process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters)
{
    auto delayTime = parameters.getRawParameterValue("space_delay_time")->load();
    auto feedback = parameters.getRawParameterValue("space_delay_feedback")->load();
    auto reverbSize = parameters.getRawParameterValue("space_reverb_size")->load();
    auto reverbDamping = parameters.getRawParameterValue("space_reverb_damping")->load();
    auto mix = parameters.getRawParameterValue("space_mix")->load();
    auto freeze = parameters.getRawParameterValue("space_freeze")->load() > 0.5f;

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // Store dry signal
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // Calculate delay time in samples
    float delayTimeSamples = (delayTime / 1000.0f) * static_cast<float>(currentSampleRate);
    delayLineLeft.setDelay(delayTimeSamples);
    delayLineRight.setDelay(delayTimeSamples);

    // Process delay
    if (delayTime > 0.0f)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            if (numChannels > 0)
            {
                float inputL = buffer.getSample(0, sample);
                float delayedL = delayLineLeft.popSample(0);
                float outputL = inputL + delayedL * feedback;
                delayLineLeft.pushSample(0, outputL);
                buffer.setSample(0, sample, outputL);
            }

            if (numChannels > 1)
            {
                float inputR = buffer.getSample(1, sample);
                float delayedR = delayLineRight.popSample(0);
                float outputR = inputR + delayedR * feedback;
                delayLineRight.pushSample(0, outputR);
                buffer.setSample(1, sample, outputR);
            }
        }
    }

    // Process reverb
    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = freeze ? 1.0f : reverbSize;
    reverbParams.damping = freeze ? 0.0f : reverbDamping;
    reverbParams.wetLevel = 1.0f; // We'll do our own mixing
    reverbParams.dryLevel = 0.0f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = freeze ? 1.0f : 0.0f;
    reverb.setParameters(reverbParams);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);

    // Mix dry and wet
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
