#pragma once

#include <JuceHeader.h>
#include "DSP/MaterialDevice.h"
#include "DSP/GranularDevice.h"
#include "DSP/FilterDevice.h"
#include "DSP/DistortionDevice.h"
#include "DSP/SpaceDevice.h"
#include "DSP/ModulationSystem.h"

//==============================================================================
/**
    Torso S-4 Sculpting Sampler Plugin

    Single track prototype with 5 audio devices in series:
    Material -> Granular -> Filter -> Distortion -> Space
*/
class TorsoS4AudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    TorsoS4AudioProcessor();
    ~TorsoS4AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter tree
    juce::AudioProcessorValueTreeState parameters;

    // Expose devices for UI access
    MaterialDevice& getMaterialDevice() { return materialDevice; }
    GranularDevice& getGranularDevice() { return granularDevice; }
    FilterDevice& getFilterDevice() { return filterDevice; }
    DistortionDevice& getDistortionDevice() { return distortionDevice; }
    SpaceDevice& getSpaceDevice() { return spaceDevice; }
    ModulationSystem& getModulationSystem() { return modulationSystem; }

private:
    //==============================================================================
    // Audio processing chain (5 devices in series)
    MaterialDevice materialDevice;
    GranularDevice granularDevice;
    FilterDevice filterDevice;
    DistortionDevice distortionDevice;
    SpaceDevice spaceDevice;

    // Modulation system
    ModulationSystem modulationSystem;

    // Create parameter layout
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TorsoS4AudioProcessor)
};
