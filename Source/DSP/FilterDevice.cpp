#include "FilterDevice.h"

FilterDevice::FilterDevice()
{
}

void FilterDevice::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    // Initialize all 48 bands for both channels
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int band = 0; band < NUM_BANDS; ++band)
        {
            filterBanks[ch].bands[band].prepare(monoSpec);
            filterBanks[ch].bands[band].reset();

            // Set initial frequency for each band (logarithmically spaced)
            float freq = getBandFrequency(band);
            filterBanks[ch].bands[band].setCutoffFrequency(freq);
        }
    }
}

void FilterDevice::reset()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int band = 0; band < NUM_BANDS; ++band)
        {
            filterBanks[ch].bands[band].reset();
        }
    }
}

float FilterDevice::getBandFrequency(int bandIndex)
{
    // Logarithmically space 48 bands from 20Hz to 20kHz
    float minFreq = 20.0f;
    float maxFreq = 20000.0f;
    float logMin = std::log(minFreq);
    float logMax = std::log(maxFreq);

    float t = static_cast<float>(bandIndex) / static_cast<float>(NUM_BANDS - 1);
    return std::exp(logMin + t * (logMax - logMin));
}

float FilterDevice::getBandGain(int bandIndex, float morph, float frequency, float centerFreq)
{
    // Calculate gain for this band based on morph parameter
    // morph: 0.0 = lowpass, 0.5 = bandpass, 1.0 = highpass

    float bandFreq = getBandFrequency(bandIndex);
    float normalizedFreq = bandFreq / 20000.0f; // 0 to 1

    if (morph < 0.5f)
    {
        // Lowpass -> Bandpass transition
        float lpAmount = (0.5f - morph) * 2.0f; // 1.0 at morph=0, 0.0 at morph=0.5
        float bpAmount = morph * 2.0f;          // 0.0 at morph=0, 1.0 at morph=0.5

        // Lowpass: Higher gain for lower frequencies
        float lpGain = 1.0f - normalizedFreq;

        // Bandpass: Peak around center frequency
        float freqDist = std::abs(bandFreq - centerFreq) / centerFreq;
        float bpGain = std::exp(-freqDist * freqDist * 8.0f); // Gaussian peak

        return lpGain * lpAmount + bpGain * bpAmount;
    }
    else
    {
        // Bandpass -> Highpass transition
        float bpAmount = (1.0f - morph) * 2.0f; // 1.0 at morph=0.5, 0.0 at morph=1.0
        float hpAmount = (morph - 0.5f) * 2.0f; // 0.0 at morph=0.5, 1.0 at morph=1.0

        // Bandpass: Peak around center frequency
        float freqDist = std::abs(bandFreq - centerFreq) / centerFreq;
        float bpGain = std::exp(-freqDist * freqDist * 8.0f);

        // Highpass: Higher gain for higher frequencies
        float hpGain = normalizedFreq;

        return bpGain * bpAmount + hpGain * hpAmount;
    }
}

void FilterDevice::process(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& parameters)
{
    auto frequency = parameters.getRawParameterValue("filter_frequency")->load();
    auto resonance = parameters.getRawParameterValue("filter_resonance")->load();
    auto morph = parameters.getRawParameterValue("filter_morph")->load();
    auto decay = parameters.getRawParameterValue("filter_decay")->load();
    auto mix = parameters.getRawParameterValue("filter_mix")->load();

    int numSamples = buffer.getNumSamples();
    int numChannels = juce::jmin(buffer.getNumChannels(), 2);

    // If mix is 0, bypass processing entirely
    if (mix <= 0.0f)
        return;

    // Store dry signal for mixing
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    // Calculate Q factor from resonance and decay
    float q = juce::jlimit(0.1f, 20.0f, resonance * (1.0f + decay * 4.0f));

    // Configure all filter bands
    for (int ch = 0; ch < numChannels; ++ch)
    {
        for (int band = 0; band < NUM_BANDS; ++band)
        {
            auto& filter = filterBanks[ch].bands[band];

            // Each band is a bandpass filter tuned to its specific frequency
            filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            filter.setResonance(q);
            filter.setCutoffFrequency(getBandFrequency(band));
        }
    }

    // Process each channel through the 48-band filter bank
    for (int ch = 0; ch < numChannels; ++ch)
    {
        // Create temporary buffer for filtered output
        std::vector<float> filteredOutput(numSamples, 0.0f);
        auto* channelData = buffer.getWritePointer(ch);

        // Process through all 48 bands
        for (int band = 0; band < NUM_BANDS; ++band)
        {
            auto& filter = filterBanks[ch].bands[band];

            // Calculate gain for this band based on morph
            float bandGain = getBandGain(band, morph, getBandFrequency(band), frequency);

            // Process this band
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float inputSample = channelData[sample];
                float bandOutput = filter.processSample(0, inputSample);

                // Accumulate with gain based on morph parameter
                filteredOutput[sample] += bandOutput * bandGain;
            }
        }

        // Normalize and write back to buffer
        float normalizationFactor = 1.0f / std::sqrt(static_cast<float>(NUM_BANDS));
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = filteredOutput[sample] * normalizationFactor * 2.0f; // 2.0x boost for presence
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

    // Copy mono to stereo if needed
    if (buffer.getNumChannels() > 1 && numChannels == 1)
    {
        buffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
    }
}
