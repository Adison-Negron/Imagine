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
#include "SliderWindow.h"

namespace fs = std::filesystem;
class SliderWindow;
//==============================================================================
/**
*/
class ImagineAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::FileDragAndDropTarget, private juce::Slider::Listener
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


    juce::GroupComponent generation_sliders;


    void playButtonClicked();
    void stopButtonClicked();
    void changeState(TransportState newState);
    TransportState state;
    void playWavFile();
    void addSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double default);

    juce::File getImageFile()
    {
        return this->imageFile;
    }

    juce::File setImageFile(juce::File imageFile)
    {
        this->imageFile = imageFile;
    }
    void generateSound();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ImagineAudioProcessor& audioProcessor;
    juce::File createFolderIfNotExists(const juce::File& parentFolder, const std::string& folderName);


    void sliderValueChanged(juce::Slider* slider) override;
    juce::File imageFile;
    SliderWindow* windowComponent;
    std::string imagePath;

    std::unique_ptr<juce::DocumentWindow> slider_window;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImagineAudioProcessorEditor)
};
