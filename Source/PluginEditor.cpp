#include "PluginProcessor.h"
#include "PluginEditor.h"

TorsoS4AudioProcessorEditor::TorsoS4AudioProcessorEditor(TorsoS4AudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), parameters(vts)
{
    setSize(1100, 750);

    // Setup rotary knob function
    auto setupKnob = [](juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
    };

    // Waveform
    addAndMakeVisible(waveformDisplay);
    waveformDisplay.onFileLoaded = [this](const juce::File& file)
    {
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
        if (reader != nullptr)
        {
            juce::AudioBuffer<float> tempBuffer;
            tempBuffer.setSize(static_cast<int>(reader->numChannels), static_cast<int>(reader->lengthInSamples));
            reader->read(&tempBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
            audioProcessor.getMaterialDevice().loadSample(tempBuffer);
        }
    };

    // Material
    materialLabel.setText("|||... MATERIAL", juce::dontSendNotification);
    materialLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    materialLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
    addAndMakeVisible(materialLabel);

    materialModeCombo.addItem("Tape", 1);
    materialModeCombo.addItem("Poly", 2);
    materialModeCombo.addItem("Bypass", 3);
    materialModeLabel.setText("Mode", juce::dontSendNotification);
    materialModeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(materialModeCombo);
    addAndMakeVisible(materialModeLabel);
    materialModeAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(parameters, "material_mode", materialModeCombo));

    setupKnob(materialGainSlider, materialGainLabel, "Gain");
    addAndMakeVisible(materialGainSlider);
    addAndMakeVisible(materialGainLabel);
    materialGainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "material_gain", materialGainSlider));

    setupKnob(materialAttackSlider, materialAttackLabel, "Attack");
    addAndMakeVisible(materialAttackSlider);
    addAndMakeVisible(materialAttackLabel);
    materialAttackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "material_poly_attack", materialAttackSlider));

    setupKnob(materialReleaseSlider, materialReleaseLabel, "Release");
    addAndMakeVisible(materialReleaseSlider);
    addAndMakeVisible(materialReleaseLabel);
    materialReleaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "material_poly_release", materialReleaseSlider));

    setupKnob(materialTapeSpeedSlider, materialTapeSpeedLabel, "Speed");
    addAndMakeVisible(materialTapeSpeedSlider);
    addAndMakeVisible(materialTapeSpeedLabel);
    materialTapeSpeedAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "material_tape_speed", materialTapeSpeedSlider));

    // Granular
    granularLabel.setText("∴ GRANULAR", juce::dontSendNotification);
    granularLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    granularLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
    addAndMakeVisible(granularLabel);

    setupKnob(granularSizeSlider, granularSizeLabel, "Size");
    addAndMakeVisible(granularSizeSlider);
    addAndMakeVisible(granularSizeLabel);
    granularSizeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "granular_grainsize", granularSizeSlider));

    setupKnob(granularDensitySlider, granularDensityLabel, "Density");
    addAndMakeVisible(granularDensitySlider);
    addAndMakeVisible(granularDensityLabel);
    granularDensityAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "granular_density", granularDensitySlider));

    setupKnob(granularPitchSlider, granularPitchLabel, "Pitch");
    addAndMakeVisible(granularPitchSlider);
    addAndMakeVisible(granularPitchLabel);
    granularPitchAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "granular_pitch", granularPitchSlider));

    setupKnob(granularSpreadSlider, granularSpreadLabel, "Spread");
    addAndMakeVisible(granularSpreadSlider);
    addAndMakeVisible(granularSpreadLabel);
    granularSpreadAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "granular_spread", granularSpreadSlider));

    setupKnob(granularMixSlider, granularMixLabel, "Mix");
    addAndMakeVisible(granularMixSlider);
    addAndMakeVisible(granularMixLabel);
    granularMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "granular_mix", granularMixSlider));

    // Filter
    filterLabel.setText("⌢ FILTER", juce::dontSendNotification);
    filterLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    filterLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
    addAndMakeVisible(filterLabel);

    setupKnob(filterFreqSlider, filterFreqLabel, "Freq");
    addAndMakeVisible(filterFreqSlider);
    addAndMakeVisible(filterFreqLabel);
    filterFreqAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "filter_frequency", filterFreqSlider));

    setupKnob(filterResSlider, filterResLabel, "Res");
    addAndMakeVisible(filterResSlider);
    addAndMakeVisible(filterResLabel);
    filterResAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "filter_resonance", filterResSlider));

    setupKnob(filterMorphSlider, filterMorphLabel, "Morph");
    addAndMakeVisible(filterMorphSlider);
    addAndMakeVisible(filterMorphLabel);
    filterMorphAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "filter_morph", filterMorphSlider));

    setupKnob(filterDecaySlider, filterDecayLabel, "Decay");
    addAndMakeVisible(filterDecaySlider);
    addAndMakeVisible(filterDecayLabel);
    filterDecayAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "filter_decay", filterDecaySlider));

    setupKnob(filterMixSlider, filterMixLabel, "Mix");
    addAndMakeVisible(filterMixSlider);
    addAndMakeVisible(filterMixLabel);
    filterMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "filter_mix", filterMixSlider));

    // Distortion
    distortionLabel.setText("⌇ COLOR", juce::dontSendNotification);
    distortionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    distortionLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
    addAndMakeVisible(distortionLabel);

    setupKnob(distDriveSlider, distDriveLabel, "Drive");
    addAndMakeVisible(distDriveSlider);
    addAndMakeVisible(distDriveLabel);
    distDriveAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "dist_drive", distDriveSlider));

    setupKnob(distBitcrushSlider, distBitcrushLabel, "Bits");
    addAndMakeVisible(distBitcrushSlider);
    addAndMakeVisible(distBitcrushLabel);
    distBitcrushAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "dist_bitcrush", distBitcrushSlider));

    setupKnob(distCompressSlider, distCompressLabel, "Comp");
    addAndMakeVisible(distCompressSlider);
    addAndMakeVisible(distCompressLabel);
    distCompressAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "dist_compress", distCompressSlider));

    setupKnob(distNoiseSlider, distNoiseLabel, "Noise");
    addAndMakeVisible(distNoiseSlider);
    addAndMakeVisible(distNoiseLabel);
    distNoiseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "dist_noise", distNoiseSlider));

    setupKnob(distTiltSlider, distTiltLabel, "Tilt");
    addAndMakeVisible(distTiltSlider);
    addAndMakeVisible(distTiltLabel);
    distTiltAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "dist_tilt", distTiltSlider));

    setupKnob(distMixSlider, distMixLabel, "Mix");
    addAndMakeVisible(distMixSlider);
    addAndMakeVisible(distMixLabel);
    distMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "dist_mix", distMixSlider));

    // Space
    spaceLabel.setText("⌯ SPACE", juce::dontSendNotification);
    spaceLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    spaceLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00d4ff));
    addAndMakeVisible(spaceLabel);

    setupKnob(spaceDelayTimeSlider, spaceDelayTimeLabel, "Time");
    addAndMakeVisible(spaceDelayTimeSlider);
    addAndMakeVisible(spaceDelayTimeLabel);
    spaceDelayTimeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "space_delay_time", spaceDelayTimeSlider));

    setupKnob(spaceDelayFeedbackSlider, spaceDelayFeedbackLabel, "Feedback");
    addAndMakeVisible(spaceDelayFeedbackSlider);
    addAndMakeVisible(spaceDelayFeedbackLabel);
    spaceDelayFeedbackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "space_delay_feedback", spaceDelayFeedbackSlider));

    setupKnob(spaceReverbSizeSlider, spaceReverbSizeLabel, "Size");
    addAndMakeVisible(spaceReverbSizeSlider);
    addAndMakeVisible(spaceReverbSizeLabel);
    spaceReverbSizeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "space_reverb_size", spaceReverbSizeSlider));

    setupKnob(spaceReverbDampingSlider, spaceReverbDampingLabel, "Damp");
    addAndMakeVisible(spaceReverbDampingSlider);
    addAndMakeVisible(spaceReverbDampingLabel);
    spaceReverbDampingAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "space_reverb_damping", spaceReverbDampingSlider));

    setupKnob(spaceMixSlider, spaceMixLabel, "Mix");
    addAndMakeVisible(spaceMixSlider);
    addAndMakeVisible(spaceMixLabel);
    spaceMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "space_mix", spaceMixSlider));

    spaceFreezeButton.setButtonText("Freeze");
    addAndMakeVisible(spaceFreezeButton);
    spaceFreezeAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(parameters, "space_freeze", spaceFreezeButton));

    // Master
    setupKnob(masterVolumeSlider, masterVolumeLabel, "Master");
    addAndMakeVisible(masterVolumeSlider);
    addAndMakeVisible(masterVolumeLabel);
    masterVolumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(parameters, "master_volume", masterVolumeSlider));
}

