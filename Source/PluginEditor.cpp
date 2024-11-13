/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ImagineAudioProcessorEditor::ImagineAudioProcessorEditor (ImagineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), thumbnailCache(1),  // Initialize the thumbnail cache with a cache size of 5
    thumbnail(1024, formatManager, thumbnailCache)
{   
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize (1280, 720);
    state = (Stopping);
    setResizable(false, false);

    //addAndMakeVisible(&playButton);
    //playButton.onClick = [this] { playButtonClicked(); };
    //playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);




    //addAndMakeVisible(&stopButton);
    //stopButton.onClick = [this] { stopButtonClicked(); };
    //stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
       
    //Audio thumbnail init


    // Optionally, add more items to mainFlexBox as needed
    // 
    // 
    // 
    // 
    // Buttons:
    // 
    // 
    // 
    //---------------------------------------------------------//

     // Initialize knobs
    addAndMakeVisible(paramslider1);
    paramslider1.setSliderStyle(juce::Slider::Rotary);
    paramslider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    paramslider1.setRange(0.0, 1.0); // Example range
    paramslider1.setValue(0.5);       // Default value

    addAndMakeVisible(paramslider2);
    paramslider2.setSliderStyle(juce::Slider::Rotary);
    paramslider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    paramslider2.setRange(0.0, 1.0);
    paramslider2.setValue(0.5);

    addAndMakeVisible(paramslider3);
    paramslider3.setSliderStyle(juce::Slider::Rotary);
    paramslider3.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    paramslider3.setRange(0.0, 1.0);
    paramslider3.setValue(0.5);

    addAndMakeVisible(paramslider4);
    paramslider4.setSliderStyle(juce::Slider::Rotary);
    paramslider4.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    paramslider4.setRange(0.0, 1.0);
    paramslider4.setValue(0.5);



    gainSlider.setRange(0.0, 10.0, 0.05);  // Set range from 0 (mute) to 1 (full gain)
    gainSlider.setValue(1);             // Default gain value
    gainSlider.addListener(this);         // Make this editor a listener
    addAndMakeVisible(gainSlider);

 

    // Gain label setup
  
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    gainSlider.setColour(juce::Slider::backgroundColourId, charcoal);
    gainSlider.setColour(juce::Slider::textBoxTextColourId, charcoal);
    gainSlider.setColour(juce::Slider::textBoxBackgroundColourId, ivory);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainSlider.setSliderStyle(juce::Slider::Rotary);


    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,100,40);


    gainLabel.attachToComponent(&gainSlider, false);

    addAndMakeVisible(gainLabel);


    //--------------------------------------------------------------------------------------------//



    formatManager.registerBasicFormats();
    deviceManager.initialiseWithDefaultDevices(0, 2);
    deviceManager.addAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(&transportSource);

    slider_window = std::make_unique<juce::DocumentWindow>("Generation Parameters", juce::Colours::lightgrey, juce::DocumentWindow::closeButton | juce::DocumentWindow::minimiseButton);
    windowComponent = new SliderWindow(this);
    slider_window->setContentOwned(windowComponent, true);
    slider_window->setVisible(false);
    slider_window->setBounds(0, 0, 400, 950);

    bounds = getLocalBounds();
    topBounds = bounds.removeFromTop(bounds.getHeight() * 0.4);

    startPosition = 0;
    endPosition = 480000;



    reverbRoomSize.setSliderStyle(juce::Slider::Rotary);
    reverbRoomSize.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    reverbRoomSize.setColour(juce::Slider::backgroundColourId, charcoal);
    reverbRoomSize.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    reverbRoomSize.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    reverbRoomSize.setRange(0.0, 1.0, 0.01);
    reverbRoomSize.setValue(audioProcessor.reverbRoomSize->get());
    reverbRoomSize.onValueChange = [this] { audioProcessor.reverbRoomSize->setValueNotifyingHost((float)reverbRoomSize.getValue()); };
    reverbRoomSize.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    reverbRoomSizeLabel.setText("Room Size", juce::dontSendNotification);
    reverbRoomSizeLabel.setJustificationType(juce::Justification::centred);
    reverbRoomSizeLabel.attachToComponent(&reverbRoomSize, false);
    addAndMakeVisible(reverbRoomSize);

    reverbDamping.setSliderStyle(juce::Slider::Rotary);
    reverbDamping.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    reverbDamping.setColour(juce::Slider::backgroundColourId, charcoal);
    reverbDamping.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    reverbDamping.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    reverbDamping.setRange(0.0, 1.0, 0.01);
    reverbDamping.setValue(audioProcessor.reverbDamping->get());
    reverbDamping.onValueChange = [this] { audioProcessor.reverbDamping->setValueNotifyingHost((float)reverbDamping.getValue()); };
    reverbDamping.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    reverbDampingLabel.setText("Damping", juce::dontSendNotification);
    reverbDampingLabel.setJustificationType(juce::Justification::centred);
    reverbDampingLabel.attachToComponent(&reverbDamping, false);
    addAndMakeVisible(reverbDamping);

    reverbWet.setSliderStyle(juce::Slider::Rotary);
    reverbWet.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    reverbWet.setColour(juce::Slider::backgroundColourId, charcoal);
    reverbWet.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    reverbWet.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    reverbWet.setRange(0.0, 1.0, 0.01);
    reverbWet.setValue(audioProcessor.reverbWet->get());
    reverbWet.onValueChange = [this] { audioProcessor.reverbWet->setValueNotifyingHost((float)reverbWet.getValue()); };
    reverbWet.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    reverbWetLabel.setText("Wet Level", juce::dontSendNotification);
    reverbWetLabel.setJustificationType(juce::Justification::centred);
    reverbWetLabel.attachToComponent(&reverbWet, false);
    addAndMakeVisible(reverbWet);

    reverbDry.setSliderStyle(juce::Slider::Rotary);
    reverbDry.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    reverbDry.setColour(juce::Slider::backgroundColourId, charcoal);
    reverbDry.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    reverbDry.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    reverbDry.setRange(0.0, 1.0, 0.01);
    reverbDry.setValue(audioProcessor.reverbDry->get());
    reverbDry.onValueChange = [this] { audioProcessor.reverbDry->setValueNotifyingHost((float)reverbDry.getValue()); };
    reverbDry.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    reverbDryLabel.setText("Dry Level", juce::dontSendNotification);
    reverbDryLabel.setJustificationType(juce::Justification::centred);
    reverbDryLabel.attachToComponent(&reverbDry, false);
    addAndMakeVisible(reverbDry);

    reverbWidth.setSliderStyle(juce::Slider::Rotary);
    reverbWidth.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    reverbWidth.setColour(juce::Slider::backgroundColourId, charcoal);
    reverbWidth.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    reverbWidth.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    reverbWidth.setRange(0.0, 1.0, 0.01);
    reverbWidth.setValue(audioProcessor.reverbWidth->get());
    reverbWidth.onValueChange = [this] { audioProcessor.reverbWidth->setValueNotifyingHost((float)reverbWidth.getValue()); };
    reverbWidth.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    reverbWidthLabel.setText("Width", juce::dontSendNotification);
    reverbWidthLabel.setJustificationType(juce::Justification::centred);
    reverbWidthLabel.attachToComponent(&reverbWidth, false);
    addAndMakeVisible(reverbWidth);

    reverbEnabled.onClick = [this] { audioProcessor.reverbEnabled->setValueNotifyingHost(reverbEnabled.getToggleState()); };
    reverbEnabled.setButtonText("Enable");
    addAndMakeVisible(reverbEnabled);


    saveButton.setButtonText("Save");
    saveButton.addListener(this);
    addAndMakeVisible(&saveButton);

    loadButton.setButtonText("Load");
    loadButton.addListener(this);
    addAndMakeVisible(&loadButton);

    reverb.setText("Reverb");
    addAndMakeVisible(reverb);

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



