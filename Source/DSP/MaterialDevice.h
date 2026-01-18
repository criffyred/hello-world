#pragma once

#include <JuceHeader.h>

/**
 * Material Device - Tape recorder/looping OR polyphonic sampler
 *
 * Features:
 * - Tape mode: Live recording, looping, overdubbing
 * - Poly mode: 8-voice polyphonic sampler with MIDI control
 */
class MaterialDevice
{
public:
    MaterialDevice();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages,
                 juce::AudioProcessorValueTreeState& parameters);

private:
    // Sampler for Poly mode
    juce::Synthesiser polySampler;

    // Tape loop buffer (6 minutes at 48kHz = ~17.3 million samples)
    juce::AudioBuffer<float> tapeBuffer;
    int tapeWritePosition = 0;
    int tapeReadPosition = 0;
    bool tapeRecording = false;
    bool tapeLooping = false;

    // Sample rate and spec
    double currentSampleRate = 44100.0;
    int maxTapeLengthSamples = 0;

    // ADSR for poly mode
    struct PolySamplerVoice : public juce::SynthesiserVoice
    {
        PolySamplerVoice();

        bool canPlaySound(juce::SynthesiserSound*) override;
        void startNote(int midiNoteNumber, float velocity,
                      juce::SynthesiserSound*, int currentPitchWheelPosition) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int newPitchWheelValue) override;
        void controllerMoved(int controllerNumber, int newControllerValue) override;
        void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

        void setADSR(float attack, float release);

    private:
        juce::ADSR adsr;
        juce::ADSR::Parameters adsrParams;
        double currentAngle = 0.0;
        double angleDelta = 0.0;
        double level = 0.0;
    };

    struct PolySamplerSound : public juce::SynthesiserSound
    {
        PolySamplerSound() {}
        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaterialDevice)
};