TorsoS4AudioProcessorEditor::~TorsoS4AudioProcessorEditor()
{
}

void TorsoS4AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));

    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("TORSO S-4 SCULPTING SAMPLER", getLocalBounds().removeFromTop(35), juce::Justification::centred);
}

void TorsoS4AudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(15);
    bounds.removeFromTop(35); // Title

    // Waveform
    waveformDisplay.setBounds(bounds.removeFromTop(120));
    bounds.removeFromTop(5);

    int knobSize = 70;
    int spacing = 10;

    auto layoutRow = [&](juce::Label& sectionLabel, auto&... controls)
    {
        auto row = bounds.removeFromTop(90);
        sectionLabel.setBounds(row.removeFromLeft(120));

        int totalWidth = 0;
        int numControls = sizeof...(controls) / 2; // Divide by 2 because we have label+slider pairs

        auto processControl = [&](auto& label, auto& control)
        {
            auto controlBounds = row.removeFromLeft(knobSize + spacing);
            label.setBounds(controlBounds.removeFromBottom(18));
            control.setBounds(controlBounds.withHeight(knobSize));
        };

        (processControl(controls.first, controls.second), ...);
    };

    // Material (1 combo + 4 knobs)
    {
        auto row = bounds.removeFromTop(90);
        materialLabel.setBounds(row.removeFromLeft(120));

        auto combo1 = row.removeFromLeft(knobSize + spacing);
        materialModeLabel.setBounds(combo1.removeFromBottom(18));
        materialModeCombo.setBounds(combo1.removeFromTop(25));

        auto knob1 = row.removeFromLeft(knobSize + spacing);
        materialGainLabel.setBounds(knob1.removeFromBottom(18));
        materialGainSlider.setBounds(knob1.withHeight(knobSize));

        auto knob2 = row.removeFromLeft(knobSize + spacing);
        materialAttackLabel.setBounds(knob2.removeFromBottom(18));
        materialAttackSlider.setBounds(knob2.withHeight(knobSize));

        auto knob3 = row.removeFromLeft(knobSize + spacing);
        materialReleaseLabel.setBounds(knob3.removeFromBottom(18));
        materialReleaseSlider.setBounds(knob3.withHeight(knobSize));

        auto knob4 = row.removeFromLeft(knobSize + spacing);
        materialTapeSpeedLabel.setBounds(knob4.removeFromBottom(18));
        materialTapeSpeedSlider.setBounds(knob4.withHeight(knobSize));
    }

    // Granular (5 knobs)
    {
        auto row = bounds.removeFromTop(90);
        granularLabel.setBounds(row.removeFromLeft(120));

        auto knob1 = row.removeFromLeft(knobSize + spacing);
        granularSizeLabel.setBounds(knob1.removeFromBottom(18));
        granularSizeSlider.setBounds(knob1.withHeight(knobSize));

        auto knob2 = row.removeFromLeft(knobSize + spacing);
        granularDensityLabel.setBounds(knob2.removeFromBottom(18));
        granularDensitySlider.setBounds(knob2.withHeight(knobSize));

        auto knob3 = row.removeFromLeft(knobSize + spacing);
        granularPitchLabel.setBounds(knob3.removeFromBottom(18));
        granularPitchSlider.setBounds(knob3.withHeight(knobSize));

        auto knob4 = row.removeFromLeft(knobSize + spacing);
        granularSpreadLabel.setBounds(knob4.removeFromBottom(18));
        granularSpreadSlider.setBounds(knob4.withHeight(knobSize));

        auto knob5 = row.removeFromLeft(knobSize + spacing);
        granularMixLabel.setBounds(knob5.removeFromBottom(18));
        granularMixSlider.setBounds(knob5.withHeight(knobSize));
    }

    // Filter (5 knobs)
    {
        auto row = bounds.removeFromTop(90);
        filterLabel.setBounds(row.removeFromLeft(120));

        auto knob1 = row.removeFromLeft(knobSize + spacing);
        filterFreqLabel.setBounds(knob1.removeFromBottom(18));
        filterFreqSlider.setBounds(knob1.withHeight(knobSize));

        auto knob2 = row.removeFromLeft(knobSize + spacing);
        filterResLabel.setBounds(knob2.removeFromBottom(18));
        filterResSlider.setBounds(knob2.withHeight(knobSize));

        auto knob3 = row.removeFromLeft(knobSize + spacing);
        filterMorphLabel.setBounds(knob3.removeFromBottom(18));
        filterMorphSlider.setBounds(knob3.withHeight(knobSize));

        auto knob4 = row.removeFromLeft(knobSize + spacing);
        filterDecayLabel.setBounds(knob4.removeFromBottom(18));
        filterDecaySlider.setBounds(knob4.withHeight(knobSize));

        auto knob5 = row.removeFromLeft(knobSize + spacing);
        filterMixLabel.setBounds(knob5.removeFromBottom(18));
        filterMixSlider.setBounds(knob5.withHeight(knobSize));
    }

    // Distortion (6 knobs)
    {
        auto row = bounds.removeFromTop(90);
        distortionLabel.setBounds(row.removeFromLeft(120));

        auto knob1 = row.removeFromLeft(knobSize + spacing);
        distDriveLabel.setBounds(knob1.removeFromBottom(18));
        distDriveSlider.setBounds(knob1.withHeight(knobSize));

        auto knob2 = row.removeFromLeft(knobSize + spacing);
        distBitcrushLabel.setBounds(knob2.removeFromBottom(18));
        distBitcrushSlider.setBounds(knob2.withHeight(knobSize));

        auto knob3 = row.removeFromLeft(knobSize + spacing);
        distCompressLabel.setBounds(knob3.removeFromBottom(18));
        distCompressSlider.setBounds(knob3.withHeight(knobSize));

        auto knob4 = row.removeFromLeft(knobSize + spacing);
        distNoiseLabel.setBounds(knob4.removeFromBottom(18));
        distNoiseSlider.setBounds(knob4.withHeight(knobSize));

        auto knob5 = row.removeFromLeft(knobSize + spacing);
        distTiltLabel.setBounds(knob5.removeFromBottom(18));
        distTiltSlider.setBounds(knob5.withHeight(knobSize));

        auto knob6 = row.removeFromLeft(knobSize + spacing);
        distMixLabel.setBounds(knob6.removeFromBottom(18));
        distMixSlider.setBounds(knob6.withHeight(knobSize));
    }

    // Space (5 knobs + button)
    {
        auto row = bounds.removeFromTop(90);
        spaceLabel.setBounds(row.removeFromLeft(120));

        auto knob1 = row.removeFromLeft(knobSize + spacing);
        spaceDelayTimeLabel.setBounds(knob1.removeFromBottom(18));
        spaceDelayTimeSlider.setBounds(knob1.withHeight(knobSize));

        auto knob2 = row.removeFromLeft(knobSize + spacing);
        spaceDelayFeedbackLabel.setBounds(knob2.removeFromBottom(18));
        spaceDelayFeedbackSlider.setBounds(knob2.withHeight(knobSize));

        auto knob3 = row.removeFromLeft(knobSize + spacing);
        spaceReverbSizeLabel.setBounds(knob3.removeFromBottom(18));
        spaceReverbSizeSlider.setBounds(knob3.withHeight(knobSize));

        auto knob4 = row.removeFromLeft(knobSize + spacing);
        spaceReverbDampingLabel.setBounds(knob4.removeFromBottom(18));
        spaceReverbDampingSlider.setBounds(knob4.withHeight(knobSize));

        auto knob5 = row.removeFromLeft(knobSize + spacing);
        spaceMixLabel.setBounds(knob5.removeFromBottom(18));
        spaceMixSlider.setBounds(knob5.withHeight(knobSize));

        auto btn = row.removeFromLeft(knobSize + spacing);
        spaceFreezeButton.setBounds(btn.withHeight(25));
    }

    // Master
    {
        auto row = bounds.removeFromTop(90);
        row = row.withSizeKeepingCentre(knobSize + spacing, 90);
        masterVolumeLabel.setBounds(row.removeFromBottom(18));
        masterVolumeSlider.setBounds(row.withHeight(knobSize));
    }
}
