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
    setSize (400, 300);
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
}

ImagineAudioProcessorEditor::~ImagineAudioProcessorEditor()
{
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
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
    imgsFolder = createFolderIfNotExists(mainFolder, "imgs");
    outputFolder = createFolderIfNotExists(mainFolder, "output");
    imgsPath = imgsFolder.getFullPathName().toStdString();
    outputPath = outputFolder.getFullPathName().toStdString() + "\\";
    
    //deleteFiles(imgsFolder);
    //deleteFiles(outputFolder);

    for (const auto& file : files)
    {
        DBG("Dropped file: " << file);
        juce::File imageFile(file);

        if (imageFile.copyFileTo(imgsFolder.getChildFile(imageFile.getFileName())))
        {
            DBG("File saved to " << imgsFolder.getChildFile(imageFile.getFileName()).getFullPathName());
            audioProcessor.callPythonFunction(imgsPath,
                outputPath,
                25, 10, 1, 44800, 6, 5, 0.8f, 0.5f, 1.0f, 1.0f, 1.0f, 0.7f, 1.0f);
        }
        else
        {
            DBG("Failed to save file.");
        }
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
    juce::DirectoryIterator iter(outputFolder, false, "*.wav", juce::File::findFiles);

    if (iter.next())
    {
        juce::File wavFile = iter.getFile();

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
