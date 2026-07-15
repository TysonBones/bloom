#include "PluginEditor.h"

BloomAudioProcessorEditor::BloomAudioProcessorEditor(BloomAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(760, 460);

    title.setText("RADIANCE", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centred);
    title.setFont(juce::FontOptions(30.0f, juce::Font::bold));

    modelStatus.setText("No model loaded", juce::dontSendNotification);
    modelStatus.setJustificationType(juce::Justification::centred);

    for (auto* slider : {&input, &output})
    {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 24);
        slider->setTextValueSuffix(" dB");
        addAndMakeVisible(*slider);
    }

    loadButton.onClick = [this]
    {
        chooser = std::make_unique<juce::FileChooser>("Choose an authorised NAM model", juce::File(), "*.nam");
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                const auto file = fc.getResult();
                if (file == juce::File()) return;
                juce::String error;
                modelStatus.setText(processor.loadNamModel(file, error) ? file.getFileName() : error, juce::dontSendNotification);
            });
    };

    addAndMakeVisible(title);
    addAndMakeVisible(modelStatus);
    addAndMakeVisible(loadButton);

    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "inputDb", input);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, "outputDb", output);
}

void BloomAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(juce::Colour(0xff18112d), 0.0f, 0.0f, juce::Colour(0xff402754), (float)getWidth(), (float)getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    g.setColour(juce::Colour(0xffffdf70));
    g.drawRoundedRectangle(getLocalBounds().reduced(24).toFloat(), 24.0f, 2.0f);
}

void BloomAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(40);
    title.setBounds(area.removeFromTop(50));
    modelStatus.setBounds(area.removeFromTop(40));
    loadButton.setBounds(area.removeFromTop(44).withSizeKeepingCentre(180, 36));
    auto controls = area.reduced(100, 30);
    input.setBounds(controls.removeFromLeft(180));
    output.setBounds(controls.removeFromRight(180));
}
