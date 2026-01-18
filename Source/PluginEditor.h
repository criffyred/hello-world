#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TorsoS4LookAndFeel.h"
#include "WaveformDisplay.h"
#include "DeviceSection.h"

//==============================================================================
class TorsoS4AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    TorsoS4AudioProcessorEditor(TorsoS4AudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~TorsoS4AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TorsoS4AudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;

    // Custom Look and Feel
    TorsoS4LookAndFeel torsoLookAndFeel;

    // Waveform Display
    WaveformDisplay waveformDisplay;

    // Material Device Controls
    DeviceSection materialSection;
    juce::ComboBox materialModeCombo;
    juce::Slider materialGainSlider, materialAttackSlider, materialReleaseSlider, materialTapeSpeedSlider;
    juce::Label materialGainLabel, materialAttackLabel, materialReleaseLabel, materialModeLabel, materialTapeSpeedLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> materialModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialTapeSpeedAttachment;

    // Granular Device Controls
    DeviceSection granularSection;
    juce::Slider granularSizeSlider, granularDensitySlider, granularPitchSlider, granularSpreadSlider, granularMixSlider;
    juce::Label granularSizeLabel, granularDensityLabel, granularPitchLabel, granularSpreadLabel, granularMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularDensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularPitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularSpreadAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularMixAttachment;

    // Filter Device Controls
    DeviceSection filterSection;
    juce::Slider filterFreqSlider, filterResSlider, filterMorphSlider, filterDecaySlider, filterMixSlider;
    juce::Label filterFreqLabel, filterResLabel, filterMorphLabel, filterDecayLabel, filterMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterMorphAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterDecayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterMixAttachment;

    // Distortion Device Controls
    DeviceSection distortionSection;
    juce::Slider distDriveSlider, distBitcrushSlider, distCompressSlider, distNoiseSlider, distTiltSlider, distMixSlider;
    juce::Label distDriveLabel, distBitcrushLabel, distCompressLabel, distNoiseLabel, distTiltLabel, distMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distBitcrushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distCompressAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distNoiseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distTiltAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distMixAttachment;

    // Space Device Controls
    DeviceSection spaceSection;
    juce::Slider spaceDelayTimeSlider, spaceDelayFeedbackSlider, spaceReverbSizeSlider, spaceReverbDampingSlider, spaceMixSlider;
    juce::ToggleButton spaceFreezeButton;
    juce::Label spaceDelayTimeLabel, spaceDelayFeedbackLabel, spaceReverbSizeLabel, spaceReverbDampingLabel, spaceMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spaceDelayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spaceDelayFeedbackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spaceReverbSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spaceReverbDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spaceMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> spaceFreezeAttachment;

    // Master Controls
    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    void setupRotaryKnob(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    void setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TorsoS4AudioProcessorEditor)
};
