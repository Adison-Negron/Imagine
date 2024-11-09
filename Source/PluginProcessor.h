/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include "Pyembedder.h"
#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

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
    bool firstload = true;
    std::unique_ptr<juce::AudioBuffer<float>> mainbuffer;
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>> effectsChain;



    //Parameter controls
    //===================================================================
    //Gain==============================
    void setGain(float gainValue) { currentGain = gainValue; }
    float currentGain = 1.0f;
    

    
;


    void callPythonFunction(const std::string& img_path,

        const std::string& out_path,
        int kernel_size,
        int step_size,
        int sound_level,
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
    juce::Synthesiser mSampler;
    const int mNumVoices{ 128 };

private:

    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };
    
    std::unique_ptr<Pyembedder> pyEmbedder;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImagineAudioProcessor)


};
