#include "SliderWindow.h"
#include "PluginProcessor.h"

SliderWindow::SliderWindow(ImagineAudioProcessorEditor* editor) : editor(editor)
{
    setSize(800, 300); // Adjust the width as needed

    addSlider(kernel, kernel_label, "Kernel Size", 0, 500, 25);
    addSlider(step, step_label, "Step Size", 0, 50, 10);
    addSlider(sound_level, sound_label, "Sound Level", 0, 10, 1);
    addSlider(sample_rate, sample_label, "Sample Rate", 22050, 44100, 44100);
    addSlider(sound_duration, duration_label, "Sound Duration", 0, 20, 6);
    addSlider(modulation_intensity, modulation_intensity_label, "Modulation Intensity", 0, 1, .8);
    addSlider(modulation_envelope_intensity, envelope_intensity_label, "Envelope Intensity", 0, 1, .2);
    addSlider(modulation_duration, modulation_duration_label, "Modulation Duration", 0, 20, 6);
    addSlider(lfo_scalar_freq, lfo_freq_label, "Lfo Frequency", 0, 1, .6);
    addSlider(lfo_scalar_amplitude, lfo_amplitude_label, "Lfo Amplitude", 0, 1, 1);
    addSlider(lfo_intensity, lfo_intensity_label, "Lfo Intensity", 0, 1, 1);
    addSlider(overtone_num_scalar, overtone_num_label, "Overtone Num", 0, 20, 1);
    addSlider(lfo_amount_scalar, lfo_amount_label, "Lfo Amount", 0, 20, 1);

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
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.alignContent = juce::FlexBox::AlignContent::flexStart;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    flexBox.alignItems = juce::FlexBox::AlignItems::flexStart;

    auto addItem = [&](juce::Slider& slider, juce::Label& label)
    {
        label.setJustificationType(juce::Justification::centred);
        label.setSize(80, 20); // Set fixed size for labels
        flexBox.items.add(juce::FlexItem(label).withWidth(80).withHeight(20).withMargin(juce::FlexItem::Margin(2, 2, 10, 2)));
        flexBox.items.add(juce::FlexItem(slider).withMinWidth(80).withMinHeight(80).withMargin(juce::FlexItem::Margin(2)));
    };

    addItem(kernel, kernel_label);
    addItem(step, step_label);
    addItem(sound_level, sound_label);
    addItem(sample_rate, sample_label);
    addItem(sound_duration, duration_label);
    addItem(modulation_intensity, modulation_intensity_label);
    addItem(modulation_envelope_intensity, envelope_intensity_label);
    addItem(modulation_duration, modulation_duration_label);
    addItem(lfo_scalar_freq, lfo_freq_label);
    addItem(lfo_scalar_amplitude, lfo_amplitude_label);
    addItem(lfo_intensity, lfo_intensity_label);
    addItem(overtone_num_scalar, overtone_num_label);
    addItem(lfo_amount_scalar, lfo_amount_label);

    flexBox.performLayout(getLocalBounds().reduced(10).removeFromTop(300)); // Adjust the height if needed

    generateButton.setBounds(getLocalBounds().removeFromBottom(40).reduced(10));
}

void SliderWindow::sliderValueChanged(juce::Slider* slider)
{
    // Implement slider change handling here if needed
}

void SliderWindow::addSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double defaultValue)
{
    slider.setRange(min, max);
    slider.setValue(defaultValue);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.attachToComponent(&slider, false); // Position label above the slider
    addAndMakeVisible(label);
}