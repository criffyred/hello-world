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
        DeviceSection(const juce::String& deviceName, const juce::String& deviceIcon)
            : name(deviceName), icon(deviceIcon) {}

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds();

            // DEBUG: Draw background to visualize bounds
            g.setColour(juce::Colour(0xff1a1a1a));
            g.fillRect(bounds);

            // Device icon and name (S-4 style)
            auto headerBounds = bounds.removeFromTop(25);
            g.setColour(juce::Colour(0xff00d4ff));
            g.setFont(juce::Font(16.0f, juce::Font::bold));
            g.drawText(icon, headerBounds.removeFromLeft(30), juce::Justification::centred);
            g.setFont(juce::Font(11.0f, juce::Font::bold));
            g.drawText(name, headerBounds, juce::Justification::centredLeft);

            // DEBUG: Draw control count
            g.setColour(juce::Colours::red);
            g.drawText("Controls: " + juce::String(controls.size()), bounds, juce::Justification::topRight);
        }

        void addKnob(juce::Slider* knob, juce::Label* label)
        {
            controls.add({ControlType::Knob, knob, nullptr, nullptr, label});
            addAndMakeVisible(knob);
            addAndMakeVisible(label);
        }

        void addCombo(juce::ComboBox* combo, juce::Label* label)
        {
            controls.add({ControlType::Combo, nullptr, combo, nullptr, label});
            addAndMakeVisible(combo);
            addAndMakeVisible(label);
        }

        void addButton(juce::ToggleButton* button)
        {
            controls.add({ControlType::Button, nullptr, nullptr, button, nullptr});
            addAndMakeVisible(button);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            bounds.removeFromTop(25); // Space for title

            if (controls.isEmpty()) return;

            int controlWidth = bounds.getWidth() / controls.size();

            for (auto& control : controls)
            {
                auto controlBounds = bounds.removeFromLeft(controlWidth).reduced(5);

                if (control.type == ControlType::Knob && control.knob != nullptr)
                {
                    if (control.label != nullptr)
                        control.label->setBounds(controlBounds.removeFromTop(15));
                    control.knob->setBounds(controlBounds);
                }
                else if (control.type == ControlType::Combo && control.combo != nullptr)
                {
                    if (control.label != nullptr)
                        control.label->setBounds(controlBounds.removeFromTop(15));
                    control.combo->setBounds(controlBounds.removeFromTop(25));
                }
                else if (control.type == ControlType::Button && control.button != nullptr)
                {
                    control.button->setBounds(controlBounds);
                }
            }
        }

    private:
        juce::String name;
        juce::String icon;

        enum class ControlType { Knob, Combo, Button };

        struct Control
        {
            ControlType type;
            juce::Slider* knob;
            juce::ComboBox* combo;
            juce::ToggleButton* button;
            juce::Label* label;
        };

        juce::Array<Control> controls;
    };

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
