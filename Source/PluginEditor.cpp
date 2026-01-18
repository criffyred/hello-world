#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TorsoS4AudioProcessorEditor::TorsoS4AudioProcessorEditor(TorsoS4AudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), parameters(vts)
{
    // Set colors
    auto bgColor = juce::Colour(0xff1a1a1a);
    auto accentColor = juce::Colour(0xffff6b35);

    setSize(900, 700);

    // Material Device Section
    materialGroup.setText("MATERIAL");
    materialGroup.setTextLabelPosition(juce::Justification::centred);
    addAndMakeVisible(materialGroup);

    setupComboBox(materialModeCombo, materialModeLabel, "Mode");
    materialModeCombo.addItem("Tape", 1);
    materialModeCombo.addItem("Poly", 2);
    materialModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(parameters, "material_mode", materialModeCombo);

    setupSlider(materialGainSlider, materialGainLabel, "Gain");
    materialGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "material_gain", materialGainSlider);

    setupSlider(materialAttackSlider, materialAttackLabel, "Attack");
    materialAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "material_poly_attack", materialAttackSlider);

    setupSlider(materialReleaseSlider, materialReleaseLabel, "Release");
    materialReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "material_poly_release", materialReleaseSlider);

    // Granular Device Section
    granularGroup.setText("GRANULAR (MOSAIC)");
    granularGroup.setTextLabelPosition(juce::Justification::centred);
    addAndMakeVisible(granularGroup);

    setupSlider(granularSizeSlider, granularSizeLabel, "Grain Size");
    granularSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_grainsize", granularSizeSlider);

    setupSlider(granularDensitySlider, granularDensityLabel, "Density");
    granularDensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_density", granularDensitySlider);

    setupSlider(granularPitchSlider, granularPitchLabel, "Pitch");
    granularPitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_pitch", granularPitchSlider);

    setupSlider(granularSpreadSlider, granularSpreadLabel, "Spread");
    granularSpreadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_spread", granularSpreadSlider);

    setupSlider(granularMixSlider, granularMixLabel, "Mix");
    granularMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_mix", granularMixSlider);

    // Filter Device Section
    filterGroup.setText("FILTER (RING)");
    filterGroup.setTextLabelPosition(juce::Justification::centred);
    addAndMakeVisible(filterGroup);

    setupSlider(filterFreqSlider, filterFreqLabel, "Frequency");
    filterFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_frequency", filterFreqSlider);

    setupSlider(filterResSlider, filterResLabel, "Resonance");
    filterResAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_resonance", filterResSlider);

    setupSlider(filterMorphSlider, filterMorphLabel, "Morph");
    filterMorphAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_morph", filterMorphSlider);

    setupSlider(filterDecaySlider, filterDecayLabel, "Decay");
    filterDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_decay", filterDecaySlider);

    // Distortion Device Section
    distortionGroup.setText("DISTORTION (DEFORM)");
    distortionGroup.setTextLabelPosition(juce::Justification::centred);
    addAndMakeVisible(distortionGroup);

    setupSlider(distDriveSlider, distDriveLabel, "Drive");
    distDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_drive", distDriveSlider);

    setupSlider(distBitcrushSlider, distBitcrushLabel, "Bit Crush");
    distBitcrushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_bitcrush", distBitcrushSlider);

    setupSlider(distCompressSlider, distCompressLabel, "Compress");
    distCompressAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_compress", distCompressSlider);

    setupSlider(distNoiseSlider, distNoiseLabel, "Noise");
    distNoiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_noise", distNoiseSlider);

    setupSlider(distTiltSlider, distTiltLabel, "Tilt");
    distTiltAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_tilt", distTiltSlider);

    // Space Device Section
    spaceGroup.setText("SPACE (VAST)");
    spaceGroup.setTextLabelPosition(juce::Justification::centred);
    addAndMakeVisible(spaceGroup);

    setupSlider(spaceDelayTimeSlider, spaceDelayTimeLabel, "Delay Time");
    spaceDelayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_delay_time", spaceDelayTimeSlider);

    setupSlider(spaceDelayFeedbackSlider, spaceDelayFeedbackLabel, "Feedback");
    spaceDelayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_delay_feedback", spaceDelayFeedbackSlider);

    setupSlider(spaceReverbSizeSlider, spaceReverbSizeLabel, "Reverb Size");
    spaceReverbSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_reverb_size", spaceReverbSizeSlider);

    setupSlider(spaceReverbDampingSlider, spaceReverbDampingLabel, "Damping");
    spaceReverbDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_reverb_damping", spaceReverbDampingSlider);

    setupSlider(spaceMixSlider, spaceMixLabel, "Mix");
    spaceMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_mix", spaceMixSlider);

    spaceFreezeButton.setButtonText("Freeze");
    addAndMakeVisible(spaceFreezeButton);
    spaceFreezeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(parameters, "space_freeze", spaceFreezeButton);

    // Master Volume
    setupSlider(masterVolumeSlider, masterVolumeLabel, "Master Volume");
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "master_volume", masterVolumeSlider);
}

