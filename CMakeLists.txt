cmake_minimum_required(VERSION 3.22)

project(MoogLadderPlugin VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(JUCE_MODULES_DIR "C:/Program Files/JUCE/modules")
set(JUCE_DISABLE_JUCE_AUDIO_PROCESSORS_HEADLESS_ENABLED OFF)
include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/develop
)
set(JUCE_VST3_CAN_REPLACE_VST2 OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(JUCE)
# add_subdirectory("C:/Program Files/JUCE" JUCE)

set(JUCE_ENABLE_MODULE_SOURCE_GROUPS OFF)

juce_add_plugin(MoogLadderPlugin
    PLUGIN_NAME "Corporate Ladder Filter"
    PLUGIN_MANUFACTURER_CODE Saha
    PLUGIN_CODE SCLF
    FORMATS VST3 Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    NEEDS_WEB_BROWSER FALSE
    VST3_COPY_DIR "$ENV{APPDATA}/VST3"
)

juce_generate_juce_header(MoogLadderPlugin)

target_sources(MoogLadderPlugin PRIVATE
    Source/MoogLadderPlugin.cpp
    Source/MoogLadderPluginPluginProcessor.cpp
    Source/rt_nonfinite.cpp
    Source/rtGetInf.cpp
    Source/rtGetNaN.cpp
)

target_include_directories(MoogLadderPlugin PRIVATE
    Source
    ${CMAKE_CURRENT_BINARY_DIR}/MoogLadderPlugin_artefacts/JuceLibraryCode
)

target_compile_definitions(MoogLadderPlugin PRIVATE
    JUCE_VST_CAN_REPLACE_VST2=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_PLUGINHOST_VST3=0
    JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=0
)

target_link_libraries(MoogLadderPlugin PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)