#include "NamEngine.h"

namespace bloom
{
void NamEngine::prepare(double sampleRate, int maximumBlockSize)
{
    currentSampleRate = sampleRate;
    currentMaximumBlockSize = maximumBlockSize;
    if (auto current = activeModel.load(std::memory_order_acquire))
        current->Reset(currentSampleRate, currentMaximumBlockSize);
}

bool NamEngine::loadModel(const juce::File& modelFile, juce::String& errorMessage)
{
    if (!modelFile.existsAsFile())
    {
        errorMessage = "The amplifier model could not be found.";
        return false;
    }

    try
    {
        nam::DspLoadOptions options;
        options.prewarm = false;
        auto unique = nam::get_dsp(std::filesystem::path(modelFile.getFullPathName().toStdString()), options);
        if (unique == nullptr)
        {
            errorMessage = "The amplifier model could not be created.";
            return false;
        }

        unique->SetPrewarmOnReset(true);
        unique->Reset(currentSampleRate, currentMaximumBlockSize);
        ModelPtr prepared(std::move(unique));
        activeModel.store(prepared, std::memory_order_release);
        loadedModelPath = modelFile.getFullPathName();
        errorMessage.clear();
        return true;
    }
    catch (const std::exception& e)
    {
        errorMessage = "Could not load this NAM model: " + juce::String(e.what());
        return false;
    }
}

void NamEngine::processMono(float* samples, int numSamples) noexcept
{
    auto current = activeModel.load(std::memory_order_acquire);
    if (current == nullptr)
        return;

    float* channels[] { samples };
    current->process(channels, channels, numSamples);
}

juce::String NamEngine::getLoadedModelPath() const
{
    return loadedModelPath;
}
}
