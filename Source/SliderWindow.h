
#pragma once
#include <JuceHeader.h>
#include "PluginEditor.h"

class ImagineAudioProcessorEditor;

class SliderWindow : public juce::Component, public juce::Slider::Listener
{
public:
	SliderWindow(ImagineAudioProcessorEditor* editor);
	~SliderWindow();

	void paint(juce::Graphics&) override;
	void resized() override;
	void sliderValueChanged(juce::Slider* slider) override;
	void addSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double default);

	juce::Slider& getKernelSlider() { return kernel; }
	juce::Slider& getStepSlider() { return step; }
	juce::Slider& getSoundLevelSlider() { return sound_level; }
	juce::Slider& getSoundDurationSlider() { return sound_duration; }
	juce::Slider& getModulationDurationSlider() { return modulation_duration; }
	juce::Slider& getModulationIntensitySlider() { return modulation_intensity; }
	juce::Slider& getModulationEnvelopeIntensitySlider() { return modulation_envelope_intensity; }
	juce::Slider& getOvertoneNumScalarSlider() { return overtone_num_scalar; }
	juce::Slider& getLfoScalarFreqSlider() { return lfo_scalar_freq; }
	juce::Slider& getLfoScalarAmplitudeSlider() { return lfo_scalar_amplitude; }
	juce::Slider& getLfoIntensitySlider() { return lfo_intensity; }
	juce::Slider& getLfoAmountScalarSlider() { return lfo_amount_scalar; }

private:

	juce::Slider kernel, step, sound_level, sound_duration, modulation_duration, modulation_intensity,
		modulation_envelope_intensity, overtone_num_scalar, lfo_scalar_freq, lfo_scalar_amplitude, lfo_intensity, lfo_amount_scalar;
	juce::Label kernel_label, step_label, sound_label, duration_label, modulation_duration_label, modulation_intensity_label,
		envelope_intensity_label, overtone_num_label, lfo_freq_label, lfo_amplitude_label, lfo_intensity_label, lfo_amount_label;

	ImagineAudioProcessorEditor* editor;
	juce::TextButton generateButton{ "Generate Sound" };

	juce::GroupComponent soundGenerationGroup;
	juce::GroupComponent modulationGroup;
	juce::GroupComponent baseToneGroup;

	juce::Viewport viewport;
	juce::Component contentComponent;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderWindow)
};