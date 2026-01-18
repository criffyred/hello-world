#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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

    // Device sections (collapsible groups)
    juce::GroupComponent materialGroup, granularGroup, filterGroup, distortionGroup, spaceGroup;

    // Material Device Controls
    juce::ComboBox materialModeCombo;
    juce::Slider materialGainSlider, materialAttackSlider, materialReleaseSlider;
    juce::Label materialGainLabel, materialAttackLabel, materialReleaseLabel, materialModeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> materialModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialReleaseAttachment;

    // Granular Device Controls
    juce::Slider granularSizeSlider, granularDensitySlider, granularPitchSlider, granularSpreadSlider, granularMixSlider;
    juce::Label granularSizeLabel, granularDensityLabel, granularPitchLabel, granularSpreadLabel, granularMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularDensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularPitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularSpreadAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> granularMixAttachment;

    // Filter Device Controls
    juce::Slider filterFreqSlider, filterResSlider, filterMorphSlider, filterDecaySlider;
    juce::Label filterFreqLabel, filterResLabel, filterMorphLabel, filterDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterMorphAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterDecayAttachment;

    // Distortion Device Controls
    juce::Slider distDriveSlider, distBitcrushSlider, distCompressSlider, distNoiseSlider, distTiltSlider;
    juce::Label distDriveLabel, distBitcrushLabel, distCompressLabel, distNoiseLabel, distTiltLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distBitcrushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distCompressAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distNoiseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distTiltAttachment;

    // Space Device Controls
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

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    void setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TorsoS4AudioProcessorEditor)
};
