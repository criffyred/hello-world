#pragma once

#include <JuceHeader.h>

class TorsoS4LookAndFeel : public juce::LookAndFeel_V4
{
public:
    TorsoS4LookAndFeel()
    {
        // S-4 color scheme - dark with cyan/white accents
        bgDark = juce::Colour(0xff0a0a0a);           // Very dark background
        bgMedium = juce::Colour(0xff1a1a1a);         // Medium dark for controls
        accentColor = juce::Colour(0xff00d4ff);      // Cyan/turquoise accent
        textColor = juce::Colour(0xffe0e0e0);        // Light gray text
        knobFill = juce::Colour(0xff2a2a2a);         // Dark knob fill

        setColour(juce::Slider::thumbColourId, accentColor);
        setColour(juce::Slider::rotarySliderFillColourId, accentColor);
        setColour(juce::Slider::rotarySliderOutlineColourId, bgMedium);
        setColour(juce::Slider::textBoxTextColourId, textColor);
        setColour(juce::Slider::textBoxBackgroundColourId, bgDark);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

        setColour(juce::Label::textColourId, textColor);
        setColour(juce::ComboBox::backgroundColourId, bgMedium);
        setColour(juce::ComboBox::textColourId, textColor);
        setColour(juce::ComboBox::outlineColourId, accentColor);

        setColour(juce::TextButton::buttonColourId, bgMedium);
        setColour(juce::TextButton::buttonOnColourId, accentColor);
        setColour(juce::TextButton::textColourOffId, textColor);
        setColour(juce::TextButton::textColourOnId, bgDark);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // Outer ring (track)
        g.setColour(bgMedium);
        g.fillEllipse(rx, ry, rw, rw);

        // Inner fill
        g.setColour(knobFill);
        g.fillEllipse(rx + 3, ry + 3, rw - 6, rw - 6);

        // Arc showing value
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius - 2, radius - 2,
                              0.0f, rotaryStartAngle, angle, true);
        g.setColour(accentColor);
        g.strokePath(valueArc, juce::PathStrokeType(3.0f));

        // Pointer
        juce::Path pointer;
        auto pointerLength = radius * 0.6f;
        auto pointerThickness = 3.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius + 8, pointerThickness, pointerLength);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(accentColor);
        g.fillPath(pointer);

        // Center dot
        g.setColour(bgDark);
        g.fillEllipse(centreX - 4, centreY - 4, 8, 8);
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font(11.0f);
    }

private:
    juce::Colour bgDark, bgMedium, accentColor, textColor, knobFill;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TorsoS4LookAndFeel)
};