//Parameters=================================================================

void ImagineAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{

}

void ImagineAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (thumbnail.getTotalLength() > 0)
    {
        if (topBounds.contains(event.getPosition()))
        {
            int numSamples = static_cast<int>(audioProcessor.mainbuffer->getNumSamples());
            float clickPosition = static_cast<float>(event.getPosition().x) / topBounds.getWidth();
            if (event.mods.isLeftButtonDown())
            {
                startPosition = static_cast<int>(clickPosition * numSamples);
            }
            else if (event.mods.isRightButtonDown())
            {
                endPosition = static_cast<int>(clickPosition * numSamples);
            }

            if (startPosition >= endPosition)
            {
                std::swap(startPosition, endPosition);
            }

            repaint();
            audioProcessor.onBlockChange(startPosition, endPosition);
        }
    }

}

void ImagineAudioProcessorEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    if (thumbnail.getTotalLength() > 0)
    {
        if (event.mods.isLeftButtonDown())
        {
            startPosition = 0;
            endPosition = audioProcessor.mainbuffer->getNumSamples();
        }
        else if (event.mods.isRightButtonDown())
        {
            startPosition = 0;
            endPosition = audioProcessor.mainbuffer->getNumSamples();
        }
        repaint();
        audioProcessor.onBlockChange(startPosition, endPosition);
    }
}

void ImagineAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &saveButton)
    {
        auto* chooser = new juce::FileChooser("Save Sound", juce::File(), "*.imag");
        chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc)
            {
                DBG("Save FileChooser lambda entered");
        auto result = fc.getResult();
        if (result != juce::File{})
        {
            auto extension = result.getFileExtension().toLowerCase();
            if (extension == ".imag")
            {
                DBG("Saving to file: " + result.getFullPathName());
                audioProcessor.saveSound(result);
            }
        }
        else
        {
            DBG("No file selected for saving");
        }
        delete chooser;
            });
    }
    else if (button == &loadButton)
    {
        auto* chooser = new juce::FileChooser("Load Sound", juce::File(), "*.imag");
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc)
            {
                DBG("Load FileChooser lambda entered");
        auto result = fc.getResult();
        if (result != juce::File{})
        {
            auto extension = result.getFileExtension().toLowerCase();
            if (extension == ".imag")
            {
                DBG("Loading from file: " + result.getFullPathName());
                audioProcessor.loadFileSound(result);
            }
        }
        else
        {
            DBG("No file selected for loading");
        }
        delete chooser;
            });
    }
}



void drawFilledBoundWithBorder(juce::Graphics& g, juce::Rectangle<int> bounds, juce::Colour fillColour, juce::Colour borderColour, int borderWidth)
{
    // Fill with the specified color
    g.setColour(fillColour);
    g.fillRect(bounds);

    // Draw the border
    g.setColour(borderColour);
    g.drawRect(bounds, borderWidth);
}

//==============================================================================
void ImagineAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Define colors for the sections
    juce::Colour topFillColour = topcolor;
    juce::Colour bottomFillColour = bottomcolor;
    juce::Colour topBorderColour = topbordercolor;
    juce::Colour bottomBorderColour = bottombordercolor;

    auto bounds = getLocalBounds();
    auto topBounds = bounds.removeFromTop(bounds.getHeight() * 0.4);
    auto bottomBounds = bounds;

    // Draw filled bounds with borders using the helper function
    drawFilledBoundWithBorder(g, topBounds, topFillColour, topBorderColour, 2);
    drawFilledBoundWithBorder(g, bottomBounds, bottomFillColour, bottomBorderColour, 2);

    // Draw the waveform in the top section if available
    if (thumbnail.getTotalLength() > 0.0)
    {
        g.setColour(juce::Colours::white);
        thumbnail.drawChannels(g, topBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
        int numSamples = static_cast<int>(audioProcessor.mainbuffer->getNumSamples());
        float start = (static_cast<float>(startPosition) / numSamples) * topBounds.getWidth();
        float end = (static_cast<float>(endPosition) / numSamples) * topBounds.getWidth();

   
        g.setColour(juce::Colours::red);  
        g.drawLine(topBounds.getX() + start, topBounds.getY(), topBounds.getX() + start, topBounds.getBottom());
        g.drawLine(topBounds.getX() + end, topBounds.getY(), topBounds.getX() + end, topBounds.getBottom());     
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.drawText(imgstate, topBounds, juce::Justification::centred);
    }
}


void  buttonsliderorganizer(int numberofsliders, int numberofbuttons,juce::TextButton buttonarray[], juce::Slider sliderarray[]) {




}

void ImagineAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //playButton.setBounds(10, 10, 100, 30);
    //stopButton.setBounds(120, 10, 100, 30);

    // Define larger width and height for each knob
    constexpr int knobWidth = 120;
    constexpr int knobHeight = 120;
    const int numKnobs = 4;

    // Calculate horizontal spacing between knobs (set to 0 for no space)
    int spacing = 0;

    // Set yPosition to the top of the bottom region
    int yPosition = getHeight() * 1.5 / 3; // Adjust this if necessary
    int rightcorner = getWidth()-50;
    //for (int i = 0; i < numKnobs; ++i)
    //{
    //    juce::Slider* knob = nullptr;
    //    switch (i) {
    //    case 0: knob = &paramslider1; break;
    //    case 1: knob = &paramslider2; break;
    //    case 2: knob = &paramslider3; break;
    //    case 3: knob = &paramslider4; break;
    //    }
    //    if (knob != nullptr)
    //    {
    //        // Adjust the size and position of each knob with no spacing
    //        knob->setBounds(spacing + i * (knobWidth + spacing), yPosition, knobWidth, knobHeight); 
    //        knob->setSliderStyle(juce::Slider::Rotary);
    //        knob->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    //    }
    //}

    //=====================================================================
    //GainSlider

    gainSlider.setBounds(rightcorner - 150, yPosition+150, 200,150);
    
    auto bounds = getLocalBounds();
    auto topBounds = bounds.removeFromTop(bounds.getHeight() * 0.4);
    auto bottomBounds = bounds;

    int rsliderWidth = 130;
    int rsliderHeight = 100;
    int rgroupX = bottomBounds.getCentreX() - 150;
    int rgroupY = bottomBounds.getY() + 5;
    int rsliderY = rgroupY + 30;
    int rmarginX = 80;
    reverb.setBounds(rgroupX - 5, rgroupY, 280, 420);
    reverbEnabled.setBounds(rgroupX + 25, rgroupY, 80, 80);
    reverbRoomSize.setBounds(rgroupX, rsliderY + 70, rsliderWidth, rsliderHeight);
    reverbDamping.setBounds((rgroupX + rmarginX), rsliderY + 70, rsliderWidth, rsliderHeight);
    reverbWet.setBounds((rgroupX + (rmarginX * 2)), rsliderY + 70, rsliderWidth, rsliderHeight);
    reverbDry.setBounds(rgroupX, rsliderY + 250, rsliderWidth, rsliderHeight);
    reverbWidth.setBounds(rgroupX + (rmarginX * 2), rsliderY + 250, rsliderWidth, rsliderHeight);


    int buttonHeight = 30;
    saveButton.setBounds(0, bottomBounds.getY()-35, 60, buttonHeight);
    loadButton.setBounds(70, bottomBounds.getY()-35, 60, buttonHeight);

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
    imgstate = "Path Loaded. Change parameters and Generate sound";
    thumbnail.clear();
    repaint();  // Repaint the editor to show the cleared thumbnail
    audioProcessor.mSampler.clearSounds();

