#include "SliderWindow.h"
#include "PluginProcessor.h"

SliderWindow::SliderWindow(ImagineAudioProcessorEditor* editor) : editor(editor)
{
    setSize(400, 950);
    
    addSlider(kernel, kernel_label, "Kernel Size", 0, 50, 25,1, "Controls range of the sound generation algorithm. Warning CPU intensive at high values");
    addSlider(step, step_label, "Step Size", 0, 50, 10,1, "Controls steps of the sound generation algorithm. Changes which parts of the image are processed");
    addSlider(sound_level, sound_label, "Level", 0, 10, 1,.1, "Controls sound level, can also add distortion.");
    addSlider(sound_duration, duration_label, "Time", 0, 30, 10,1, "Controls how long the sound lasts, can smoothen out the resultant sound.");
    addSlider(modulation_intensity, modulation_intensity_label, "Modulation Intensity", 0, 1, .8,.1, "Controls the intensity of modulation filter");
    addSlider(modulation_envelope_intensity, envelope_intensity_label, "Envelope Intensity (Falloff)", .1, 1, .2,.1, "Controls the intensity of falloff of the modulation filter");
    addSlider(modulation_duration, modulation_duration_label, "Modulation Duration (Stretch)", -20, 20, 6,.1, "Controls how long the modulation filter lasts");
    addSlider(lfo_scalar_freq, lfo_freq_label, "Lfo Frequency", 0,50, .5,.01, "Controls the frequency of lfo's.");
    addSlider(lfo_scalar_amplitude, lfo_amplitude_label, "Lfo Amplitude", 0, 1, .6,.1, "Controls the amplitude of the LFO's");
    addSlider(lfo_intensity, lfo_intensity_label, "Lfo Intensity", 0, 1, .6,.1, "Affects level of the lfo, also lowers final sound level.");
    addSlider(overtone_num_scalar, overtone_num_label, "Overtone Anmount", 0, 4, 1,.1, "Number of additive signals to add using the harmonic series");
    addSlider(lfo_amount_scalar, lfo_amount_label, "Lfo Amount ", 0, 4, 1,.1, "Generates low frequency oscillators using the image information");
        

    soundGenerationGroup.setText("Sound Generation");
    addAndMakeVisible(soundGenerationGroup);
    modulationGroup.setText("Modulation");
    addAndMakeVisible(modulationGroup);
    baseToneGroup.setText("Base Tone");
    addAndMakeVisible(baseToneGroup);

    addAndMakeVisible(generateButton);
    generateButton.onClick = [editor] { editor->generateSound(); };



    addAndMakeVisible(saveButton);
    saveButton.setButtonText("Save");
    saveButton.onClick = [this] {
        DBG("Save button clicked");
        auto* chooser = new juce::FileChooser("Save Parameters", {}, "*.imag");
        chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc)
            {
                DBG("Save FileChooser lambda entered");
        auto result = fc.getResult();
        if (result != juce::File{})
        {
            DBG("Saving to file: " + result.getFullPathName());
            saveParameters(result);
        }
        else
        {
            DBG("No file selected for saving");
        }
        delete chooser; 
            });
    };

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Load");
    loadButton.onClick = [this] {
        DBG("Load button clicked");
        auto* chooser = new juce::FileChooser("Load Parameters", {}, "*.imag");
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc)
            {
                DBG("Load FileChooser lambda entered");
        auto result = fc.getResult();
        if (result != juce::File{})
        {
            DBG("Loading from file: " + result.getFullPathName());
            loadParameters(result);
        }
        else
        {
            DBG("No file selected for loading");
        }
        delete chooser; 
            });
    };

}

SliderWindow::~SliderWindow()
{
}

void SliderWindow::saveParameters(const juce::File& file)
{
    std::unique_ptr<juce::XmlElement> rootElement = juce::XmlDocument::parse(file);
    if (rootElement == nullptr)
    {
        rootElement = std::make_unique<juce::XmlElement>("Root");
    }

    // Remove existing Parameters if present
    if (auto* existingParameters = rootElement->getChildByName("Parameters"))
    {
        rootElement->removeChildElement(existingParameters, true);
    }

    juce::XmlElement* parameters = new juce::XmlElement("Parameters");
    parameters->setAttribute("Kernel", kernel.getValue());
    parameters->setAttribute("Step", step.getValue());
    parameters->setAttribute("SoundLevel", sound_level.getValue());
    parameters->setAttribute("SoundDuration", sound_duration.getValue());
    parameters->setAttribute("ModulationIntensity", modulation_intensity.getValue());
    parameters->setAttribute("ModulationEnvelopeIntensity", modulation_envelope_intensity.getValue());
    parameters->setAttribute("ModulationDuration", modulation_duration.getValue());
    parameters->setAttribute("LfoScalarFreq", lfo_scalar_freq.getValue());
    parameters->setAttribute("LfoScalarAmplitude", lfo_scalar_amplitude.getValue());
    parameters->setAttribute("LfoIntensity", lfo_intensity.getValue());
    parameters->setAttribute("OvertoneNumScalar", overtone_num_scalar.getValue());
    parameters->setAttribute("LfoAmountScalar", lfo_amount_scalar.getValue());

    rootElement->addChildElement(parameters);

    if (!rootElement->writeToFile(file, {}))
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "Save Error",
            "Could not save parameters to file.");
    }
}

