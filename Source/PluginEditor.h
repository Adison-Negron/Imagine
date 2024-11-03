/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

//==============================================================================
/**
*/
class ImagineAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::FileDragAndDropTarget
{
public:
    ImagineAudioProcessorEditor (ImagineAudioProcessor&);
    ~ImagineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;


    
    juce::File documentsDir;
    juce::File mainFolder;
    juce::File imgsFolder;
    juce::File outputFolder;
    std::string imgsPath;
    std::string outputPath;

    void deleteFiles(const juce::File& folder);

    enum TransportState
    {
        Starting,
        Stopping
    };

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioDeviceManager deviceManager;

    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };

    void playButtonClicked();
    void stopButtonClicked();
    void changeState(TransportState newState);
    TransportState state;
    void playWavFile();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ImagineAudioProcessor& audioProcessor;
    juce::File createFolderIfNotExists(const juce::File& parentFolder, const std::string& folderName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImagineAudioProcessorEditor)
};
