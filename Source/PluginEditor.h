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

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ImagineAudioProcessor& audioProcessor;
    juce::File createFolderIfNotExists(const juce::File& parentFolder, const std::string& folderName);

    juce::Slider kernel, step, sound_level, sample_rate, sound_duration, modulation_duration, modulation_intensity,
        modulation_envelope_intensity, overtone_num_scalar, lfo_scalar_freq, lfo_scalar_amplitude, lfo_intensity, lfo_amount_scalar;
    juce::Label kernel_label, step_label, sound_label, sample_label, duration_label, modulation_duration_label, modulation_intensity_label,
        envelope_intensity_label, overtone_num_label, lfo_freq_label , lfo_amplitude_label, lfo_intensity_label, lfo_amount_label;

    void sliderValueChanged(juce::Slider* slider) override;
    juce::File imageFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImagineAudioProcessorEditor)
};
