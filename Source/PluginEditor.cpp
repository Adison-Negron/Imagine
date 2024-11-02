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
    

}

ImagineAudioProcessorEditor::~ImagineAudioProcessorEditor()
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

void ImagineAudioProcessorEditor::filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/)
{
    for (const auto& file : files)
    {
        DBG("Dropped file: " << file);

        juce::File destinationFolder("C:/Users/minec/OneDrive/Desktop/Imagine/Python/conversionmodules/imgs/");
        juce::File imageFile(file);

        if (imageFile.copyFileTo(destinationFolder.getChildFile(imageFile.getFileName())))
        {
            DBG("File saved to " << destinationFolder.getChildFile(imageFile.getFileName()).getFullPathName());
            audioProcessor.callPythonFunction(destinationFolder.getFullPathName().toStdString(),
                "C:/Users/minec/OneDrive/Desktop/Imagine/Python/conversionmodules/output/",
                25, 10, 1, 44800, 6, 5, 0.8f, 0.5f, 1.0f, 1.0f, 1.0f, 0.7f, 1.0f);
        }
        else
        {
            DBG("Failed to save file.");
        }
    }
}

void ImagineAudioProcessorEditor::saveImageFile(const juce::File& imageFile)
{

}

void ImagineAudioProcessorEditor::playButtonClicked()
{
    if (transportSource.isPlaying())
    {
        transportSource.stop();
    }
    else
    {
        transportSource.setPosition(0);
        transportSource.start();
    }
}
