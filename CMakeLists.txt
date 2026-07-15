cmake_minimum_required(VERSION 3.22)
project(Bloom VERSION 0.1.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6
  GIT_SHALLOW TRUE
)

set(BLOOM_NAM_CORE_COMMIT 3cde95c354d5ba6da01316cad90b05cfc4855053)
FetchContent_Declare(
  NeuralAmpModelerCore
  GIT_REPOSITORY https://github.com/sdatkinson/NeuralAmpModelerCore.git
  GIT_TAG ${BLOOM_NAM_CORE_COMMIT}
)

FetchContent_MakeAvailable(JUCE NeuralAmpModelerCore)

file(GLOB NAM_CORE_TOP CONFIGURE_DEPENDS
  "${neuralampmodelercore_SOURCE_DIR}/NAM/*.cpp"
  "${neuralampmodelercore_SOURCE_DIR}/NAM/*.c"
)
file(GLOB NAM_CORE_SUB CONFIGURE_DEPENDS
  "${neuralampmodelercore_SOURCE_DIR}/NAM/*/*.cpp"
)

add_library(BloomNamCore STATIC ${NAM_CORE_TOP} ${NAM_CORE_SUB})
target_compile_features(BloomNamCore PUBLIC cxx_std_20)
target_compile_definitions(BloomNamCore PUBLIC NAM_SAMPLE_FLOAT NAM_ENABLE_A2_FAST NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(BloomNamCore SYSTEM PUBLIC
  "${neuralampmodelercore_SOURCE_DIR}"
  "${neuralampmodelercore_SOURCE_DIR}/NAM"
  "${neuralampmodelercore_SOURCE_DIR}/Dependencies/eigen"
  "${neuralampmodelercore_SOURCE_DIR}/Dependencies/nlohmann"
)

# First prove the NAM-powered application builds and runs as a standalone.
# VST3 will be restored after the JUCE manifest step is handled cleanly.
juce_add_plugin(Bloom
  COMPANY_NAME "Bloom"
  BUNDLE_ID "audio.bloom.player"
  PLUGIN_MANUFACTURER_CODE Blom
  PLUGIN_CODE Blm1
  FORMATS Standalone
  PRODUCT_NAME "Bloom"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(Bloom PRIVATE
  src/PluginProcessor.cpp
  src/PluginProcessor.h
  src/PluginEditor.cpp
  src/PluginEditor.h
  src/audio/NamEngine.cpp
  src/audio/NamEngine.h
)

target_compile_definitions(Bloom PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 NAM_SAMPLE_FLOAT)
target_link_libraries(Bloom PRIVATE BloomNamCore juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
juce_generate_juce_header(Bloom)
