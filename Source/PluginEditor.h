/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <string>
#include <filesystem>
#include "SliderWindow.h"


namespace fs = std::filesystem;
class SliderWindow;
//==============================================================================
/**
* 
* 
* 
* 
* 
* 
*
*/



class ImagineAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::FileDragAndDropTarget, private juce::Slider::Listener, public juce::MouseListener, public juce::Button::Listener, public juce::Timer
{
public:

    //Colors configuration
    juce::Colour charcoal = juce::Colour::fromRGB(51, 62, 72);   // Charcoal
    juce::Colour gunmetal = juce::Colour::fromRGB(34, 41, 48);
    juce::Colour ashgrey = juce::Colour::fromRGB(193, 201, 187);
    juce::Colour roseTaupe = juce::Colour::fromRGB(127, 98, 104); // Rose taupe
    juce::Colour roseTaupeLight = juce::Colour::fromRGB(144, 100, 104); // Rose taupe (lighter variant)
    juce::Colour rosyBrown = juce::Colour::fromRGB(176, 130, 130); // Rosy brown
    juce::Colour ivory = juce::Colour::fromRGB(242, 245, 234); // Ivory
    juce::Colour timberwolf = juce::Colour::fromRGB(214, 219, 210); // Timberwolf
    juce::Colour cadetGray1 = juce::Colour::fromRGB(136, 152, 170); // Cadet gray (first variant)
    juce::Colour cadetGray2 = juce::Colour::fromRGB(141, 160, 181); // Cadet gray (second variant)
    juce::Colour silverLakeBlue = juce::Colour::fromRGB(116, 140, 171); // Silver Lake Blue
    juce::Colour mintGreen = juce::Colour::fromRGB(209, 227, 221); // Mint green
    juce::Colour quinacridoneMagenta = juce::Colour::fromRGB(153, 57, 85);  // Quinacridone magenta
    juce::Colour lavenderFloral = juce::Colour::fromRGB(164, 145, 211); // Lavender (floral)
    juce::Colour carrotOrange = juce::Colour::fromRGB(243, 146, 55);  // Carrot orange
    

    juce::Colour topcolor = gunmetal;
    juce::Colour topbordercolor = gunmetal;
    juce::Colour bottomcolor = charcoal;
    juce::Colour bottombordercolor = ashgrey;

    //-----------------------------------------------------------//

    int numofbuttons = 4;
    int numofsliders = 4;
    juce::TextButton buttonarray[4];
    juce::Slider    sliderarray[4];

    juce::Slider paramslider1, paramslider2, paramslider3, paramslider4;


    //Parameters Effects and stuff
    //=================================================================================
    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::LookAndFeel_V4 gainLookAndFeel;


    //Filters
    juce::Label Filterlbl{ "Filter" };
    juce::ToggleButton filter1{ "Filter 1" }, filter2{ "Filter 2" }, filter3{ "Filter 3" }, filter4{ "Filter 4" };
    bool istoggled_fil1{false}, istoggled_fil2{ false }, istoggled_fil3{ false }, istoggled_fil4{ false };
    int filter1freq{20}, filter2freq{ 20 }, filter3freq{ 20 }, filter4freq{ 20 }, tempfreq{ 20 };
    float filter1q{ 0.1 }, filter2q{ 0.1 }, filter3q{ 0.1 }, filter4q{ 0.1 }, tempq{ 0.1};



    juce::ComboBox curfiltertype_combobox;
    juce::Label is_enabledlabel;
    juce::ToggleButton curfiltertoggle;
    juce::Slider curfilterfreq;
    juce::Slider cur_q_val;
    juce::Label freqfilterlbl;
    juce::Label qfilterlbl;

    //Envelope
    //========================================
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    juce::Label attacklabel;
    juce::Label decaylabel;
    juce::Label sustainlabel;
    juce::Label releaselabel;

    int findtoggledfilter();
    void untoggleOtherFilters(int selectedFilter);
    int findfilternum();
    void onFilterToggled(juce::Button* toggledButton);
    void initializeSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
        float minValue, float maxValue, float defaultValue, float interval);
    void updatefilters();
    //------------------------------------------------------------//
    //live view

    juce::ToggleButton viewToggle;

    //-----------------------------------------------------------//
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    //-----------------------------------------------------------//

    std::string imgstate = "Imagine: Drag an image to begin";

    //-----------------------------------------------------------//

    ImagineAudioProcessorEditor (ImagineAudioProcessor&);
    ~ImagineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;


    
    juce::File documentsDir;
    juce::File mainFolder;
    juce::File imgsFolder;
    juce::File outputFolder;
    std::string imgsPath;
    std::string outputPath;

    void deleteFiles(const juce::File& folder);

    enum TransportState
    {
        Starting,
        Stopping
    };

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioDeviceManager deviceManager;


    juce::TextButton block{ "Play Block" };
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event);
    juce::Rectangle<int> bounds;
    juce::Rectangle<int> topBounds;
    juce::GroupComponent generation_sliders;
    void changeState(TransportState newState);
    TransportState state;
    void addSlider(juce::Slider& slider, juce::Label& label, const juce::String& name, double min, double max, double default);

    juce::File getImageFile()
    {
        return this->imageFile;
    }

    juce::File setImageFile(juce::File imageFile)
    {
        this->imageFile = imageFile;
    }
    void generateSound();

    int startPosition, endPosition;


    juce::Slider reverbRoomSize, reverbDamping, reverbWet, reverbDry, reverbWidth;
    juce::Label reverbRoomSizeLabel, reverbDampingLabel, reverbWetLabel, reverbDryLabel, reverbWidthLabel;
    juce::ToggleButton reverbEnabled;
    juce::GroupComponent reverb;

    juce::Slider delayTime, delayFeedback, delayMix;
    juce::Label delayTimeLabel, delayFeedbackLabel, delayMixLabel;
    juce::ToggleButton delayEnabled;
    juce::GroupComponent delay;


    juce::TextButton saveButton;
    juce::TextButton loadButton; 

    void loadThumbnailAsync(const juce::File& file);
    void drawLiveBuffer(juce::Graphics& g, juce::Rectangle<int> bounds);
    void timerCallback();
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ImagineAudioProcessor& audioProcessor;
    juce::File createFolderIfNotExists(const juce::File& parentFolder, const std::string& folderName);
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    juce::File imageFile;
    SliderWindow* windowComponent;
    std::string imagePath;
    std::unique_ptr<juce::DocumentWindow> slider_window;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImagineAudioProcessorEditor)

};


