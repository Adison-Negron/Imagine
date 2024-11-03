/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

//==============================================================================
ImagineAudioProcessor::ImagineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations


    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )





#endif
{
    Py_Initialize();
    currentPath = juce::File::getCurrentWorkingDirectory();
    root = currentPath;

    while (root.exists() && !root.getFileName().equalsIgnoreCase("Imagine"))
    {
        root = root.getParentDirectory();
    }

    if (root.getFileName().equalsIgnoreCase("Imagine"))
    {
        juce::File pythonScriptDir = root.getChildFile("Python/conversionmodules");
        pythonpath = pythonScriptDir.getFullPathName().toStdString();

        PyObject* sysPath = PySys_GetObject("path");
        if (sysPath && PyList_Check(sysPath)) {
            PyObject* pyPath = PyUnicode_FromString(pythonpath.c_str());
            PyList_Append(sysPath, pyPath);
            Py_DECREF(pyPath);
        }
    }
}
ImagineAudioProcessor::~ImagineAudioProcessor()
{
    Py_Finalize();
}

//==============================================================================
const juce::String ImagineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ImagineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ImagineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ImagineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ImagineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ImagineAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ImagineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ImagineAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ImagineAudioProcessor::getProgramName (int index)
{
    return {};
}

void ImagineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ImagineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ImagineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ImagineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ImagineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool ImagineAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ImagineAudioProcessor::createEditor()
{
    return new ImagineAudioProcessorEditor (*this);
}

//==============================================================================
void ImagineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ImagineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ImagineAudioProcessor();
}


void ImagineAudioProcessor::callPythonFunction(const std::string& img_path,
    const std::string& out_path,
    int kernel_size,
    int step_size,
    int sound_level,
    int sample_rate,
    int sound_duration,
    int modulation_duration,
    float modulation_intensity,
    float modulation_envelope_intensity,
    float overtone_num_scalar,
    float lfo_scalar_freq,
    float lfo_scalar_amplitude,
    float lfo_intensity,
    float lfo_amount_scalar)
{
    PyObject* pName, * pModule, *pDict, * pFunc, * pArgs, * pValue;

    // Set the Python script name 
    pName = PyUnicode_FromString("imagetoaudio");

    // Import the Python module
    pModule = PyImport_Import(pName);


    Py_DECREF(pName);

    if (pModule != nullptr)
    {
        // Get the function from the module
        pFunc = PyObject_GetAttrString(pModule,(char*) "main_generation_handler");

        // Check if the function is callable
        if (pFunc && PyCallable_Check(pFunc))
        {
            // Create the arguments for the Python function
            PyObject* pArgs = PyTuple_Pack(15,
                PyUnicode_FromString(img_path.c_str()),
                PyUnicode_FromString(out_path.c_str()),
                PyLong_FromLong(kernel_size),
                PyLong_FromLong(step_size),
                PyLong_FromLong(sound_level),
                PyLong_FromLong(sample_rate),
                PyLong_FromLong(sound_duration),
                PyLong_FromLong(modulation_duration),
                PyFloat_FromDouble(modulation_intensity),
                PyFloat_FromDouble(modulation_envelope_intensity),
                PyFloat_FromDouble(overtone_num_scalar),
                PyFloat_FromDouble(lfo_scalar_freq),
                PyFloat_FromDouble(lfo_scalar_amplitude),
                PyFloat_FromDouble(lfo_intensity),
                PyFloat_FromDouble(lfo_amount_scalar)
            );

            // Call the function
            pValue = PyObject_CallObject(pFunc, pArgs);
            double pOut = PyFloat_AsDouble(pValue);

            if (pValue != nullptr)
            {
                // Print the return value
                juce::Logger::outputDebugString(PyUnicode_AsUTF8(pValue));
                Py_DECREF(pValue);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                Py_DECREF(pArgs);
                PyErr_Print();
                juce::Logger::outputDebugString("Call to Python function failed");
                return;
            }
            Py_DECREF(pArgs);
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            juce::Logger::outputDebugString("Cannot find function main_generation_handler");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else
    {
        PyErr_Print();
        juce::Logger::outputDebugString("Failed to load");
    }
}
