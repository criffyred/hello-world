#pragma once

#include <JuceHeader.h>

class DeviceSection : public juce::Component
{
public:
    DeviceSection(const juce::String& deviceName, const juce::String& deviceIcon)
        : name(deviceName), icon(deviceIcon) {}

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();

        // DEBUG: Draw background
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillRect(bounds);

        // Device icon and name (S-4 style)
        auto headerBounds = bounds.removeFromTop(25);
        g.setColour(juce::Colour(0xff00d4ff));
        g.setFont(juce::Font(16.0f, juce::Font::bold));
        g.drawText(icon, headerBounds.removeFromLeft(30), juce::Justification::centred);
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText(name, headerBounds, juce::Justification::centredLeft);
    }

    void addKnob(juce::Slider* knob, juce::Label* label)
    {
        knobs.add(knob);
        knobLabels.add(label);
        addAndMakeVisible(*knob);
        addAndMakeVisible(*label);
    }

    void addCombo(juce::ComboBox* combo, juce::Label* label)
    {
        combos.add(combo);
        comboLabels.add(label);
        addAndMakeVisible(*combo);
        addAndMakeVisible(*label);
    }

    void addButton(juce::ToggleButton* button)
    {
        buttons.add(button);
        addAndMakeVisible(*button);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(25); // Header space

        int totalControls = combos.size() + knobs.size() + buttons.size();
        if (totalControls == 0) return;

        float controlWidth = (float)bounds.getWidth() / (float)totalControls;

        // Layout combos first (they appear first in Material section)
        for (int i = 0; i < combos.size(); ++i)
        {
            auto controlBounds = bounds.removeFromLeft((int)controlWidth).reduced(5);
            comboLabels[i]->setBounds(controlBounds.removeFromTop(15));
            combos[i]->setBounds(controlBounds.removeFromTop(25));
        }

        // Then knobs
        for (int i = 0; i < knobs.size(); ++i)
        {
            auto controlBounds = bounds.removeFromLeft((int)controlWidth).reduced(5);
            knobLabels[i]->setBounds(controlBounds.removeFromTop(15));
            knobs[i]->setBounds(controlBounds);
        }

        // Then buttons
        for (auto* button : buttons)
        {
            auto controlBounds = bounds.removeFromLeft((int)controlWidth).reduced(5);
            button->setBounds(controlBounds);
        }
    }

private:
    juce::String name;
    juce::String icon;

    juce::Array<juce::Slider*> knobs;
    juce::Array<juce::Label*> knobLabels;
    juce::Array<juce::ComboBox*> combos;
    juce::Array<juce::Label*> comboLabels;
    juce::Array<juce::ToggleButton*> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSection)
};
