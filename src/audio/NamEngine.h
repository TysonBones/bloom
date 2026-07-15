#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <NAM/dsp.h>
#include <NAM/get_dsp.h>
#include <atomic>
#include <memory>

namespace bloom
{
class NamEngine
{
public:
    void prepare(double sampleRate, int maximumBlockSize);
    bool loadModel(const juce::File& modelFile, juce::String& errorMessage);
    void processMono(float* samples, int numSamples) noexcept;
    juce::String getLoadedModelPath() const;

private:
    using ModelPtr = std::shared_ptr<nam::DSP>;
    std::atomic<ModelPtr> activeModel;
    juce::String loadedModelPath;
    double currentSampleRate { 48000.0 };
    int currentMaximumBlockSize { 128 };
};
}
