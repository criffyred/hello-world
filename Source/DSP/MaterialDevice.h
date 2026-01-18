#pragma once

#include <JuceHeader.h>

/**
 * Material Device - Tape recorder/looping OR polyphonic sampler
 *
 * Features:
 * - Tape mode: Live recording, looping, overdubbing with varispeed
 * - Poly mode: 8-voice polyphonic sampler with MIDI control playing actual samples
 * - Bypass mode: Pass audio through unprocessed
 */
class MaterialDevice
{
public:
    MaterialDevice();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages,
                 juce::AudioProcessorValueTreeState& parameters);

    // Load a sample for poly mode
    void loadSample(const juce::AudioBuffer<float>& sampleToLoad);
    void setSampleData(const float* const* sampleData, int numChannels, int numSamples);

private:
    // Shared sample buffer for poly mode
    juce::AudioBuffer<float> sampleBuffer;
    int sampleLength = 0;

    // Sampler for Poly mode
    juce::Synthesiser polySampler;

    // Tape loop buffer (6 minutes at 48kHz)
    juce::AudioBuffer<float> tapeBuffer;
    int tapeWritePosition = 0;
    int tapeReadPosition = 0;
    float tapeSpeed = 1.0f;
    bool tapeRecording = true;
    bool tapeLooping = true;

    // Sample rate and spec
    double currentSampleRate = 44100.0;
    int maxTapeLengthSamples = 0;

    // Polyphonic Sample Voice - actually plays loaded samples
    class SampleVoice : public juce::SynthesiserVoice
    {
    public:
        SampleVoice(const juce::AudioBuffer<float>& sampleBufferToUse);

        bool canPlaySound(juce::SynthesiserSound*) override;
        void startNote(int midiNoteNumber, float velocity,
                      juce::SynthesiserSound*, int currentPitchWheelPosition) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int newPitchWheelValue) override;
        void controllerMoved(int controllerNumber, int newControllerValue) override;
        void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

        void setADSR(float attack, float release);
        void updateSampleBuffer(const juce::AudioBuffer<float>& newBuffer);

    private:
        const juce::AudioBuffer<float>& sampleBuffer;
        juce::ADSR adsr;
        juce::ADSR::Parameters adsrParams;

        double sourceSamplePosition = 0.0;
        double pitchRatio = 1.0;
        double level = 0.0;
    };

    class SampleSound : public juce::SynthesiserSound
    {
    public:
        SampleSound() {}
        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaterialDevice)
};
