/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "HelpWindow.h"


//==============================================================================
ImagineAudioProcessorEditor::ImagineAudioProcessorEditor(ImagineAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), thumbnailCache(1),  // Initialize the thumbnail cache with a cache size of 5
    thumbnail(1024, formatManager, thumbnailCache)
{

    juce::Timer::startTimer(30);  // 30 ms update interval for ~30 FPS
    setSize(1280, 720);
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



    gainSlider.setRange(0.0f, 10.0f, 0.05f);  // Set range from 0 (mute) to 1 (full gain)
    gainSlider.setValue(audioProcessor.gainS->get());
    gainSlider.onValueChange = [this] { audioProcessor.gainS->setValueNotifyingHost((float)gainSlider.getValue() / 10.0f); };
    gainSlider.addListener(this);         // Make this editor a listener
    addAndMakeVisible(gainSlider);



    // Gain label setup

    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    gainSlider.setColour(juce::Slider::backgroundColourId, charcoal);
    gainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    gainSlider.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainSlider.setSliderStyle(juce::Slider::Rotary);

    DBG("Constructor Gain Slider is " + std::to_string(gainSlider.getValue()));
    DBG("Constructor Gain in Processor is " + std::to_string(audioProcessor.gainS->get()));

    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);





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
    helpbutton.addListener(this);
    addAndMakeVisible(helpbutton);


    helpbutton.setButtonText("Help");


    initializeSlider(attackSlider, attacklabel, "Attack", 0.1f, 5.0f, 0.0f, 0.01f);
    initializeSlider(decaySlider, decaylabel, "Decay", 0.1f, 5.0f, 0.0f, 0.01f);
    initializeSlider(sustainSlider, sustainlabel, "Sustain", 0.0f, 1.0f, 1.0f, 0.01f);
    initializeSlider(releaseSlider, releaselabel, "Release", 0.1f, 5.0f, 2.0f, 0.01f);

    attackSlider.setValue(audioProcessor.attack->get());
    attackSlider.onValueChange = [this] { audioProcessor.attack->setValueNotifyingHost((float)attackSlider.getValue() / 5.0f); };

    decaySlider.setValue(audioProcessor.decay->get());
    decaySlider.onValueChange = [this] { audioProcessor.decay->setValueNotifyingHost((float)decaySlider.getValue() / 5.0f); };

    sustainSlider.setValue(audioProcessor.sustain->get());
    sustainSlider.onValueChange = [this] { audioProcessor.sustain->setValueNotifyingHost((float)sustainSlider.getValue()); };

    releaseSlider.setValue(audioProcessor.release->get());
    releaseSlider.onValueChange = [this] { audioProcessor.release->setValueNotifyingHost((float)releaseSlider.getValue() / 5.0f); };



    audioProcessor.waveviewer.setColours(gunmetal, juce::Colours::whitesmoke.withAlpha(0.5f));


    filter1.onClick = [this]() { audioProcessor.filterOne->setValueNotifyingHost(filter1.getToggleState()); onFilterToggled(&filter1); if (audioProcessor.isfilterOne->get())
    {
        curfilterfreq.setValue(audioProcessor.filter1Freq->get());
        cur_q_val.setValue(audioProcessor.filter1Q->get());
        curfiltertype_combobox.setSelectedId(audioProcessor.filter1Type->get());
    }};
    filter1.setToggleState(audioProcessor.filterOne->get(), juce::dontSendNotification);
    filter2.onClick = [this]() { audioProcessor.filterTwo->setValueNotifyingHost(filter2.getToggleState()); onFilterToggled(&filter2); if (audioProcessor.isfilterTwo->get())
    {
        curfilterfreq.setValue(audioProcessor.filter2Freq->get());
        cur_q_val.setValue(audioProcessor.filter2Q->get());
        curfiltertype_combobox.setSelectedId(audioProcessor.filter2Type->get());
    }};
    filter2.setToggleState(audioProcessor.filterTwo->get(), juce::dontSendNotification);
    filter3.onClick = [this]() { audioProcessor.filterThree->setValueNotifyingHost(filter3.getToggleState()); onFilterToggled(&filter3); if (audioProcessor.isfilterThree->get())
    {
        curfilterfreq.setValue(audioProcessor.filter3Freq->get());
        cur_q_val.setValue(audioProcessor.filter3Q->get());
        curfiltertype_combobox.setSelectedId(audioProcessor.filter3Type->get());
    } };
    filter3.setToggleState(audioProcessor.filterThree->get(), juce::dontSendNotification);
    filter4.onClick = [this]() { audioProcessor.filterFour->setValueNotifyingHost(filter4.getToggleState()); onFilterToggled(&filter4); if (audioProcessor.isfilterFour->get())
    {
        curfilterfreq.setValue(audioProcessor.filter4Freq->get());
        cur_q_val.setValue(audioProcessor.filter4Q->get());
        curfiltertype_combobox.setSelectedId(audioProcessor.filter4Type->get());
    } };
    filter4.setToggleState(audioProcessor.filterFour->get(), juce::dontSendNotification);

    //filter1.setToggleState(true,juce::dontSendNotification);


    curfiltertype_combobox.setColour(juce::ComboBox::backgroundColourId, charcoal);
    curfiltertype_combobox.addItem("LowPass", 1);
    curfiltertype_combobox.addItem("HighPass", 2);
    curfiltertype_combobox.addItem("BandPass", 3);
    curfiltertype_combobox.addItem("Notch", 4);
    curfiltertype_combobox.setText(audioProcessor.filterType->getCurrentValueAsText());
    curfiltertype_combobox.addListener(this);


    curfilterfreq.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    curfilterfreq.setColour(juce::Slider::backgroundColourId, charcoal);
    curfilterfreq.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    curfilterfreq.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    curfilterfreq.setTextValueSuffix("hz");
    freqfilterlbl.setText("Frequency", juce::dontSendNotification);
    curfilterfreq.setSliderStyle(juce::Slider::Rotary);
    curfilterfreq.setRange(20, 5000, 5);
    curfilterfreq.setValue(audioProcessor.filterFreq->get());
    curfilterfreq.onValueChange = [this] { audioProcessor.filterFreq->setValueNotifyingHost(curfilterfreq.getValue() / 5000.0f); };
    curfilterfreq.addListener(this);

    cur_q_val.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    cur_q_val.setColour(juce::Slider::backgroundColourId, charcoal);
    cur_q_val.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    cur_q_val.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    cur_q_val.setTextValueSuffix("");
    qfilterlbl.setText("Q", juce::dontSendNotification);
    cur_q_val.setSliderStyle(juce::Slider::Rotary);
    cur_q_val.setRange(.1, 5, .1);
    cur_q_val.setValue(audioProcessor.filterQ->get());
    cur_q_val.onValueChange = [this] { audioProcessor.filterQ->setValueNotifyingHost((float)cur_q_val.getValue() / 5.0f); };
    cur_q_val.addListener(this);






    is_enabledlabel.setText("Enable filter", juce::dontSendNotification);
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

    delayTime.setSliderStyle(juce::Slider::Rotary);
    delayTime.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    delayTime.setColour(juce::Slider::backgroundColourId, charcoal);
    delayTime.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    delayTime.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    delayTime.setRange(0.0f, 2.0, 0.01);
    delayTime.setValue(audioProcessor.delayTime->get());
    delayTime.onValueChange = [this] { audioProcessor.delayTime->setValueNotifyingHost((float)delayTime.getValue() / 2.0f); };
    delayTime.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    delayTimeLabel.setText("Time", juce::dontSendNotification);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    delayTimeLabel.attachToComponent(&delayTime, false);
    addAndMakeVisible(delayTime);

    delayFeedback.setSliderStyle(juce::Slider::Rotary);
    delayFeedback.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    delayFeedback.setColour(juce::Slider::backgroundColourId, charcoal);
    delayFeedback.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    delayFeedback.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    delayFeedback.setRange(0.0f, 2.0, 0.01);
    delayFeedback.setValue(audioProcessor.feedback->get());
    delayFeedback.onValueChange = [this] { audioProcessor.feedback->setValueNotifyingHost((float)delayFeedback.getValue() / 2.0f); };
    delayFeedback.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    delayFeedbackLabel.setText("Feedback", juce::dontSendNotification);
    delayFeedbackLabel.setJustificationType(juce::Justification::centred);
    delayFeedbackLabel.attachToComponent(&delayFeedback, false);
    addAndMakeVisible(delayFeedback);

    delayMix.setSliderStyle(juce::Slider::Rotary);
    delayMix.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
    delayMix.setColour(juce::Slider::backgroundColourId, charcoal);
    delayMix.setColour(juce::Slider::textBoxTextColourId, juce::Colours::whitesmoke);
    delayMix.setColour(juce::Slider::textBoxBackgroundColourId, charcoal);
    delayMix.setRange(0.0f, 1.0f, 0.01);
    delayMix.setValue(audioProcessor.feedback->get());
    delayMix.onValueChange = [this] { audioProcessor.mix->setValueNotifyingHost((float)delayMix.getValue()); };
    delayMix.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 30);
    delayMixLabel.setText("Mix", juce::dontSendNotification);
    delayMixLabel.setJustificationType(juce::Justification::centred);
    delayMixLabel.attachToComponent(&delayMix, false);
    addAndMakeVisible(delayMix);

    reverbEnabled.setToggleState(audioProcessor.reverbEnabled->get(), juce::dontSendNotification);
    reverbEnabled.onClick = [this] { audioProcessor.reverbEnabled->setValueNotifyingHost(reverbEnabled.getToggleState()); };
    reverbEnabled.setButtonText("Enable");
    addAndMakeVisible(reverbEnabled);

    delayEnabled.setToggleState(audioProcessor.delayEnabled->get(), juce::dontSendNotification);
    delayEnabled.onClick = [this] { audioProcessor.delayEnabled->setValueNotifyingHost(delayEnabled.getToggleState());  };
    delayEnabled.setButtonText("Enable");
    addAndMakeVisible(delayEnabled);

    saveButton.setButtonText("Save");
    saveButton.addListener(this);
    addAndMakeVisible(&saveButton);

    loadButton.setButtonText("Load");
    loadButton.addListener(this);
    addAndMakeVisible(&loadButton);

    toggleWindow.setButtonText("Toggle Parameter Window");
    toggleWindow.addListener(this);
    addAndMakeVisible(&toggleWindow);

    refreshPreset.setButtonText("Re-scan");
    refreshPreset.addListener(this);
    addAndMakeVisible(&refreshPreset);

    reverb.setText("Reverb");
    addAndMakeVisible(reverb);

    delay.setText("Delay");
    addAndMakeVisible(delay);

    presets.setText("Presets");
    addAndMakeVisible(presets);

    filters.setText("Filters");
    addAndMakeVisible(filters);


    windowComponent->getKernelSlider().onValueChange = [this] {audioProcessor.kernel->setValueNotifyingHost(windowComponent->getKernelSlider().getValue() / 50.0f); };
    windowComponent->getStepSlider().onValueChange = [this] {audioProcessor.stepSize->setValueNotifyingHost(windowComponent->getStepSlider().getValue() / 50.0f); };
    windowComponent->getSoundLevelSlider().onValueChange = [this] {audioProcessor.level->setValueNotifyingHost(windowComponent->getSoundLevelSlider().getValue() / 10.0f); };
    windowComponent->getSoundDurationSlider().onValueChange = [this] {audioProcessor.duration->setValueNotifyingHost(windowComponent->getSoundDurationSlider().getValue() / 30.0f); };
    windowComponent->getModulationIntensitySlider().onValueChange = [this] {audioProcessor.modulationIntensity->setValueNotifyingHost(windowComponent->getModulationIntensitySlider().getValue()); };
    windowComponent->getModulationEnvelopeIntensitySlider().onValueChange = [this] {audioProcessor.modulationEnvelopeIntensity->setValueNotifyingHost(windowComponent->getModulationEnvelopeIntensitySlider().getValue()); };
    windowComponent->getModulationDurationSlider().onValueChange = [this] {audioProcessor.modulationDuration->setValueNotifyingHost((windowComponent->getModulationDurationSlider().getValue() + 20.0f) / 40.0f); };
    windowComponent->getLfoScalarFreqSlider().onValueChange = [this] {audioProcessor.lfoScalarFreq->setValueNotifyingHost(windowComponent->getLfoScalarFreqSlider().getValue() / 50.0f); };
    windowComponent->getLfoScalarAmplitudeSlider().onValueChange = [this] {audioProcessor.lfoScalarAmplitude->setValueNotifyingHost(windowComponent->getLfoScalarAmplitudeSlider().getValue()); };
    windowComponent->getLfoIntensitySlider().onValueChange = [this] {audioProcessor.lfoIntensity->setValueNotifyingHost(windowComponent->getLfoIntensitySlider().getValue()); };
    windowComponent->getOvertoneNumScalarSlider().onValueChange = [this] {audioProcessor.overtoneNumScalar->setValueNotifyingHost(windowComponent->getOvertoneNumScalarSlider().getValue() / 4.0f); };
    windowComponent->getLfoAmountScalarSlider().onValueChange = [this] {audioProcessor.lfoAmountScalar->setValueNotifyingHost(windowComponent->getLfoAmountScalarSlider().getValue() / 4.0f); };

    juce::File generatedFile(audioProcessor.outputpath);
    if (generatedFile.existsAsFile())
    {
        thumbnail.setSource(new juce::FileInputSource(generatedFile));
        repaint();
    }
    else
    {
        DBG("Generated file does not exist: ");
    }

    if (!presetdir.exists()) {
        presetdir.createDirectory();
    }

    presetscontentlst.setDirectory(presetdir, true, true);
    presetscontentlst.setIgnoresHiddenFiles(true);
    presetsthread.startThread();
    addAndMakeVisible(presetlistbox);
    presetlistbox.setColour(juce::ListBox::backgroundColourId, charcoal);
    presetlistbox.addMouseListener(this, true);
    presetlistbox.addKeyListener(this);

    if (audioProcessor.isfilterOne->get() || audioProcessor.isfilterTwo->get() || audioProcessor.isfilterThree->get() || audioProcessor.isfilterFour->get())
    {
        restoreFilterState();
    }

}

