/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ImagineAudioProcessorEditor::ImagineAudioProcessorEditor (ImagineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1280, 720);
    state = (Stopping);

    addAndMakeVisible(&playButton);
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);



    addAndMakeVisible(&stopButton);
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);



    formatManager.registerBasicFormats();
    deviceManager.initialiseWithDefaultDevices(0, 2);
    deviceManager.addAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(&transportSource);

    addSlider(kernel, kernel_label, "Kernel Size", 0, 500, 25);
    addSlider(step, step_label, "Step Size", 0, 50, 10);
    addSlider(sound_level, sound_label, "Sound Level", 0, 10, 1);
    addSlider(sample_rate, sample_label, "Sample Rate", 0, 44800, 44800);
    addSlider(sound_duration, duration_label, "Sound Duration", 0, 20, 6);
    addSlider(modulation_intensity, modulation_intensity_label, "Modulation Intensity", 0, 1, .8);
    addSlider(modulation_envelope_intensity, envelope_intensity_label, "Envelope Intensity", 0, 1, .2);
    addSlider(modulation_duration, modulation_duration_label, "Modulation Duration", 0, 20, 6);
    addSlider(lfo_scalar_freq, lfo_freq_label, "Lfo Frequency", 0, 1, .6);
    addSlider(lfo_scalar_amplitude, lfo_amplitude_label, "Lfo Amplitude", 0, 1, 1);
    addSlider(lfo_intensity, lfo_intensity_label, "Lfo Intensity", 0, 1, 1);
    addSlider(overtone_num_scalar, overtone_num_label, "Overtone Num", 0, 20, 1);
    addSlider(lfo_amount_scalar, lfo_amount_label, "Lfo Amount", 0, 20, 1);

    kernel.addListener(this);
    step.addListener(this);
    sound_level.addListener(this);
    sample_rate.addListener(this);
    sound_duration.addListener(this);
    modulation_intensity.addListener(this);
    modulation_envelope_intensity.addListener(this);
    modulation_duration.addListener(this);
    lfo_scalar_freq.addListener(this);
    lfo_scalar_amplitude.addListener(this);
    lfo_intensity.addListener(this);
    overtone_num_scalar.addListener(this);
    lfo_amount_scalar.addListener(this);


}

ImagineAudioProcessorEditor::~ImagineAudioProcessorEditor()
{
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
}


void ImagineAudioProcessorEditor::addSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double default)
{
    slider.setRange(min, max);
    slider.setValue(default);
    slider.setTextValueSuffix(name);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.attachToComponent(&slider, true);
    addAndMakeVisible(label);
}

void ImagineAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{

}

//==============================================================================
void ImagineAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ImagineAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    playButton.setBounds(10, 10, 100, 30);
    stopButton.setBounds(120, 10, 100, 30);

    const int sliderHeight = 20;
    const int labelWidth = 100;
    const int spacing = 5;
    int y = getHeight() / 2; 

    auto positionSliderAndLabel = [&](juce::Slider& slider, juce::Label& label) {
        label.setBounds(10, y, labelWidth, sliderHeight);
        slider.setBounds(labelWidth + spacing, y, getWidth() - labelWidth - spacing * 2, sliderHeight);
        y += sliderHeight + spacing;
    };

    positionSliderAndLabel(kernel, kernel_label);
    positionSliderAndLabel(step, step_label);
    positionSliderAndLabel(sound_level, sound_label);
    positionSliderAndLabel(sample_rate, sample_label);
    positionSliderAndLabel(sound_duration, duration_label);
    positionSliderAndLabel(modulation_intensity, modulation_duration_label);
    positionSliderAndLabel(modulation_envelope_intensity, envelope_intensity_label);
    positionSliderAndLabel(modulation_duration, modulation_duration_label);
    positionSliderAndLabel(lfo_scalar_freq, lfo_freq_label);
    positionSliderAndLabel(lfo_scalar_amplitude, lfo_amplitude_label);
    positionSliderAndLabel(lfo_intensity, lfo_intensity_label);
    positionSliderAndLabel(overtone_num_scalar, overtone_num_label);
    positionSliderAndLabel(lfo_amount_scalar, lfo_amount_label);


}

bool ImagineAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    // Accept only image files
    for (auto& file : files)
    {
        if (file.endsWithIgnoreCase(".jpg") || file.endsWithIgnoreCase(".png"))
            return true;
    }
    return false;
}

void ImagineAudioProcessorEditor::deleteFiles(const juce::File& folder)
{
    juce::DirectoryIterator iter(folder, false, "*", juce::File::findFiles);

    while (iter.next())
    {
        juce::File file = iter.getFile();
        if (file.existsAsFile())
        {
            file.deleteFile();
        }
    }
}

void ImagineAudioProcessorEditor::filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/)
{

    documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    mainFolder = createFolderIfNotExists(documentsDir, "Imagine");
    outputFolder = createFolderIfNotExists(mainFolder, "output");
    outputPath = outputFolder.getFullPathName().toStdString() + "\\";
    
    for (const auto& file : files)
    {
        juce::File imageFile(file);
        std::string imagePath = imageFile.getFullPathName().toStdString();

            audioProcessor.callPythonFunction(imagePath,
                outputPath,
                kernel.getValue(),
                step.getValue(),
                sound_level.getValue(),
                sample_rate.getValue(),
                sound_duration.getValue(),
                modulation_intensity.getValue(),
                modulation_envelope_intensity.getValue(),
                modulation_duration.getValue(),
                lfo_scalar_freq.getValue(),
                lfo_scalar_amplitude.getValue(),
                lfo_intensity.getValue(),
                overtone_num_scalar.getValue(),
                lfo_amount_scalar.getValue());

    }
}

juce::File ImagineAudioProcessorEditor::createFolderIfNotExists(const juce::File& parentFolder, const std::string& folderName)
{
    juce::File folder = parentFolder.getChildFile(folderName);

    if (!folder.exists())
    {
        bool success = folder.createDirectory();
        if (success)
        {
            DBG(folderName + " folder created successfully.");
        }
        else
        {
            DBG("Failed to create " + folderName + " folder.");
        }
    }
    else
    {
        DBG(folderName + " folder already exists.");
    }

    return folder;
}

void ImagineAudioProcessorEditor::playButtonClicked()
{
    changeState(Starting);
}

void ImagineAudioProcessorEditor::stopButtonClicked()
{
    changeState(Stopping);
}

void ImagineAudioProcessorEditor::playWavFile()
{
    juce::File wavFile = audioProcessor.outputpath;

    if (wavFile.existsAsFile())
    {
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(wavFile));

        if (reader != nullptr)
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader.release(), true));
            transportSource.setSource(newSource.get(), 0, nullptr, 44800);
            readerSource.reset(newSource.release());
            transportSource.start();
        }
    }
}

void ImagineAudioProcessorEditor::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case Starting:
            playWavFile();
            transportSource.start();
            break;

        case Stopping:
            transportSource.stop();
            break;
        }
    }
}
