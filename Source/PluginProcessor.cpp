#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TorsoS4AudioProcessor::TorsoS4AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
    :
#endif
      parameters(*this, nullptr, juce::Identifier("TorsoS4"), createParameterLayout())
{
}

TorsoS4AudioProcessor::~TorsoS4AudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout TorsoS4AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Material Device Parameters
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "material_mode", "Material Mode",
        juce::StringArray{"Tape", "Poly"}, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "material_gain", "Material Gain",
        juce::NormalisableRange<float>(0.0f, 2.0f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "material_poly_attack", "Poly Attack",
        juce::NormalisableRange<float>(0.001f, 1.0f, 0.0f, 0.3f), 0.01f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "material_poly_release", "Poly Release",
        juce::NormalisableRange<float>(0.01f, 5.0f, 0.0f, 0.3f), 0.1f));

    // Granular Device Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "granular_grainsize", "Grain Size",
        juce::NormalisableRange<float>(1.0f, 500.0f, 0.0f, 0.3f), 50.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "granular_density", "Grain Density",
        1, 128, 32));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "granular_pitch", "Granular Pitch",
        juce::NormalisableRange<float>(-36.0f, 36.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "granular_spread", "Grain Spread",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "granular_mix", "Granular Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Filter Device Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter_frequency", "Filter Frequency",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.0f, 0.3f), 1000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter_resonance", "Filter Resonance",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.3f), 0.707f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter_morph", "Filter Morph",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter_decay", "Filter Decay",
        juce::NormalisableRange<float>(0.01f, 10.0f, 0.0f, 0.3f), 0.5f));

    // Distortion Device Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "dist_drive", "Distortion Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "dist_bitcrush", "Bit Reduction",
        juce::NormalisableRange<float>(1.0f, 16.0f), 16.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "dist_compress", "Compression",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "dist_noise", "Noise Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "dist_tilt", "Tilt Balance",
        juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));

    // Space Device Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "space_delay_time", "Delay Time",
        juce::NormalisableRange<float>(0.0f, 2000.0f, 0.0f, 0.3f), 250.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "space_delay_feedback", "Delay Feedback",
        juce::NormalisableRange<float>(0.0f, 0.95f), 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "space_reverb_size", "Reverb Size",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "space_reverb_damping", "Reverb Damping",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "space_mix", "Space Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "space_freeze", "Space Freeze", false));

    // Master Parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "master_volume", "Master Volume",
        juce::NormalisableRange<float>(0.0f, 2.0f), 1.0f));

    return layout;
}

//==============================================================================
const juce::String TorsoS4AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TorsoS4AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TorsoS4AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TorsoS4AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TorsoS4AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TorsoS4AudioProcessor::getNumPrograms()
{
    return 1;
}

int TorsoS4AudioProcessor::getCurrentProgram()
{
    return 0;
}

void TorsoS4AudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String TorsoS4AudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void TorsoS4AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void TorsoS4AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;

    // Initialize all devices
    materialDevice.prepare(spec);
    granularDevice.prepare(spec);
    filterDevice.prepare(spec);
    distortionDevice.prepare(spec);
    spaceDevice.prepare(spec);
    modulationSystem.prepare(sampleRate);
}

void TorsoS4AudioProcessor::releaseResources()
{
    materialDevice.reset();
    granularDevice.reset();
    filterDevice.reset();
    distortionDevice.reset();
    spaceDevice.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TorsoS4AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TorsoS4AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any extra output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Update modulation system
    modulationSystem.update(buffer.getNumSamples(), getSampleRate());

    // Process through the 5-device chain in series
    // 1. Material (Tape/Sampler)
    materialDevice.process(buffer, midiMessages, parameters);

    // 2. Granular
    granularDevice.process(buffer, parameters);

    // 3. Filter
    filterDevice.process(buffer, parameters);

    // 4. Distortion
    distortionDevice.process(buffer, parameters);

    // 5. Space (Reverb/Delay)
    spaceDevice.process(buffer, parameters);

    // Apply master volume
    auto masterVolume = parameters.getRawParameterValue("master_volume")->load();
    buffer.applyGain(masterVolume);
}

//==============================================================================
bool TorsoS4AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* TorsoS4AudioProcessor::createEditor()
{
    return new TorsoS4AudioProcessorEditor(*this, parameters);
}

//==============================================================================
void TorsoS4AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TorsoS4AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TorsoS4AudioProcessor();
}
