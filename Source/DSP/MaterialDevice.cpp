#include "MaterialDevice.h"

MaterialDevice::MaterialDevice()
{
    // Initialize with a default 1-second silent buffer
    sampleBuffer.setSize(2, 44100);
    sampleBuffer.clear();
    sampleLength = 44100;

    // Initialize poly sampler with 8 voices
    for (int i = 0; i < 8; ++i)
        polySampler.addVoice(new SampleVoice(sampleBuffer));

    polySampler.addSound(new SampleSound());
}

void MaterialDevice::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    // Allocate tape buffer for 6 minutes of stereo audio
    maxTapeLengthSamples = static_cast<int>(6.0 * 60.0 * spec.sampleRate);
    tapeBuffer.setSize(2, maxTapeLengthSamples);
    tapeBuffer.clear();

    polySampler.setCurrentPlaybackSampleRate(spec.sampleRate);
}

void MaterialDevice::reset()
{
    tapeBuffer.clear();
    tapeWritePosition = 0;
    tapeReadPosition = 0;
    polySampler.allNotesOff(0, false);
}

void MaterialDevice::loadSample(const juce::AudioBuffer<float>& sampleToLoad)
{
    sampleBuffer.makeCopyOf(sampleToLoad);
    sampleLength = sampleToLoad.getNumSamples();

    // Update all voices with new sample buffer
    for (int i = 0; i < polySampler.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SampleVoice*>(polySampler.getVoice(i)))
        {
            voice->updateSampleBuffer(sampleBuffer);
        }
    }
}

void MaterialDevice::setSampleData(const float* const* sampleData, int numChannels, int numSamples)
{
    sampleBuffer.setSize(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        sampleBuffer.copyFrom(ch, 0, sampleData[ch], numSamples);
    }
    sampleLength = numSamples;
}

void MaterialDevice::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages,
                             juce::AudioProcessorValueTreeState& parameters)
{
    auto mode = static_cast<int>(parameters.getRawParameterValue("material_mode")->load());
    auto gain = parameters.getRawParameterValue("material_gain")->load();
    auto attack = parameters.getRawParameterValue("material_poly_attack")->load();
    auto release = parameters.getRawParameterValue("material_poly_release")->load();

    int numSamples = buffer.getNumSamples();

    if (mode == 0) // Tape mode
    {
        // Tape looping with varispeed
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Read from tape with speed control
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                // Linear interpolation for smooth tape playback
                int pos1 = static_cast<int>(tapeReadPosition);
                int pos2 = (pos1 + 1) % maxTapeLengthSamples;
                float frac = tapeReadPosition - static_cast<float>(pos1);

                float tapeSample = tapeBuffer.getSample(ch, pos1) * (1.0f - frac) +
                                 tapeBuffer.getSample(ch, pos2) * frac;

                float inputSample = buffer.getSample(ch, sample);

                // Mix tape playback with input (overdub mode)
                buffer.setSample(ch, sample, (tapeSample + inputSample * 0.5f) * gain);

                // Write to tape (overdub mode)
                if (tapeRecording)
                {
                    tapeBuffer.setSample(ch, tapeWritePosition,
                                        tapeBuffer.getSample(ch, tapeWritePosition) * 0.5f + inputSample * 0.5f);
                }
            }

            // Advance tape positions with speed control
            tapeReadPosition += tapeSpeed;
            if (tapeReadPosition >= maxTapeLengthSamples)
                tapeReadPosition -= maxTapeLengthSamples;
            if (tapeReadPosition < 0)
                tapeReadPosition += maxTapeLengthSamples;

            if (tapeRecording)
                tapeWritePosition = (tapeWritePosition + 1) % maxTapeLengthSamples;
        }
    }
    else if (mode == 1) // Poly mode - play actual samples
    {
        // Update ADSR for all voices
        for (int i = 0; i < polySampler.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SampleVoice*>(polySampler.getVoice(i)))
            {
                voice->setADSR(attack, release);
            }
        }

        // Clear buffer for polyphonic rendering
        buffer.clear();

        // Render polyphonic sampler with actual sample playback
        polySampler.renderNextBlock(buffer, midiMessages, 0, numSamples);

        // Apply gain
        buffer.applyGain(gain);
    }
    // else mode == 2: Bypass - do nothing, audio passes through
}

//==============================================================================
// SampleVoice implementation - ACTUALLY plays samples with pitch shifting

MaterialDevice::SampleVoice::SampleVoice(const juce::AudioBuffer<float>& sampleBufferToUse)
    : sampleBuffer(sampleBufferToUse)
{
    adsrParams.attack = 0.01f;
    adsrParams.decay = 0.1f;
    adsrParams.sustain = 1.0f;
    adsrParams.release = 0.1f;
    adsr.setParameters(adsrParams);
}

bool MaterialDevice::SampleVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SampleSound*>(sound) != nullptr;
}

void MaterialDevice::SampleVoice::startNote(int midiNoteNumber, float velocity,
                                           juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    sourceSamplePosition = 0.0;
    level = velocity;

    // Calculate pitch ratio for sample playback
    // Assuming middle C (MIDI 60) plays at original speed
    auto pitchInSemitones = static_cast<double>(midiNoteNumber - 60);
    pitchRatio = std::pow(2.0, pitchInSemitones / 12.0);

    adsr.noteOn();
}

void MaterialDevice::SampleVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        sourceSamplePosition = 0.0;
    }
}

void MaterialDevice::SampleVoice::pitchWheelMoved(int /*newPitchWheelValue*/)
{
    // Could implement pitch bend here
}

void MaterialDevice::SampleVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}

void MaterialDevice::SampleVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                                  int startSample, int numSamples)
{
    if (sampleBuffer.getNumSamples() == 0)
        return;

    auto numChannels = outputBuffer.getNumChannels();
    auto sampleLength = sampleBuffer.getNumSamples();

    while (--numSamples >= 0)
    {
        auto envValue = adsr.getNextSample();

        if (sourceSamplePosition < sampleLength && envValue > 0.0f)
        {
            // Linear interpolation for smooth pitch shifting
            auto pos = static_cast<int>(sourceSamplePosition);
            auto nextPos = pos + 1;
            auto frac = static_cast<float>(sourceSamplePosition - pos);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                auto sample1 = (pos < sampleLength) ? sampleBuffer.getSample(ch % sampleBuffer.getNumChannels(), pos) : 0.0f;
                auto sample2 = (nextPos < sampleLength) ? sampleBuffer.getSample(ch % sampleBuffer.getNumChannels(), nextPos) : 0.0f;

                auto currentSample = sample1 + frac * (sample2 - sample1);
                outputBuffer.addSample(ch, startSample, currentSample * envValue * level);
            }

            sourceSamplePosition += pitchRatio;
        }
        else
        {
            if (!adsr.isActive())
                stopNote(0.0f, false);
        }

        ++startSample;
    }
}

void MaterialDevice::SampleVoice::setADSR(float attack, float release)
{
    adsrParams.attack = attack;
    adsrParams.release = release;
    adsr.setParameters(adsrParams);
}

void MaterialDevice::SampleVoice::updateSampleBuffer(const juce::AudioBuffer<float>& /*newBuffer*/)
{
    // Sample buffer is a const reference, so it automatically updates
    // when the parent MaterialDevice updates its sampleBuffer
}
