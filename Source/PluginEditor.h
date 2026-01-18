#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TorsoS4LookAndFeel.h"
#include "WaveformDisplay.h"

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

    // Device sections (custom components)
    class DeviceSection : public juce::Component
    {
    public:
        DeviceSection(const juce::String& deviceName) : name(deviceName) {}

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds();

            // Device name
            g.setColour(juce::Colour(0xff00d4ff));
            g.setFont(juce::Font(12.0f, juce::Font::bold));
            g.drawText(name, bounds.removeFromTop(20), juce::Justification::centred);
        }

        void addKnob(juce::Slider* knob, juce::Label* label)
        {
            knobs.add(knob);
            labels.add(label);
            addAndMakeVisible(knob);
            addAndMakeVisible(label);
        }

        void addCombo(juce::ComboBox* combo, juce::Label* label)
        {
            combos.add(combo);
            labels.add(label);
            addAndMakeVisible(combo);
            addAndMakeVisible(label);
        }

        void addButton(juce::ToggleButton* button)
        {
            buttons.add(button);
            addAndMakeVisible(button);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            bounds.removeFromTop(25); // Space for title

            int numControls = knobs.size() + combos.size() + buttons.size();
            if (numControls == 0) return;

            int controlWidth = bounds.getWidth() / numControls;

            // Layout knobs
            for (int i = 0; i < knobs.size(); ++i)
            {
                auto controlBounds = bounds.removeFromLeft(controlWidth).reduced(5);
                labels[i]->setBounds(controlBounds.removeFromTop(15));
                knobs[i]->setBounds(controlBounds.removeFromTop(controlBounds.getHeight() - 20));
            }

            // Layout combos
            for (int i = 0; i < combos.size(); ++i)
            {
                auto controlBounds = bounds.removeFromLeft(controlWidth).reduced(5);
                int labelIdx = knobs.size() + i;
                labels[labelIdx]->setBounds(controlBounds.removeFromTop(15));
                combos[i]->setBounds(controlBounds.reduced(0, 20));
            }

            // Layout buttons
            for (auto* button : buttons)
            {
                auto controlBounds = bounds.removeFromLeft(controlWidth).reduced(5);
                button->setBounds(controlBounds);
            }
        }

    private:
        juce::String name;
        juce::OwnedArray<juce::Slider> knobs;
        juce::OwnedArray<juce::ComboBox> combos;
        juce::OwnedArray<juce::ToggleButton> buttons;
        juce::OwnedArray<juce::Label> labels;
    };

    // Material Device Controls
    DeviceSection materialSection;
    juce::ComboBox materialModeCombo;
    juce::Slider materialGainSlider, materialAttackSlider, materialReleaseSlider;
    juce::Label materialGainLabel, materialAttackLabel, materialReleaseLabel, materialModeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> materialModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> materialReleaseAttachment;

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
    juce::Slider filterFreqSlider, filterResSlider, filterMorphSlider, filterDecaySlider;
    juce::Label filterFreqLabel, filterResLabel, filterMorphLabel, filterDecayLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterMorphAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterDecayAttachment;

    // Distortion Device Controls
    DeviceSection distortionSection;
    juce::Slider distDriveSlider, distBitcrushSlider, distCompressSlider, distNoiseSlider, distTiltSlider;
    juce::Label distDriveLabel, distBitcrushLabel, distCompressLabel, distNoiseLabel, distTiltLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distBitcrushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distCompressAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distNoiseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distTiltAttachment;

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
