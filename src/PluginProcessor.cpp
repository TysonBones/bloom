#include "PluginProcessor.h"
#include "PluginEditor.h"

BloomAudioProcessor::BloomAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::mono(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "BLOOM_STATE", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BloomAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"inputDb", 1}, "Input", juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"outputDb", 1}, "Output", juce::NormalisableRange<float>(-36.0f, 12.0f, 0.1f), -12.0f));
    return {p.begin(), p.end()};
}

void BloomAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    namEngine.prepare(sampleRate, samplesPerBlock);
}

bool BloomAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void BloomAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto* mono = buffer.getWritePointer(0);
    const auto inGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("inputDb")->load());
    const auto outGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("outputDb")->load());
    juce::FloatVectorOperations::multiply(mono, inGain, buffer.getNumSamples());
    namEngine.processMono(mono, buffer.getNumSamples());
    juce::FloatVectorOperations::multiply(mono, outGain, buffer.getNumSamples());
    if (buffer.getNumChannels() > 1)
        buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
}

bool BloomAudioProcessor::loadNamModel(const juce::File& file, juce::String& error)
{
    return namEngine.loadModel(file, error);
}

juce::String BloomAudioProcessor::getLoadedModelPath() const
{
    return namEngine.getLoadedModelPath();
}

void BloomAudioProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    auto state = parameters.copyState();
    state.setProperty("modelPath", namEngine.getLoadedModelPath(), nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary(*xml, destination);
}

void BloomAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid() && state.hasType(parameters.state.getType()))
        {
            parameters.replaceState(state);
            const auto path = state.getProperty("modelPath").toString();
            if (path.isNotEmpty()) { juce::String ignored; namEngine.loadModel(juce::File(path), ignored); }
        }
    }
}

juce::AudioProcessorEditor* BloomAudioProcessor::createEditor() { return new BloomAudioProcessorEditor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new BloomAudioProcessor(); }
