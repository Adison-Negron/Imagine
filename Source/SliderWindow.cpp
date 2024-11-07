#include "SliderWindow.h"
#include "PluginProcessor.h"

SliderWindow::SliderWindow(ImagineAudioProcessorEditor* editor) : editor(editor)
{
    setSize(400, 950);
    
    addSlider(kernel, kernel_label, "Kernel Size", 0, 100, 25,1);
    addSlider(step, step_label, "Step Size", 0, 50, 10,1);
    addSlider(sound_level, sound_label, "Level", 0, 10, 1,.1);
    addSlider(sound_duration, duration_label, "Length scalar (Normalizer)", 0, 20, 6,1);
    addSlider(modulation_intensity, modulation_intensity_label, "Modulation Intensity", 0, 1, .8,.1);
    addSlider(modulation_envelope_intensity, envelope_intensity_label, "Envelope Intensity (Falloff)", .1, 1, .2,.1);
    addSlider(modulation_duration, modulation_duration_label, "Modulation Duration (Stretch)", -20, 20, 6,.1);
    addSlider(lfo_scalar_freq, lfo_freq_label, "Lfo Frequency", 0, 10, .5,.01);
    addSlider(lfo_scalar_amplitude, lfo_amplitude_label, "Lfo Amplitude", 0, 1, 1,.1);
    addSlider(lfo_intensity, lfo_intensity_label, "Lfo Intensity", 0, 1, 1,.1);
    addSlider(overtone_num_scalar, overtone_num_label, "Overtone Anmount", 0, 4, 1,.1);
    addSlider(lfo_amount_scalar, lfo_amount_label, "Lfo Amount ", 0, 4, 1,.1);
        

    soundGenerationGroup.setText("Sound Generation");
    contentComponent.addAndMakeVisible(soundGenerationGroup);
    modulationGroup.setText("Modulation");
    contentComponent.addAndMakeVisible(modulationGroup);
    baseToneGroup.setText("Base Tone");
    contentComponent.addAndMakeVisible(baseToneGroup);

    contentComponent.addAndMakeVisible(generateButton);
    generateButton.onClick = [editor] { editor->generateSound(); };

    viewport.setViewedComponent(&contentComponent, false);
    addAndMakeVisible(viewport);
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

    viewport.setBounds(getLocalBounds());
    int padding = 10;
    int sliderWidth = 70;
    int sliderHeight = 70;
    int labelHeight = 40;
    int labelWidth = 70; 
    int groupPadding = 30;
    int numColumns = 3;

    int totalWidth = numColumns * (sliderWidth + padding) - padding;
    int xPos = (getWidth() - totalWidth) / 2;


    soundGenerationGroup.setBounds(xPos - groupPadding, 10, totalWidth + 2 * groupPadding, 2 * (sliderHeight + labelHeight + padding) + groupPadding);
    int yPos = 30 + groupPadding / 2;

    auto setPosition = [&](juce::Slider& slider, juce::Label& label)
    {
        slider.setBounds(xPos, yPos + labelHeight, sliderWidth, sliderHeight);
        label.setBounds(xPos, yPos, labelWidth, labelHeight); 
        xPos += sliderWidth + padding;

        if ((xPos + sliderWidth) > (getWidth() - groupPadding))
        {
            xPos = (getWidth() - totalWidth) / 2;
            yPos += sliderHeight + labelHeight + padding;
        }
    };

    setPosition(kernel, kernel_label);
    setPosition(step, step_label);
    setPosition(sound_level, sound_label);
    setPosition(sound_duration, duration_label);

    xPos = (getWidth() - totalWidth) / 2;
    yPos += sliderHeight + labelHeight + padding + groupPadding;
    modulationGroup.setBounds(xPos - groupPadding, yPos - groupPadding / 2, totalWidth + 2 * groupPadding, (sliderHeight + labelHeight + padding) * 2 + groupPadding);
    yPos += groupPadding / 2;

    setPosition(modulation_intensity, modulation_intensity_label);
    setPosition(modulation_envelope_intensity, envelope_intensity_label);
    setPosition(modulation_duration, modulation_duration_label);

    xPos = (getWidth() - totalWidth) / 2;
    yPos += sliderHeight + labelHeight + padding + groupPadding;
    baseToneGroup.setBounds(xPos - groupPadding, yPos - groupPadding / 2, totalWidth + 2 * groupPadding, (sliderHeight + labelHeight + padding) * 2 + groupPadding);
    yPos += groupPadding / 2;

    setPosition(overtone_num_scalar, overtone_num_label);
    setPosition(lfo_scalar_freq, lfo_freq_label);
    setPosition(lfo_scalar_amplitude, lfo_amplitude_label);
    setPosition(lfo_intensity, lfo_intensity_label);
    setPosition(lfo_amount_scalar, lfo_amount_label);

    generateButton.setBounds((xPos/2) - 40, (yPos + sliderHeight + labelHeight + padding) + 10, totalWidth, 30);
    contentComponent.setSize(getWidth(), yPos + sliderHeight + labelHeight + padding + groupPadding + 30);

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
    addAndMakeVisible(slider);
    contentComponent.addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.attachToComponent(&slider, false); 
    addAndMakeVisible(label);
    contentComponent.addAndMakeVisible(label);
}