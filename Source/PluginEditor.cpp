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
    
    juce::Timer::startTimer(30);  // 30 ms update interval for ~30 FPS
    setSize (1280, 720);
    state = (Stopping);
    setResizable(false, false);

    viewToggle.setButtonText("Live View");
    viewToggle.setToggleState(false, juce::dontSendNotification); // Start in thumbnail view
    viewToggle.onClick = [this]() { repaint(); }; // Repaint on toggle
    addAndMakeVisible(viewToggle);

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
  
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    gainSlider.setColour(juce::Slider::backgroundColourId, charcoal);
    gainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    gainSlider.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainSlider.setSliderStyle(juce::Slider::Rotary);


    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,100,40);


    gainLabel.attachToComponent(&gainSlider, false);

    addAndMakeVisible(gainLabel);


    //Filter section=====================================================

    addAndMakeVisible(Filterlbl);
    addAndMakeVisible(filter1);
    addAndMakeVisible(filter2);
    addAndMakeVisible(filter3);
    addAndMakeVisible(filter4);
    addAndMakeVisible(curfiltertype_combobox);
    addAndMakeVisible(curfiltertoggle);
    addAndMakeVisible(curfilterfreq);
    addAndMakeVisible(cur_q_val);
    addAndMakeVisible(is_enabledlabel);
    addAndMakeVisible(freqfilterlbl);
    addAndMakeVisible(qfilterlbl);
    addAndMakeVisible(audioProcessor.waveviewer);


    initializeSlider(attackSlider, attacklabel, "Attack", 0.1f, 5.0f, 0.0f, 0.01f);
    initializeSlider(decaySlider, decaylabel, "Decay", 0.1f, 5.0f, 0.0f, 0.01f);
    initializeSlider(sustainSlider, sustainlabel, "Sustain", 0.0f, 1.0f, 1.0f, 0.01f);
    initializeSlider(releaseSlider, releaselabel, "Release", 0.1f, 5.0f, 0.0f, 0.01f);



    audioProcessor.waveviewer.setColours(gunmetal, juce::Colours::whitesmoke.withAlpha(0.5f));


    filter1.onClick = [this]() { onFilterToggled(&filter1); };
    filter2.onClick = [this]() { onFilterToggled(&filter2); };
    filter3.onClick = [this]() { onFilterToggled(&filter3); };
    filter4.onClick = [this]() { onFilterToggled(&filter4); };

    filter1.setToggleState(true,juce::dontSendNotification);



    curfiltertype_combobox.setColour(juce::ComboBox::backgroundColourId, charcoal);
    curfiltertype_combobox.addItem("LowPass", 1);
    curfiltertype_combobox.addItem("HighPass", 2);
    curfiltertype_combobox.addItem("BandPass", 3);
    curfiltertype_combobox.addItem("Notch", 4);
    curfiltertype_combobox.setText("LowPass");

    curfilterfreq.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    curfilterfreq.setColour(juce::Slider::backgroundColourId, charcoal);
    curfilterfreq.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    curfilterfreq.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    curfilterfreq.setTextValueSuffix("hz");
    freqfilterlbl.setText("Frequency", juce::dontSendNotification);
    curfilterfreq.setSliderStyle(juce::Slider::Rotary);
    curfilterfreq.setRange(20, 5000, 5);
    curfilterfreq.addListener(this);

    cur_q_val.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    cur_q_val.setColour(juce::Slider::backgroundColourId, charcoal);
    cur_q_val.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    cur_q_val.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    cur_q_val.setTextValueSuffix("");
    qfilterlbl.setText("Q", juce::dontSendNotification);
    cur_q_val.setSliderStyle(juce::Slider::Rotary);
    cur_q_val.setRange(.1, 5, .1);
    cur_q_val.addListener(this);


    



    is_enabledlabel.setText("Enable filter",juce::dontSendNotification);
    is_enabledlabel.setColour(juce::Label::backgroundColourId, charcoal);
    is_enabledlabel.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);


    Filterlbl.setColour(juce::Slider::backgroundColourId, charcoal);
    Filterlbl.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    Filterlbl.setText("Filter section", juce::dontSendNotification);
    auto setFilterColours = [](juce::TextButton& filter, juce::Colour background, juce::Colour textBoxText, juce::Colour textBoxBackground) {
        
        filter.setColour(juce::Slider::backgroundColourId, background);
        filter.setColour(juce::Slider::textBoxTextColourId, textBoxText);
        filter.setColour(juce::Slider::textBoxBackgroundColourId, textBoxBackground);
        };

    curfiltertoggle.onClick = [this]() {
        updatefilters(); };
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
    stopTimer();
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
}


