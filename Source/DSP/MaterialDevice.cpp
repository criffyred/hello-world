#include "MaterialDevice.h"

MaterialDevice::MaterialDevice()
{
    // Initialize poly sampler with 8 voices
    for (int i = 0; i < 8; ++i)
        polySampler.addVoice(new PolySamplerVoice());

    polySampler.addSound(new PolySamplerSound());
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

void MaterialDevice::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages,
                             juce::AudioProcessorValueTreeState& parameters)
{
    auto mode = parameters.getRawParameterValue("material_mode")->load();
    auto gain = parameters.getRawParameterValue("material_gain")->load();
    auto attack = parameters.getRawParameterValue("material_poly_attack")->load();
    auto release = parameters.getRawParameterValue("material_poly_release")->load();

    int numSamples = buffer.getNumSamples();

    if (mode == 0) // Tape mode
    {
        // Simple tape looping - record input and play back
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Read from tape
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                float tapeSample = tapeBuffer.getSample(ch, tapeReadPosition);
                float inputSample = buffer.getSample(ch, sample);

                // Mix tape playback with input
                buffer.setSample(ch, sample, (tapeSample + inputSample) * gain);

                // Write to tape (overdub mode)
                if (tapeRecording)
                {
                    tapeBuffer.setSample(ch, tapeWritePosition,
                                        tapeBuffer.getSample(ch, tapeWritePosition) * 0.5f + inputSample * 0.5f);
                }
            }

            // Advance tape positions
            tapeReadPosition = (tapeReadPosition + 1) % maxTapeLengthSamples;
            if (tapeRecording)
                tapeWritePosition = (tapeWritePosition + 1) % maxTapeLengthSamples;
        }
    }
    else // Poly mode (mode == 1)
    {
        // Update ADSR for all voices
        for (int i = 0; i < polySampler.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<PolySamplerVoice*>(polySampler.getVoice(i)))
            {
                voice->setADSR(attack, release);
            }
        }

        // Render polyphonic sampler
        polySampler.renderNextBlock(buffer, midiMessages, 0, numSamples);

        // Apply gain
        buffer.applyGain(gain);
    }
}

//==============================================================================
// PolySamplerVoice implementation

MaterialDevice::PolySamplerVoice::PolySamplerVoice()
{
    adsrParams.attack = 0.01f;
    adsrParams.decay = 0.1f;
    adsrParams.sustain = 1.0f;
    adsrParams.release = 0.1f;
}

bool MaterialDevice::PolySamplerVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<PolySamplerSound*>(sound) != nullptr;
}

void MaterialDevice::PolySamplerVoice::startNote(int midiNoteNumber, float velocity,
                                                 juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    currentAngle = 0.0;
    level = velocity * 0.15;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();
    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;

    adsr.noteOn();
}

void MaterialDevice::PolySamplerVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void MaterialDevice::PolySamplerVoice::pitchWheelMoved(int /*newPitchWheelValue*/)
{
}

void MaterialDevice::PolySamplerVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}

void MaterialDevice::PolySamplerVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                                       int startSample, int numSamples)
{
    if (angleDelta != 0.0)
    {
        while (--numSamples >= 0)
        {
            auto currentSample = (float)(std::sin(currentAngle) * level * adsr.getNextSample());

            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample(i, startSample, currentSample);

            currentAngle += angleDelta;
            ++startSample;

            if (!adsr.isActive())
                stopNote(0.0f, false);
        }
    }
}

void MaterialDevice::PolySamplerVoice::setADSR(float attack, float release)
{
    adsrParams.attack = attack;
    adsrParams.release = release;
    adsr.setParameters(adsrParams);
}