ImagineAudioProcessorEditor::~ImagineAudioProcessorEditor()
{
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
    helpbutton.removeListener(this);
    presetsthread.stopThread(1000);
}

void ImagineAudioProcessorEditor::restoreFilterState()
{
    std::string type;
    if (audioProcessor.isfilterOne->get())
    {
        curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        type = getFilterType(audioProcessor.filter1Type->get());
        audioProcessor.setFilter(1, type, audioProcessor.filter1Freq->get(), audioProcessor.filter1Q->get());
    }
    if (audioProcessor.isfilterTwo->get())
    {
        curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        type = getFilterType(audioProcessor.filter2Type->get());
        audioProcessor.setFilter(2, type, audioProcessor.filter2Freq->get(), audioProcessor.filter2Q->get());
    }
    if (audioProcessor.isfilterThree->get())
    {
        curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        type = getFilterType(audioProcessor.filter3Type->get());
        audioProcessor.setFilter(3, type, audioProcessor.filter3Freq->get(), audioProcessor.filter3Q->get());
    }
    if (audioProcessor.isfilterFour->get())
    {
        curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        type = getFilterType(audioProcessor.filter4Type->get());
        audioProcessor.setFilter(4, type, audioProcessor.filter4Freq->get(), audioProcessor.filter4Q->get());
    }
}

