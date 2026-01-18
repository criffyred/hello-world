#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TorsoS4AudioProcessorEditor::TorsoS4AudioProcessorEditor(TorsoS4AudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      parameters(vts),
      materialSection("MATERIAL", "|||•"),
      granularSection("GRANULAR", "∴"),
      filterSection("FILTER", "⌢"),
      distortionSection("COLOR", "⌇"),
      spaceSection("SPACE", "⌯")
{
    // Apply custom look and feel
    setLookAndFeel(&torsoLookAndFeel);

    setSize(1200, 800);

    // Waveform Display
    addAndMakeVisible(waveformDisplay);

    // Material Device Section
    addAndMakeVisible(materialSection);

    materialModeCombo.addItem("Tape", 1);
    materialModeCombo.addItem("Poly", 2);
    materialModeCombo.addItem("Bypass", 3);
    setupComboBox(materialModeCombo, materialModeLabel, "Mode");
    materialSection.addCombo(&materialModeCombo, &materialModeLabel);
    materialModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(parameters, "material_mode", materialModeCombo);

    setupRotaryKnob(materialGainSlider, materialGainLabel, "Gain");
    materialSection.addKnob(&materialGainSlider, &materialGainLabel);
    materialGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "material_gain", materialGainSlider);

    setupRotaryKnob(materialAttackSlider, materialAttackLabel, "Attack");
    materialSection.addKnob(&materialAttackSlider, &materialAttackLabel);
    materialAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "material_poly_attack", materialAttackSlider);

    setupRotaryKnob(materialReleaseSlider, materialReleaseLabel, "Release");
    materialSection.addKnob(&materialReleaseSlider, &materialReleaseLabel);
    materialReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "material_poly_release", materialReleaseSlider);

    // Granular Device Section
    addAndMakeVisible(granularSection);

    setupRotaryKnob(granularSizeSlider, granularSizeLabel, "Grain Size");
    granularSection.addKnob(&granularSizeSlider, &granularSizeLabel);
    granularSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_grainsize", granularSizeSlider);

    setupRotaryKnob(granularDensitySlider, granularDensityLabel, "Density");
    granularSection.addKnob(&granularDensitySlider, &granularDensityLabel);
    granularDensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_density", granularDensitySlider);

    setupRotaryKnob(granularPitchSlider, granularPitchLabel, "Pitch");
    granularSection.addKnob(&granularPitchSlider, &granularPitchLabel);
    granularPitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_pitch", granularPitchSlider);

    setupRotaryKnob(granularSpreadSlider, granularSpreadLabel, "Spread");
    granularSection.addKnob(&granularSpreadSlider, &granularSpreadLabel);
    granularSpreadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_spread", granularSpreadSlider);

    setupRotaryKnob(granularMixSlider, granularMixLabel, "Mix");
    granularSection.addKnob(&granularMixSlider, &granularMixLabel);
    granularMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "granular_mix", granularMixSlider);

    // Filter Device Section
    addAndMakeVisible(filterSection);

    setupRotaryKnob(filterFreqSlider, filterFreqLabel, "Frequency");
    filterSection.addKnob(&filterFreqSlider, &filterFreqLabel);
    filterFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_frequency", filterFreqSlider);

    setupRotaryKnob(filterResSlider, filterResLabel, "Resonance");
    filterSection.addKnob(&filterResSlider, &filterResLabel);
    filterResAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_resonance", filterResSlider);

    setupRotaryKnob(filterMorphSlider, filterMorphLabel, "Morph");
    filterSection.addKnob(&filterMorphSlider, &filterMorphLabel);
    filterMorphAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_morph", filterMorphSlider);

    setupRotaryKnob(filterDecaySlider, filterDecayLabel, "Decay");
    filterSection.addKnob(&filterDecaySlider, &filterDecayLabel);
    filterDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_decay", filterDecaySlider);

    setupRotaryKnob(filterMixSlider, filterMixLabel, "Mix");
    filterSection.addKnob(&filterMixSlider, &filterMixLabel);
    filterMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "filter_mix", filterMixSlider);

    // Distortion Device Section
    addAndMakeVisible(distortionSection);

    setupRotaryKnob(distDriveSlider, distDriveLabel, "Drive");
    distortionSection.addKnob(&distDriveSlider, &distDriveLabel);
    distDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_drive", distDriveSlider);

    setupRotaryKnob(distBitcrushSlider, distBitcrushLabel, "Bit Crush");
    distortionSection.addKnob(&distBitcrushSlider, &distBitcrushLabel);
    distBitcrushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_bitcrush", distBitcrushSlider);

    setupRotaryKnob(distCompressSlider, distCompressLabel, "Compress");
    distortionSection.addKnob(&distCompressSlider, &distCompressLabel);
    distCompressAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_compress", distCompressSlider);

    setupRotaryKnob(distNoiseSlider, distNoiseLabel, "Noise");
    distortionSection.addKnob(&distNoiseSlider, &distNoiseLabel);
    distNoiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_noise", distNoiseSlider);

    setupRotaryKnob(distTiltSlider, distTiltLabel, "Tilt");
    distortionSection.addKnob(&distTiltSlider, &distTiltLabel);
    distTiltAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_tilt", distTiltSlider);

    setupRotaryKnob(distMixSlider, distMixLabel, "Mix");
    distortionSection.addKnob(&distMixSlider, &distMixLabel);
    distMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "dist_mix", distMixSlider);

    // Space Device Section
    addAndMakeVisible(spaceSection);

    setupRotaryKnob(spaceDelayTimeSlider, spaceDelayTimeLabel, "Delay Time");
    spaceSection.addKnob(&spaceDelayTimeSlider, &spaceDelayTimeLabel);
    spaceDelayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_delay_time", spaceDelayTimeSlider);

    setupRotaryKnob(spaceDelayFeedbackSlider, spaceDelayFeedbackLabel, "Feedback");
    spaceSection.addKnob(&spaceDelayFeedbackSlider, &spaceDelayFeedbackLabel);
    spaceDelayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_delay_feedback", spaceDelayFeedbackSlider);

    setupRotaryKnob(spaceReverbSizeSlider, spaceReverbSizeLabel, "Reverb Size");
    spaceSection.addKnob(&spaceReverbSizeSlider, &spaceReverbSizeLabel);
    spaceReverbSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_reverb_size", spaceReverbSizeSlider);

    setupRotaryKnob(spaceReverbDampingSlider, spaceReverbDampingLabel, "Damping");
    spaceSection.addKnob(&spaceReverbDampingSlider, &spaceReverbDampingLabel);
    spaceReverbDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_reverb_damping", spaceReverbDampingSlider);

    setupRotaryKnob(spaceMixSlider, spaceMixLabel, "Mix");
    spaceSection.addKnob(&spaceMixSlider, &spaceMixLabel);
    spaceMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "space_mix", spaceMixSlider);

    spaceFreezeButton.setButtonText("Freeze");
    spaceSection.addButton(&spaceFreezeButton);
    spaceFreezeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(parameters, "space_freeze", spaceFreezeButton);

    // Master Volume
    setupRotaryKnob(masterVolumeSlider, masterVolumeLabel, "Master Volume");
    addAndMakeVisible(masterVolumeSlider);
    addAndMakeVisible(masterVolumeLabel);
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(parameters, "master_volume", masterVolumeSlider);
}