void SliderWindow::loadParameters(const juce::File& file)
{
    juce::XmlDocument doc(file);
    std::unique_ptr<juce::XmlElement> rootElement(doc.getDocumentElement());

    if (rootElement == nullptr || !rootElement->hasTagName("Root"))
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "Load Error",
            "Could not load parameters from file.");
        return;
    }
    auto* parameters = rootElement->getChildByName("Parameters");

    kernel.setValue(parameters->getDoubleAttribute("Kernel", kernel.getValue()));
    step.setValue(parameters->getDoubleAttribute("Step", step.getValue()));
    sound_level.setValue(parameters->getDoubleAttribute("SoundLevel", sound_level.getValue()));
    sound_duration.setValue(parameters->getDoubleAttribute("SoundDuration", sound_duration.getValue()));
    modulation_intensity.setValue(parameters->getDoubleAttribute("ModulationIntensity", modulation_intensity.getValue()));
    modulation_envelope_intensity.setValue(parameters->getDoubleAttribute("ModulationEnvelopeIntensity", modulation_envelope_intensity.getValue()));
    modulation_duration.setValue(parameters->getDoubleAttribute("ModulationDuration", modulation_duration.getValue()));
    lfo_scalar_freq.setValue(parameters->getDoubleAttribute("LfoScalarFreq", lfo_scalar_freq.getValue()));
    lfo_scalar_amplitude.setValue(parameters->getDoubleAttribute("LfoScalarAmplitude", lfo_scalar_amplitude.getValue()));
    lfo_intensity.setValue(parameters->getDoubleAttribute("LfoIntensity", lfo_intensity.getValue()));
    overtone_num_scalar.setValue(parameters->getDoubleAttribute("OvertoneNumScalar", overtone_num_scalar.getValue()));
    lfo_amount_scalar.setValue(parameters->getDoubleAttribute("LfoAmountScalar", lfo_amount_scalar.getValue()));
}


void SliderWindow::paint(juce::Graphics& g)
{
    g.fillAll(editor->charcoal);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void SliderWindow::resized()
{

    soundGenerationGroup.setBoundsRelative(0.1f, 0.05f, 0.8f, 0.25f);
    modulationGroup.setBoundsRelative(0.1f, 0.33f, 0.8f, 0.25f);
    baseToneGroup.setBoundsRelative(0.1f, 0.61f, 0.8f, 0.25f);


    setPositionWithinGroup(soundGenerationGroup, kernel, kernel_label, 0.0f, 0.02f, 0.25f, 0.30f);
    setPositionWithinGroup(soundGenerationGroup, step, step_label, 0.3f, 0.02f, 0.25f, 0.30f);
    setPositionWithinGroup(soundGenerationGroup, sound_level, sound_label, 0.6f, 0.02f, 0.25f, 0.30f);
    setPositionWithinGroup(soundGenerationGroup, sound_duration, duration_label, 0.0f, 0.52f, 0.25f, 0.30f);


    setPositionWithinGroup(modulationGroup, modulation_intensity, modulation_intensity_label, 0.0f, 0.03f, 0.25f, 0.30f);
    setPositionWithinGroup(modulationGroup, modulation_envelope_intensity, envelope_intensity_label, 0.3f, 0.03f, 0.25f, 0.30f);
    setPositionWithinGroup(modulationGroup, modulation_duration, modulation_duration_label, 0.6f, 0.03f, 0.25f, 0.30f);


    setPositionWithinGroup(baseToneGroup, overtone_num_scalar, overtone_num_label, 0.0f, 0.02f, 0.25f, 0.30f);
    setPositionWithinGroup(baseToneGroup, lfo_scalar_freq, lfo_freq_label, 0.3f, 0.02f, 0.25f, 0.30f);
    setPositionWithinGroup(baseToneGroup, lfo_scalar_amplitude, lfo_amplitude_label, 0.6f, 0.02f, 0.25f, 0.30f);
    setPositionWithinGroup(baseToneGroup, lfo_intensity, lfo_intensity_label, 0.0f, 0.52f, 0.25f, 0.30f);
    setPositionWithinGroup(baseToneGroup, lfo_amount_scalar, lfo_amount_label, 0.3f, 0.52f, 0.25f, 0.30f);


    generateButton.setBoundsRelative(0.1f, 0.87f, 0.8f, 0.04f);

    saveButton.setBoundsRelative(0.1f, 0.93f, 0.35f, 0.03f);
    loadButton.setBoundsRelative(0.55f, 0.93f, 0.35f, 0.03f);

}

void SliderWindow::sliderValueChanged(juce::Slider* slider)
{

}

void SliderWindow::addSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double defaultValue,double interval, std::string tooltip)
{
    slider.setRange(min, max,interval);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
    slider.setTooltip(tooltip);
    slider.addListener(this);

    label.setText(name, juce::dontSendNotification);
    label.attachToComponent(&slider, false); 

}

void SliderWindow::setPositionWithinGroup(juce::Component& group, juce::Slider& slider, juce::Label& label, float relX, float relY, float relWidth, float relHeight)
{
    group.addAndMakeVisible(slider);
    group.addAndMakeVisible(label);
    slider.setBoundsRelative(relX, relY + 0.14f, relWidth, relHeight); 
    label.setBoundsRelative(relX, relY, relWidth, 0.2f);

}