std::string ImagineAudioProcessorEditor::getFilterType(int type)
{
    switch (type)
    {
    default:
        break;
    case 1:
        return "LowPass";
        break;
    case 2:
        return "HighPass";
        break;
    case 3:
        return "BandPass";
        break;
    case 4:
        return "Notch";
        break;

    }
}

bool ImagineAudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) {
    if (key == juce::KeyPress::returnKey) {

        juce::File selectedFile = presetlistbox.getSelectedFile();
        if (selectedFile.exists()) {
            thumbnail.clear(); // Clear the thumbnail first
            juce::File loadFile = audioProcessor.loadFileSound(selectedFile);
            if (loadFile.existsAsFile())
            {
                startPosition = 0;
                endPosition = audioProcessor.mainbuffer->getNumSamples();
                thumbnailCache.clear();
                thumbnail.clear();
                thumbnail.setSource(new juce::FileInputSource(loadFile));
                juce::Timer::callAfterDelay(500, [this]() { repaint(); });
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
        presetsthread.startThread();
        repaint();
        return 1;
    }
    return 0;


}

void ImagineAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    if (comboBox == &curfiltertype_combobox)
    {
        audioProcessor.filterType->setValueNotifyingHost((curfiltertype_combobox.getSelectedId() - 1) / 3.0f);
    }
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
                audioProcessor.isfilterOne->setValueNotifyingHost(true);
                audioProcessor.filter1_enabled = true;
                freq = &filter1freq;
                q = &filter1q;
                audioProcessor.filter1Freq->setValueNotifyingHost(filter1freq / 5000.0f);
                audioProcessor.filter1Q->setValueNotifyingHost(filter1q / 5.0f);
                audioProcessor.filter1Type->setValueNotifyingHost(curfilter_id / 4.0f);

            }
            else {
                istoggled_fil1 = false;
                audioProcessor.isfilterOne->setValueNotifyingHost(false);
                audioProcessor.filter1_enabled = false;
                return;
            }
            break;

        case 2:
            if (curfiltertoggle.getToggleState() == true) {
                istoggled_fil2 = true;
                audioProcessor.isfilterTwo->setValueNotifyingHost(true);
                audioProcessor.filter2_enabled = true;
                freq = &filter2freq;
                q = &filter2q;
                audioProcessor.filter2Freq->setValueNotifyingHost(filter2freq / 5000.0f);
                audioProcessor.filter2Q->setValueNotifyingHost(filter2q / 5.0f);
                audioProcessor.filter2Type->setValueNotifyingHost(curfilter_id / 4.0f);
            }

            else {
                istoggled_fil2 = false;
                audioProcessor.isfilterTwo->setValueNotifyingHost(false);
                audioProcessor.filter2_enabled = false;
                return;
            }
            break;

        case 3:
            if (curfiltertoggle.getToggleState() == true) {
                istoggled_fil3 = true;
                audioProcessor.isfilterThree->setValueNotifyingHost(true);
                audioProcessor.filter3_enabled = true;
                freq = &filter3freq;
                q = &filter3q;
                audioProcessor.filter3Freq->setValueNotifyingHost(filter3freq / 5000.0f);
                audioProcessor.filter3Q->setValueNotifyingHost(filter3q / 5.0f);
                audioProcessor.filter3Type->setValueNotifyingHost(curfilter_id / 4.0f);
            }
            else {
                istoggled_fil3 = false;
                audioProcessor.isfilterThree->setValueNotifyingHost(false);
                audioProcessor.filter3_enabled = false;
                return;
            }
            break;

        case 4:
            if (curfiltertoggle.getToggleState() == true) {
                istoggled_fil4 = true;
                audioProcessor.isfilterFour->setValueNotifyingHost(true);
                audioProcessor.filter4_enabled = true;
                freq = &filter4freq;
                q = &filter4q;
                audioProcessor.filter4Freq->setValueNotifyingHost(filter4freq / 5000.0f);
                audioProcessor.filter4Q->setValueNotifyingHost(filter4q / 5.0f);
                audioProcessor.filter4Type->setValueNotifyingHost(curfilter_id / 4.0f);
            }
            else {
                istoggled_fil4 = false;
                audioProcessor.isfilterFour->setValueNotifyingHost(false);
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

    if (presetlistbox.isParentOf(event.eventComponent)) {

        juce::File selectedFile = presetlistbox.getSelectedFile();
        if (selectedFile.exists()) {

            juce::File loadFile = audioProcessor.loadFileSound(selectedFile);
            if (loadFile.existsAsFile())
            {
                startPosition = 0;
                endPosition = audioProcessor.mainbuffer->getNumSamples();
                thumbnailCache.clear();
                thumbnail.clear(); // Clear the thumbnail first
                thumbnail.setSource(new juce::FileInputSource(loadFile));
                juce::Timer::callAfterDelay(500, [this]() { repaint(); });
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
        presetsthread.startThread();
        repaint();

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
                juce::File loadFile = audioProcessor.loadFileSound(result);
                juce::File resultFile(loadFile);
                startPosition = 0;
                endPosition = audioProcessor.mainbuffer->getNumSamples();
                thumbnail.clear();
                thumbnailCache.clear();
                thumbnail.setSource(new juce::FileInputSource(resultFile));
                juce::Timer::callAfterDelay(500, [this]() { repaint(); });
            }
        }
        else
        {
            DBG("No file selected for loading");
        }
        delete chooser;
            });
    }
    if (button == &helpbutton)
    {
        if (!helpWindow)
        {
            // Create the help window if it doesn't exist
            helpWindow = std::make_unique<HelpWindow>("Help", *this);
            helpWindow->setVisible(true);
        }
        else
        {
            // If the help window exists but is hidden, make it visible again
            if (!helpWindow->isVisible())
            {
                helpWindow->setVisible(true);
                helpWindow->toFront(true); // Bring it to the front
            }
        }
    }
    if (button == &toggleWindow)
    {
        if (slider_window->isVisible())
        {
            slider_window->setVisible(false);
        }
        else if (!slider_window->isVisible() && imgstate == "Path Loaded. Change parameters and Generate sound") {
            slider_window->setVisible(true);
        }
    }
    if (button == &refreshPreset)
    {
        presetscontentlst.refresh();
        presetlistbox.updateContent();
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
        juce::Rectangle<int> adjustedWaveviewerBounds = topBounds.reduced(0, viewToggle.getHeight() + 20);

        audioProcessor.waveviewer.setBounds(adjustedWaveviewerBounds);
    }
    else {
        audioProcessor.waveviewer.setVisible(false);
        if (thumbnail.getTotalLength() > 0.0)
        {
            g.setColour(juce::Colours::white);
            int thumbnailw = topBounds.getWidth();
            int thumbnailh = topBounds.getHeight() - 5;
            auto thumbnaildims = topBounds;
            thumbnaildims.setHeight(thumbnailh);

            thumbnail.drawChannels(g, thumbnaildims, 0.0, thumbnail.getTotalLength(), 1.0f);

            int numSamples = static_cast<int>(audioProcessor.mainbuffer->getNumSamples());
            float start = (static_cast<float>(startPosition) / numSamples) * thumbnaildims.getWidth();
            float end = (static_cast<float>(endPosition) / numSamples) * thumbnaildims.getWidth();

            g.setColour(juce::Colours::red);
            g.drawLine(thumbnaildims.getX() + start, thumbnaildims.getY(), thumbnaildims.getX() + start, thumbnaildims.getBottom());
            g.drawLine(thumbnaildims.getX() + end, thumbnaildims.getY(), thumbnaildims.getX() + end, thumbnaildims.getBottom());
        }
        else
        {
            g.setColour(juce::Colours::white);
            g.drawText(imgstate, topBounds, juce::Justification::centred);
        }
    }

    // Set viewToggle button position
    viewToggle.setBounds(topBounds.getX() + 145, topBounds.getBottom() - viewToggle.getHeight() + 5, 100, 50);

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


    //Filterlbl.setBounds(50, filterlblpos, 100, 30);
    filters.setBounds(10, filterlblpos - 5, 400, 420);
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
    sustainSlider.setBounds(200, envelopesectiony, 140, 90);
    releaseSlider.setBounds(300, envelopesectiony, 140, 90);




    presetlistbox.setBounds(700, 520, 250, 180);
    /*presets.toFront(true);*/
    presets.setBounds(695, 500, 280, 210);
    refreshPreset.setBounds(980, 670, 60, 30);
    //refreshPreset.toFront(true);
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
    int rightcorner = getWidth() - 50;


    //==================================================================oncl===
    //GainSlider
    viewToggle.setBounds(50, 10, 100, 30);
    gainSlider.setBounds(rightcorner - 205, yPosition + 180, 200, 150);

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
    saveButton.setBounds(0, bottomBounds.getY() - 35, 60, buttonHeight);
    loadButton.setBounds(70, bottomBounds.getY() - 35, 60, buttonHeight);
    helpbutton.setBounds(rightcorner - 20, bottomBounds.getY() - 35, 60, buttonHeight);
    toggleWindow.setBounds(rightcorner - 130, bottomBounds.getY() - 35, 100, buttonHeight);

    int dsliderWidth = 130;
    int dsliderHeight = 100;
    int delayGroupX = bottomBounds.getCentreX() + 60;
    int delayGroupY = bottomBounds.getY() + 5;
    int dsliderY = delayGroupY + 30;
    int dmarginX = 80;
    delay.setBounds(delayGroupX - 5, delayGroupY, 280, 210);
    delayEnabled.setBounds(delayGroupX + 25, delayGroupY, 80, 80);
    delayTime.setBounds(delayGroupX, dsliderY + 70, dsliderWidth, dsliderHeight);
    delayFeedback.setBounds((delayGroupX + dmarginX), dsliderY + 70, dsliderWidth, dsliderHeight);
    delayMix.setBounds((delayGroupX + (dmarginX * 2)), dsliderY + 70, dsliderWidth, dsliderHeight);



}

