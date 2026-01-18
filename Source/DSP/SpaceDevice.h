#pragma once

#include <JuceHeader.h>

/**
 * Space Device (Vast) - Stereo delay and reverb processor
 *
 * Features:
 * - Stereo delay with feedback
 * - Multiple delay algorithms
 * - Reverb with size and damping control
 * - Freeze mode
 * - Combined processing (not send/return)
 */
class SpaceDevice
{
public:
    SpaceDevice();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters);

private:
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineLeft;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineRight;
    juce::dsp::Reverb reverb;

    double currentSampleRate = 44100.0;
    float delayFeedbackLeft = 0.0f;
    float delayFeedbackRight = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpaceDevice)
};