void ImagineAudioProcessorEditor::initializeSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
    float minValue, float maxValue, float defaultValue, float interval)
{
    // Slider style settings
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    slider.setColour(juce::Slider::backgroundColourId, charcoal);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setRange(minValue, maxValue, interval);
    slider.setValue(defaultValue);
    slider.addListener(this);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(slider);

    // Label settings
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);
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


void ImagineAudioProcessorEditor::updatefilters() {

    auto* freq = &tempfreq;
    auto* q = &tempq;
    //Filtertype
    int curfilter_id = curfiltertype_combobox.getSelectedId();
    //On which filter
    int result = findtoggledfilter();
    if (result == 0) {
        curfiltertoggle.setToggleState(false, juce::dontSendNotification);

    }
    else
    {
        switch (result)
        {
        default:
            break;

        case 1:
            if (curfiltertoggle.getToggleState() == true) {
                istoggled_fil1 = true;
                audioProcessor.filter1_enabled = true;
                freq = &filter1freq;
                q = &filter1q;

            }
            else {
                istoggled_fil1 = false;
                audioProcessor.filter1_enabled = false;
                return;
            }
            break;

        case 2:
            if (curfiltertoggle.getToggleState() == true) {
                istoggled_fil2 = true;
                audioProcessor.filter2_enabled = true;
                freq = &filter2freq;
                q = &filter2q;
            }

            else {
                istoggled_fil2 = false;
                audioProcessor.filter2_enabled = false;
                return;
            }
            break;

        case 3:
            if (curfiltertoggle.getToggleState() == true) {
                istoggled_fil3 = true;
                audioProcessor.filter3_enabled = true;
                freq = &filter3freq;
                q = &filter3q;
            }
            else {
                istoggled_fil3 = false;
                audioProcessor.filter3_enabled = false;
                return;
            }
            break;

        case 4:
            if (curfiltertoggle.getToggleState() == true) {
                istoggled_fil4 = true;
                audioProcessor.filter4_enabled = true;
                freq = &filter4freq;
                q = &filter4q;
            }
            else {
                istoggled_fil4 = false;
                audioProcessor.filter4_enabled = false;
                return;
            }
            break;

        }

        //Setup filter type
        switch (curfilter_id)
        {
        default:
            break;
        case 1:
            audioProcessor.setFilter(result, "LowPass", *freq, *q);
            break;
        case 2:
            audioProcessor.setFilter(result, "HighPass", *freq, *q);
            break;
        case 3:
            audioProcessor.setFilter(result, "BandPass", *freq, *q);
            break;
        case 4:
            audioProcessor.setFilter(result, "Notch", *freq, *q);
            break;
        }


    }

    return;
}

//Parameters=================================================================

void ImagineAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &gainSlider) {
        audioProcessor.setGain(gainSlider.getValue());
    }

    if (slider == &curfilterfreq) {
       int result = findfilternum();
       switch (result)
       {
       default:
           break;

       case 1:
           filter1freq = curfilterfreq.getValue();
           break;

       case 2:
           filter2freq = curfilterfreq.getValue();
           break;
       case 3:
           filter3freq = curfilterfreq.getValue();
           break;
       case 4:
           filter4freq = curfilterfreq.getValue();
           break;

       }
       updatefilters();
    }
    
    if (slider == &cur_q_val) {
        int result = findfilternum();
        switch (result)
        {
        default:
            break;

        case 1:
            filter1q = cur_q_val.getValue();
            updatefilters();
            break;
        case 2:
            filter2q = cur_q_val.getValue();
            updatefilters();
            break;
        case 3:
            filter3q = cur_q_val.getValue();
            updatefilters();
            break;
        case 4:
            filter1q = cur_q_val.getValue();
            updatefilters();
            break;

        
        }

    }

    if (slider == &attackSlider || slider == &decaySlider || slider == &sustainSlider || slider == &releaseSlider) {

        audioProcessor.updateADSRParameters(attackSlider.getValue(), decaySlider.getValue(), sustainSlider.getValue(), releaseSlider.getValue());

    }
}

void ImagineAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (thumbnail.getTotalLength() > 0)
    {
        auto numSamples = static_cast<int>(audioProcessor.mainbuffer->getNumSamples());
        float clickPosition = static_cast<float>(event.getPosition().x) / getWidth();

        if (event.mods.isLeftButtonDown())
        {
            startPosition = juce::jlimit(0, numSamples - 1, static_cast<int>(clickPosition * numSamples));
        }
        else if (event.mods.isRightButtonDown())
        {
            endPosition = juce::jlimit(0, numSamples - 1, static_cast<int>(clickPosition * numSamples));
        }

        if (startPosition >= endPosition)
        {
            std::swap(startPosition, endPosition);
        }

        repaint();
        audioProcessor.onBlockChange(startPosition, endPosition);
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
    if (viewToggle.getToggleStateValue() == true) {
        // Adjust waveviewer bounds to leave space for viewToggle
        audioProcessor.waveviewer.setVisible(true);
        juce::Rectangle<int> adjustedWaveviewerBounds = topBounds.reduced(0, viewToggle.getHeight() + 10);
        audioProcessor.waveviewer.setBounds(adjustedWaveviewerBounds);
    }
    else {
        audioProcessor.waveviewer.setVisible(false);
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

    // Set viewToggle button position
    viewToggle.setBounds(topBounds.getX() + 145, topBounds.getBottom() - viewToggle.getHeight()+5, 100, 50);

    // Additional UI setup...
    int lowertop = bounds.getHeight() * 0.6;
    int filterbuttonpos = lowertop + 30;
    int filterbuttonmargin = 10;
    int filterlblpos = filterbuttonpos + 10;
    int enabledposy = filterbuttonpos + 80;
    int filterposy = filterbuttonpos + 140;
    int freq_q_lblpos = filterbuttonpos + 140;
    int filtercomboposy = filterbuttonpos + 240;

    int envelopesectiony = filtercomboposy + 75;


    Filterlbl.setBounds(50, filterlblpos, 100, 30);
    filter1.setBounds(50, filterbuttonpos + 50, 50, 30);

    filter2.setBounds(100 + filterbuttonmargin, filterbuttonpos + 50, 50, 30);
    filter3.setBounds(150 + filterbuttonmargin * 2, filterbuttonpos + 50, 50, 30);
    filter4.setBounds(200 + filterbuttonmargin * 3, filterbuttonpos + 50, 50, 30);

    is_enabledlabel.setBounds(80, enabledposy, 100, 50);
    curfiltertoggle.setBounds(50, enabledposy, 50, 50);

    freqfilterlbl.setBounds(50, freq_q_lblpos, 80, 30);
    qfilterlbl.setBounds(200, freq_q_lblpos, 50, 30);

    curfilterfreq.setBounds(50, filterposy, 150, 100);
    cur_q_val.setBounds(200, filterposy, 150, 100);

    curfiltertype_combobox.setBounds(50, filtercomboposy, 250, 30);
    curfiltertype_combobox.onChange = [this]() {
        updatefilters();
        };

    attackSlider.setBounds(0, envelopesectiony, 140, 90);
    decaySlider.setBounds(100, envelopesectiony, 140, 90);
    sustainSlider.setBounds(200, envelopesectiony , 140, 90);
    releaseSlider.setBounds(300 , envelopesectiony , 140, 90);
}


void ImagineAudioProcessorEditor::resized()
{
    constexpr int knobWidth = 120;
    constexpr int knobHeight = 120;
    const int numKnobs = 4;

    // Calculate horizontal spacing between knobs (set to 0 for no space)
    int spacing = 0;
    
    // Set yPosition to the top of the bottom region
    int yPosition = getHeight() * 1.5 / 3; // Adjust this if necessary
    int rightcorner = getWidth()-50;


    //=====================================================================
    //GainSlider
    viewToggle.setBounds(50, 10, 100, 30);
    gainSlider.setBounds(rightcorner - 150, yPosition+150, 200,150);
    
    auto bounds = getLocalBounds();
    auto topBounds = bounds.removeFromTop(bounds.getHeight() * 0.4);
    auto bottomBounds = bounds;

    int rsliderWidth = 130;
    int rsliderHeight = 100;
    int rgroupX = bottomBounds.getCentreX() - 220;
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

void ImagineAudioProcessorEditor::generateSound()
{
    // Clear the existing thumbnail and sounds
    thumbnail.clear();
    repaint();
    audioProcessor.mSampler.clearSounds();

    // Generate sound by calling the Python function
    audioProcessor.callPythonFunction(imagePath,
        outputPath,
        (int)windowComponent->getKernelSlider().getValue(),
        (int)windowComponent->getStepSlider().getValue(),
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


    // Load the generated sound
    audioProcessor.loadSound(audioProcessor.outputpath);

    // Verify that the output file exists and set it as the source
    DBG("Path is " + audioProcessor.outputpath.getFileName());
    juce::File generatedFile(audioProcessor.outputpath);
    if (generatedFile.existsAsFile())
    {
        thumbnail.setSource(new juce::FileInputSource(generatedFile));
        repaint();  // Repaint to show the updated thumbnail
    }
    else
    {
        DBG("Generated file does not exist: ");
    }

    // Clear selected block if it exists
    if (audioProcessor.selectedBlock != nullptr)
    {
        audioProcessor.selectedBlock->clear();
    }
}

int ImagineAudioProcessorEditor::findtoggledfilter() {

    if (filter1.getToggleState() == true) {

        istoggled_fil1 = true;
        return 1;
    }
    if (filter2.getToggleState() == true) {

        istoggled_fil2 = true;
        return 2;
    }
    if (filter3.getToggleState() == true) {

        istoggled_fil3 = true;
        return 3;
    }
    if (filter4.getToggleState() == true) {

        istoggled_fil4 = true;
        return 4;
    }
    else {
        return 0;
    }
}


void ImagineAudioProcessorEditor::untoggleOtherFilters(int selectedFilter)
{
    switch (selectedFilter)
    {
    case 1:
        filter2.setToggleState(false, juce::dontSendNotification);
        filter3.setToggleState(false, juce::dontSendNotification);
        filter4.setToggleState(false, juce::dontSendNotification);
        break;
    case 2:
        filter1.setToggleState(false, juce::dontSendNotification);
        filter3.setToggleState(false, juce::dontSendNotification);
        filter4.setToggleState(false, juce::dontSendNotification);
        break;
    case 3:
        filter1.setToggleState(false, juce::dontSendNotification);
        filter2.setToggleState(false, juce::dontSendNotification);
        filter4.setToggleState(false, juce::dontSendNotification);
        break;
    case 4:
        filter1.setToggleState(false, juce::dontSendNotification);
        filter2.setToggleState(false, juce::dontSendNotification);
        filter3.setToggleState(false, juce::dontSendNotification);
        break;
    default:
        // If no filter is selected, do nothing
        break;
    }
}

int ImagineAudioProcessorEditor::findfilternum()
{
    if (filter1.getToggleState()) {
        
        return 1;

    }
    if (filter2.getToggleState()) {

        return 2;

    }

    if (filter3.getToggleState()) {

        return 3;

    }

    if (filter4.getToggleState()) {

        return 4;

    }


    return 0; // Return 0 if none are toggled
}



void ImagineAudioProcessorEditor::onFilterToggled(juce::Button* toggledButton)
{

    if (toggledButton == &filter1)
    {
        untoggleOtherFilters(1);
        if (istoggled_fil1) {
            curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        }
        else {
            curfiltertoggle.setToggleState(false, juce::dontSendNotification);
        }
        
    }
    else if (toggledButton == &filter2)
    {
        untoggleOtherFilters(2);
        if (istoggled_fil2) {
            curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        }
        else {
            curfiltertoggle.setToggleState(false, juce::dontSendNotification);
        }

    }
    else if (toggledButton == &filter3)
    {
        untoggleOtherFilters(3);
        if (istoggled_fil3) {
            curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        }
        else {
            curfiltertoggle.setToggleState(false, juce::dontSendNotification);
        }

    }
    else if (toggledButton == &filter4)
    {
        untoggleOtherFilters(4);
        if (istoggled_fil4) {
            curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        }
        else {
            curfiltertoggle.setToggleState(false, juce::dontSendNotification);
        }

    }
}

void ImagineAudioProcessorEditor::loadThumbnailAsync(const juce::File& file)
{
    juce::FileInputSource* fileSource = new juce::FileInputSource(file);
    juce::Thread::launch([this, fileSource]()
        {
            thumbnail.setSource(fileSource); // This might take some time
            juce::MessageManager::callAsync([this]() { repaint(); }); // Repaint once loaded
        });
}

void ImagineAudioProcessorEditor::drawLiveBuffer(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Access the audio buffer from the processor
    auto* buffer = audioProcessor.getLiveBuffer();
    if (buffer == nullptr || buffer->getNumSamples() == 0) return;

    g.setColour(juce::Colours::whitesmoke.withAlpha(0.5f));
    int numChannels = buffer->getNumChannels();
    int numSamples = buffer->getNumSamples();
    float verticalScale = bounds.getHeight() / (float)numChannels;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto channelData = buffer->getReadPointer(channel);
        juce::Path waveformPath;
        waveformPath.startNewSubPath(bounds.getX(), bounds.getCentreY());

        // Map buffer samples to screen coordinates
        for (int i = 0; i < numSamples; ++i)
        {
            float x = juce::jmap(i, 0, numSamples - 1, bounds.getX(), bounds.getRight());
            float y = juce::jmap(static_cast<float>(channelData[i]), -1.0f, 1.0f, static_cast<float>(bounds.getBottom()), static_cast<float>(bounds.getY()));
            waveformPath.lineTo(x, y);
        }

        // Draw the waveform path for each channel
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f));
    }
}

// Timer callback for continuous repaint
void ImagineAudioProcessorEditor::timerCallback()
{
    if (viewToggle.getToggleState()) // Only update if live view is enabled
    {
        repaint();
    }
}