bool ImagineAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    // Accept only image files
    for (auto& file : files)
    {
        if (file.endsWithIgnoreCase(".jpg") || file.endsWithIgnoreCase(".png") || file.endsWithIgnoreCase("imag"))
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

    windowComponent->getKernelSlider().setValue(audioProcessor.kernel->get());
    windowComponent->getStepSlider().setValue(audioProcessor.stepSize->get());
    windowComponent->getSoundLevelSlider().setValue(audioProcessor.level->get());
    windowComponent->getSoundDurationSlider().setValue(audioProcessor.duration->get());
    windowComponent->getModulationIntensitySlider().setValue(audioProcessor.modulationIntensity->get());
    windowComponent->getModulationEnvelopeIntensitySlider().setValue(audioProcessor.modulationEnvelopeIntensity->get());
    windowComponent->getModulationDurationSlider().setValue(audioProcessor.modulationDuration->get());
    windowComponent->getLfoScalarFreqSlider().setValue(audioProcessor.lfoScalarFreq->get());
    windowComponent->getLfoScalarAmplitudeSlider().setValue(audioProcessor.lfoScalarAmplitude->get());
    windowComponent->getLfoIntensitySlider().setValue(audioProcessor.lfoIntensity->get());
    windowComponent->getOvertoneNumScalarSlider().setValue(audioProcessor.overtoneNumScalar->get());
    windowComponent->getLfoAmountScalarSlider().setValue(audioProcessor.lfoAmountScalar->get());


    documentsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    mainFolder = createFolderIfNotExists(documentsDir, "Imagine");
    outputFolder = createFolderIfNotExists(mainFolder, "output");
    outputPath = outputFolder.getFullPathName().toStdString() + "\\";

    for (const auto& file : files)
    {
        juce::File imageFile(file);
        if (imageFile.hasFileExtension("imag"))
        {
            juce::File loadFile = audioProcessor.loadFileSound(file);
            juce::File resultFile(loadFile);
            thumbnail.clear();
            thumbnail.setSource(new juce::FileInputSource(resultFile));
            repaint();
            return;
        }
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
    thumbnailCache.clear();
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
        startPosition = 0;
        endPosition = audioProcessor.mainbuffer->getNumSamples();
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
        audioProcessor.filterTwo->setValueNotifyingHost(false);
        audioProcessor.filterThree->setValueNotifyingHost(false);
        audioProcessor.filterFour->setValueNotifyingHost(false);


        break;
    case 2:
        filter1.setToggleState(false, juce::dontSendNotification);
        filter3.setToggleState(false, juce::dontSendNotification);
        filter4.setToggleState(false, juce::dontSendNotification);
        audioProcessor.filterOne->setValueNotifyingHost(false);
        audioProcessor.filterThree->setValueNotifyingHost(false);
        audioProcessor.filterFour->setValueNotifyingHost(false);
        break;
    case 3:
        filter1.setToggleState(false, juce::dontSendNotification);
        filter2.setToggleState(false, juce::dontSendNotification);
        filter4.setToggleState(false, juce::dontSendNotification);
        audioProcessor.filterOne->setValueNotifyingHost(false);
        audioProcessor.filterTwo->setValueNotifyingHost(false);
        audioProcessor.filterFour->setValueNotifyingHost(false);
        break;
    case 4:
        filter1.setToggleState(false, juce::dontSendNotification);
        filter2.setToggleState(false, juce::dontSendNotification);
        filter3.setToggleState(false, juce::dontSendNotification);
        audioProcessor.filterOne->setValueNotifyingHost(false);
        audioProcessor.filterTwo->setValueNotifyingHost(false);
        audioProcessor.filterThree->setValueNotifyingHost(false);
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
        if (istoggled_fil1 || audioProcessor.isfilterOne->get()) {
            curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        }
        else {
            curfiltertoggle.setToggleState(false, juce::dontSendNotification);
        }

    }
    else if (toggledButton == &filter2)
    {
        untoggleOtherFilters(2);
        if (istoggled_fil2 || audioProcessor.isfilterTwo->get()) {
            curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        }
        else {
            curfiltertoggle.setToggleState(false, juce::dontSendNotification);
        }

    }
    else if (toggledButton == &filter3)
    {
        untoggleOtherFilters(3);
        if (istoggled_fil3 || audioProcessor.isfilterThree->get()) {
            curfiltertoggle.setToggleState(true, juce::dontSendNotification);
        }
        else {
            curfiltertoggle.setToggleState(false, juce::dontSendNotification);
        }

    }
    else if (toggledButton == &filter4)
    {
        untoggleOtherFilters(4);
        if (istoggled_fil4 || audioProcessor.isfilterFour->get()) {
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