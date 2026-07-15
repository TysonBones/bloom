#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class BloomAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit BloomAudioProcessorEditor(BloomAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BloomAudioProcessor& processor;
    juce::Label title;
    juce::Label modelStatus;
    juce::TextButton loadButton { "Load NAM" };
    juce::Slider input;
    juce::Slider output;
    std::unique_ptr<juce::FileChooser> chooser;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BloomAudioProcessorEditor)
};