TorsoS4AudioProcessorEditor::~TorsoS4AudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void TorsoS4AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0a0a0a));

    // Title
    g.setColour(juce::Colour(0xffe0e0e0));
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("TORSO S-4 SCULPTING SAMPLER", getLocalBounds().removeFromTop(35), juce::Justification::centred, true);

    // Signal flow arrows between devices
    g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(180); // Skip waveform area

    for (int i = 0; i < 4; ++i)
    {
        auto arrowY = bounds.getY() + (i + 1) * 110 - 10;
        juce::Path arrow;
        arrow.startNewSubPath(bounds.getWidth() / 2.0f - 20, arrowY);
        arrow.lineTo(bounds.getWidth() / 2.0f + 20, arrowY);
        arrow.lineTo(bounds.getWidth() / 2.0f + 15, arrowY - 3);
        arrow.startNewSubPath(bounds.getWidth() / 2.0f + 20, arrowY);
        arrow.lineTo(bounds.getWidth() / 2.0f + 15, arrowY + 3);
        g.strokePath(arrow, juce::PathStrokeType(1.5f));
    }
}

void TorsoS4AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(40); // Title space

    // Waveform display (top 20%)
    waveformDisplay.setBounds(bounds.removeFromTop(140));
    bounds.removeFromTop(10);

    int sectionHeight = 100;

    // Device sections (each 12% of height)
    materialSection.setBounds(bounds.removeFromTop(sectionHeight));
    bounds.removeFromTop(10);

    granularSection.setBounds(bounds.removeFromTop(sectionHeight));
    bounds.removeFromTop(10);

    filterSection.setBounds(bounds.removeFromTop(sectionHeight));
    bounds.removeFromTop(10);

    distortionSection.setBounds(bounds.removeFromTop(sectionHeight));
    bounds.removeFromTop(10);

    spaceSection.setBounds(bounds.removeFromTop(sectionHeight));
    bounds.removeFromTop(10);

    // Master volume at bottom
    auto masterBounds = bounds.removeFromBottom(80).withSizeKeepingCentre(120, 80);
    masterVolumeLabel.setBounds(masterBounds.removeFromTop(15));
    masterVolumeSlider.setBounds(masterBounds);
}

void TorsoS4AudioProcessorEditor::setupRotaryKnob(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    slider.setLookAndFeel(&torsoLookAndFeel);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(11.0f));
}

void TorsoS4AudioProcessorEditor::setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText)
{
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(11.0f));
}
