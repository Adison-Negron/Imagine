/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif


//==============================================================================
/**
*/
class ImagineAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ImagineAudioProcessor();
    ~ImagineAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadSound(juce::File& soundpath);
    void callPythonFunction(const std::string& img_path,
        const std::string& out_path,
        int kernel_size,
        int step_size,
        int sound_level,
<<<<<<< HEAD
        double sampleRate,
=======
>>>>>>> 994086f4f71c25c16cef09c9d438c2e5723991d9
        int sound_duration,
        int modulation_duration,
        float modulation_intensity,
        float modulation_envelope_intensity,
        float overtone_num_scalar,
        float lfo_scalar_freq, 
        float lfo_scalar_amplitude,
        float lfo_intensity,
        float lfo_amount_scalar);
    std::string pythonpath; 
    juce::File currentPath;
    juce::File root;
    juce::File outputpath;

private:
    juce::Synthesiser mSampler;
    const int mNumVoices{ 128 };
    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImagineAudioProcessor)


};