TorsoS4AudioProcessorEditor::~TorsoS4AudioProcessorEditor()
{
}

//==============================================================================
void TorsoS4AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));

    g.setColour(juce::Colour(0xffff6b35));
    g.setFont(24.0f);
    g.drawText("TORSO S-4 SCULPTING SAMPLER", getLocalBounds().removeFromTop(40), juce::Justification::centred, true);
}

void TorsoS4AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(50); // Title space
    bounds.reduce(10, 10);

    int deviceHeight = 120;

    // Material section
    materialGroup.setBounds(bounds.removeFromTop(deviceHeight));
    auto materialBounds = materialGroup.getBounds().reduced(10, 25);
    int controlWidth = materialBounds.getWidth() / 4;
    materialModeCombo.setBounds(materialBounds.removeFromLeft(controlWidth).reduced(5));
    materialModeLabel.setBounds(materialModeCombo.getBounds().removeFromTop(20));
    materialGainSlider.setBounds(materialBounds.removeFromLeft(controlWidth).reduced(5));
    materialGainLabel.setBounds(materialGainSlider.getBounds().removeFromTop(20));
    materialAttackSlider.setBounds(materialBounds.removeFromLeft(controlWidth).reduced(5));
    materialAttackLabel.setBounds(materialAttackSlider.getBounds().removeFromTop(20));
    materialReleaseSlider.setBounds(materialBounds.removeFromLeft(controlWidth).reduced(5));
    materialReleaseLabel.setBounds(materialReleaseSlider.getBounds().removeFromTop(20));

    bounds.removeFromTop(5);

    // Granular section
    granularGroup.setBounds(bounds.removeFromTop(deviceHeight));
    auto granularBounds = granularGroup.getBounds().reduced(10, 25);
    controlWidth = granularBounds.getWidth() / 5;
    granularSizeSlider.setBounds(granularBounds.removeFromLeft(controlWidth).reduced(5));
    granularSizeLabel.setBounds(granularSizeSlider.getBounds().removeFromTop(20));
    granularDensitySlider.setBounds(granularBounds.removeFromLeft(controlWidth).reduced(5));
    granularDensityLabel.setBounds(granularDensitySlider.getBounds().removeFromTop(20));
    granularPitchSlider.setBounds(granularBounds.removeFromLeft(controlWidth).reduced(5));
    granularPitchLabel.setBounds(granularPitchSlider.getBounds().removeFromTop(20));
    granularSpreadSlider.setBounds(granularBounds.removeFromLeft(controlWidth).reduced(5));
    granularSpreadLabel.setBounds(granularSpreadSlider.getBounds().removeFromTop(20));
    granularMixSlider.setBounds(granularBounds.removeFromLeft(controlWidth).reduced(5));
    granularMixLabel.setBounds(granularMixSlider.getBounds().removeFromTop(20));

    bounds.removeFromTop(5);

    // Filter section
    filterGroup.setBounds(bounds.removeFromTop(deviceHeight));
    auto filterBounds = filterGroup.getBounds().reduced(10, 25);
    controlWidth = filterBounds.getWidth() / 4;
    filterFreqSlider.setBounds(filterBounds.removeFromLeft(controlWidth).reduced(5));
    filterFreqLabel.setBounds(filterFreqSlider.getBounds().removeFromTop(20));
    filterResSlider.setBounds(filterBounds.removeFromLeft(controlWidth).reduced(5));
    filterResLabel.setBounds(filterResSlider.getBounds().removeFromTop(20));
    filterMorphSlider.setBounds(filterBounds.removeFromLeft(controlWidth).reduced(5));
    filterMorphLabel.setBounds(filterMorphSlider.getBounds().removeFromTop(20));
    filterDecaySlider.setBounds(filterBounds.removeFromLeft(controlWidth).reduced(5));
    filterDecayLabel.setBounds(filterDecaySlider.getBounds().removeFromTop(20));

    bounds.removeFromTop(5);

    // Distortion section
    distortionGroup.setBounds(bounds.removeFromTop(deviceHeight));
    auto distBounds = distortionGroup.getBounds().reduced(10, 25);
    controlWidth = distBounds.getWidth() / 5;
    distDriveSlider.setBounds(distBounds.removeFromLeft(controlWidth).reduced(5));
    distDriveLabel.setBounds(distDriveSlider.getBounds().removeFromTop(20));
    distBitcrushSlider.setBounds(distBounds.removeFromLeft(controlWidth).reduced(5));
    distBitcrushLabel.setBounds(distBitcrushSlider.getBounds().removeFromTop(20));
    distCompressSlider.setBounds(distBounds.removeFromLeft(controlWidth).reduced(5));
    distCompressLabel.setBounds(distCompressSlider.getBounds().removeFromTop(20));
    distNoiseSlider.setBounds(distBounds.removeFromLeft(controlWidth).reduced(5));
    distNoiseLabel.setBounds(distNoiseSlider.getBounds().removeFromTop(20));
    distTiltSlider.setBounds(distBounds.removeFromLeft(controlWidth).reduced(5));
    distTiltLabel.setBounds(distTiltSlider.getBounds().removeFromTop(20));

    bounds.removeFromTop(5);

    // Space section
    spaceGroup.setBounds(bounds.removeFromTop(deviceHeight));
    auto spaceBounds = spaceGroup.getBounds().reduced(10, 25);
    controlWidth = spaceBounds.getWidth() / 6;
    spaceDelayTimeSlider.setBounds(spaceBounds.removeFromLeft(controlWidth).reduced(5));
    spaceDelayTimeLabel.setBounds(spaceDelayTimeSlider.getBounds().removeFromTop(20));
    spaceDelayFeedbackSlider.setBounds(spaceBounds.removeFromLeft(controlWidth).reduced(5));
    spaceDelayFeedbackLabel.setBounds(spaceDelayFeedbackSlider.getBounds().removeFromTop(20));
    spaceReverbSizeSlider.setBounds(spaceBounds.removeFromLeft(controlWidth).reduced(5));
    spaceReverbSizeLabel.setBounds(spaceReverbSizeSlider.getBounds().removeFromTop(20));
    spaceReverbDampingSlider.setBounds(spaceBounds.removeFromLeft(controlWidth).reduced(5));
    spaceReverbDampingLabel.setBounds(spaceReverbDampingSlider.getBounds().removeFromTop(20));
    spaceMixSlider.setBounds(spaceBounds.removeFromLeft(controlWidth).reduced(5));
    spaceMixLabel.setBounds(spaceMixSlider.getBounds().removeFromTop(20));
    spaceFreezeButton.setBounds(spaceBounds.removeFromLeft(controlWidth).reduced(5));

    // Master volume at bottom
    bounds.removeFromTop(10);
    auto masterBounds = bounds.removeFromTop(80);
    masterVolumeSlider.setBounds(masterBounds.withSizeKeepingCentre(150, 60));
    masterVolumeLabel.setBounds(masterVolumeSlider.getBounds().removeFromTop(20));
}

void TorsoS4AudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(12.0f));
    addAndMakeVisible(label);
}

void TorsoS4AudioProcessorEditor::setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(combo);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(12.0f));
    addAndMakeVisible(label);
}
