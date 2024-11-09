#include "SliderWindow.h"
#include "PluginProcessor.h"

SliderWindow::SliderWindow(ImagineAudioProcessorEditor* editor) : editor(editor)
{
    setSize(400, 950);
    
    addSlider(kernel, kernel_label, "Kernel Size", 0, 100, 25,1);
    addSlider(step, step_label, "Step Size", 0, 50, 10,1);
    addSlider(sound_level, sound_label, "Level", 0, 10, 1,.1);
    addSlider(sound_duration, duration_label, "Time", 0, 30, 10,1);
    addSlider(modulation_intensity, modulation_intensity_label, "Modulation Intensity", 0, 1, .8,.1);
    addSlider(modulation_envelope_intensity, envelope_intensity_label, "Envelope Intensity (Falloff)", .1, 1, .2,.1);
    addSlider(modulation_duration, modulation_duration_label, "Modulation Duration (Stretch)", -20, 20, 6,.1);
    addSlider(lfo_scalar_freq, lfo_freq_label, "Lfo Frequency", 0,50, .5,.01);
    addSlider(lfo_scalar_amplitude, lfo_amplitude_label, "Lfo Amplitude", 0, 1, .6,.1);
    addSlider(lfo_intensity, lfo_intensity_label, "Lfo Intensity", 0, 1, .6,.1);
    addSlider(overtone_num_scalar, overtone_num_label, "Overtone Anmount", 0, 4, 1,.1);
    addSlider(lfo_amount_scalar, lfo_amount_label, "Lfo Amount ", 0, 4, 1,.1);
        

    soundGenerationGroup.setText("Sound Generation");
    addAndMakeVisible(soundGenerationGroup);
    modulationGroup.setText("Modulation");
    addAndMakeVisible(modulationGroup);
    baseToneGroup.setText("Base Tone");
    addAndMakeVisible(baseToneGroup);

    addAndMakeVisible(generateButton);
    generateButton.onClick = [editor] { editor->generateSound(); };

}

SliderWindow::~SliderWindow()
{
}

void SliderWindow::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void SliderWindow::resized()
{

    soundGenerationGroup.setBoundsRelative(0.1f, 0.1f, 0.8f, 0.25f);
    modulationGroup.setBoundsRelative(0.1f, 0.38f, 0.8f, 0.25f);
    baseToneGroup.setBoundsRelative(0.1f, 0.66f, 0.8f, 0.25f);


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


    generateButton.setBoundsRelative(0.1f, 0.93f, 0.8f, 0.05f);

}

void SliderWindow::sliderValueChanged(juce::Slider* slider)
{

}

void SliderWindow::addSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double defaultValue,double interval)
{
    slider.setRange(min, max,interval);
    slider.setValue(defaultValue);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);

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