//Default values
    windowComponent->getKernelSlider().setValue(25);
    windowComponent->getStepSlider().setValue(10);
    windowComponent->getSoundLevelSlider().setValue(1);
    windowComponent->getSoundDurationSlider().setValue(10);
    windowComponent->getModulationIntensitySlider().setValue(.8);
    windowComponent->getModulationEnvelopeIntensitySlider().setValue(.2);
    windowComponent->getModulationDurationSlider().setValue(6);
    windowComponent->getLfoScalarFreqSlider().setValue(.5);
    windowComponent->getLfoScalarAmplitudeSlider().setValue(1);
    windowComponent->getOvertoneNumScalarSlider().setValue(1);
    windowComponent->getLfoAmountScalarSlider().setValue(1);


    documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    mainFolder = createFolderIfNotExists(documentsDir, "Imagine");
    outputFolder = createFolderIfNotExists(mainFolder, "output");
    outputPath = outputFolder.getFullPathName().toStdString() + "\\";
    
    for (const auto& file : files)
    {
        juce::File imageFile(file);
        imagePath = imageFile.getFullPathName().toStdString();
    }
    slider_window->setVisible(true);

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

//void ImagineAudioProcessorEditor::playButtonClicked()
//{
//    changeState(Starting);
//}
//
//void ImagineAudioProcessorEditor::stopButtonClicked()
//{
//    changeState(Stopping);
//}

//void ImagineAudioProcessorEditor::playWavFile()
//{
//    juce::File wavFile = audioProcessor.outputpath;
//
//    if (wavFile.existsAsFile())
//    {
//        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(wavFile));
//
//        if (reader != nullptr)
//        {
//            std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader.release(), true));
//            transportSource.setSource(newSource.get(), 0, nullptr, 44800);
//            readerSource.reset(newSource.release());
//            transportSource.start();
//        }
//    }
//}

void ImagineAudioProcessorEditor::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case Starting:
            //playWavFile();
            transportSource.start();
            break;

        case Stopping:
            transportSource.stop();
            break;
        }
    }
}

void ImagineAudioProcessorEditor::generateSound()
{
    
    audioProcessor.mSampler.clearSounds();
    audioProcessor.callPythonFunction(imagePath,
        outputPath,
        (int) windowComponent-> getKernelSlider().getValue(),
        (int) windowComponent->getStepSlider().getValue(),
        windowComponent->getSoundLevelSlider().getValue(),
        windowComponent->getSoundDurationSlider().getValue(),
        windowComponent->getModulationDurationSlider().getValue(),
        windowComponent->getModulationIntensitySlider().getValue(),
        windowComponent->getModulationEnvelopeIntensitySlider().getValue(),
        windowComponent->getOvertoneNumScalarSlider().getValue(),
        windowComponent->getLfoScalarFreqSlider().getValue(),
        windowComponent->getLfoScalarAmplitudeSlider().getValue(),
        windowComponent->getLfoIntensitySlider().getValue(),
        windowComponent->getLfoAmountScalarSlider().getValue());
    
    //slider_window->setVisible(false);
    audioProcessor.loadSound(audioProcessor.outputpath);
    int numSamples = audioProcessor.mainbuffer->getNumSamples();
    int numChannels = audioProcessor.mainbuffer->getNumChannels();
   
    thumbnail.setSource(new juce::FileInputSource(audioProcessor.outputpath));

    repaint();
    if (audioProcessor.selectedBlock != nullptr)
    {
        audioProcessor.selectedBlock->clear();
    }
    
}
