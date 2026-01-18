#pragma once

#include <JuceHeader.h>

class WaveformDisplay : public juce::Component,
                        public juce::FileDragAndDropTarget
{
public:
    WaveformDisplay()
    {
        setInterceptsMouseClicks(true, false);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();

        // Background
        g.fillAll(juce::Colour(0xff0f0f0f));

        // Border
        g.setColour(juce::Colour(0xff00d4ff));
        g.drawRect(bounds, 1);

        if (audioBuffer.getNumSamples() > 0)
        {
            // Draw waveform
            auto waveformBounds = bounds.reduced(4);
            drawWaveform(g, waveformBounds);
        }
        else
        {
            // Draw placeholder text
            g.setColour(juce::Colour(0xff666666));
            g.setFont(14.0f);
            g.drawText("Drop audio file here or record", bounds, juce::Justification::centred);
        }
    }

    void drawWaveform(juce::Graphics& g, juce::Rectangle<int> bounds)
    {
        if (audioBuffer.getNumSamples() == 0)
            return;

        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto centerY = bounds.getY() + height / 2.0f;

        juce::Path waveformPath;
        auto numSamples = audioBuffer.getNumSamples();
        auto samplesPerPixel = juce::jmax(1, numSamples / width);
        auto* channelData = audioBuffer.getReadPointer(0);

        for (int x = 0; x < width; ++x)
        {
            auto startSample = x * samplesPerPixel;
            auto endSample = juce::jmin(startSample + samplesPerPixel, numSamples);

            auto min = 0.0f;
            auto max = 0.0f;

            for (int i = startSample; i < endSample; ++i)
            {
                auto sample = channelData[i];
                min = juce::jmin(min, sample);
                max = juce::jmax(max, sample);
            }

            auto yMin = centerY - min * (height / 2.0f) * 0.9f;
            auto yMax = centerY - max * (height / 2.0f) * 0.9f;

            if (x == 0)
                waveformPath.startNewSubPath(bounds.getX() + x, yMax);
            else
                waveformPath.lineTo(bounds.getX() + x, yMax);
        }

        // Draw bottom half
        for (int x = width - 1; x >= 0; --x)
        {
            auto startSample = x * samplesPerPixel;
            auto endSample = juce::jmin(startSample + samplesPerPixel, numSamples);

            auto min = 0.0f;

            for (int i = startSample; i < endSample; ++i)
            {
                auto sample = channelData[i];
                min = juce::jmin(min, sample);
            }

            auto yMin = centerY - min * (height / 2.0f) * 0.9f;
            waveformPath.lineTo(bounds.getX() + x, yMin);
        }

        waveformPath.closeSubPath();

        g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
        g.fillPath(waveformPath);

        g.setColour(juce::Colour(0xff00d4ff));
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
    }

    void setAudioBuffer(const juce::AudioBuffer<float>& buffer)
    {
        audioBuffer = buffer;
        repaint();
    }

    // Drag and drop implementation
    bool isInterestedInFileDrag(const juce::StringArray& files) override
    {
        for (auto& file : files)
        {
            if (file.endsWithIgnoreCase(".wav") ||
                file.endsWithIgnoreCase(".aif") ||
                file.endsWithIgnoreCase(".aiff") ||
                file.endsWithIgnoreCase(".mp3") ||
                file.endsWithIgnoreCase(".flac"))
                return true;
        }
        return false;
    }

    void filesDropped(const juce::StringArray& files, int, int) override
    {
        if (files.isEmpty())
            return;

        juce::File audioFile(files[0]);
        loadAudioFile(audioFile);
    }

    void loadAudioFile(const juce::File& file)
    {
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

        if (reader != nullptr)
        {
            audioBuffer.setSize(static_cast<int>(reader->numChannels),
                              static_cast<int>(reader->lengthInSamples));
            reader->read(&audioBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
            repaint();

            // Notify parent/processor that a file was loaded
            if (onFileLoaded)
                onFileLoaded(file);
        }
    }

    std::function<void(const juce::File&)> onFileLoaded;

private:
    juce::AudioBuffer<float> audioBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
