// HelpWindow.h
#pragma once

#include <JuceHeader.h>

class HelpWindow : public juce::DocumentWindow
{
public:
    HelpWindow(const juce::String& name, juce::Component& parentEditor)
        : juce::DocumentWindow(name,
            parentEditor.findColour(juce::ResizableWindow::backgroundColourId),
            juce::DocumentWindow::closeButton)
    {
        // Set window properties
        setUsingNativeTitleBar(true);
        setResizable(false, false);
        setAlwaysOnTop(true);
        setSize(400, 300);  // Ensure that the help window has a reasonable size

        // Create a TextEditor to display help text
        helpTextEditor.setMultiLine(true);
        helpTextEditor.setReadOnly(true);
        helpTextEditor.setColour(juce::TextEditor::backgroundColourId,
            parentEditor.findColour(juce::ResizableWindow::backgroundColourId));
        helpTextEditor.setColour(juce::TextEditor::textColourId, juce::Colours::white);

        // Set help text
        helpTextEditor.setText("Imagine - A Synthesizer that Generates Sounds from Images\n\n"
            "Created as a Capstone graduation project.\n\n"
            "GitHub Link: https://github.com/Adison-Negron/Imagine\n\n"
            "Developed by Adrian Irizarry and Adison Negron.\n\n"
            "The parameter window can be used to tweak generation parameters and customize the resultant waveform.\n\n"
            "Double-click the top window to reset sound play bounds, and use right-click and left-click to establish the range of the sound played.\n\n"
            "Use the audio effect parameters to modify the resultant sound.");

        // Set size and add content
        helpTextEditor.setBounds(0, 0, getWidth(), getHeight()); // Make sure the TextEditor has valid dimensions
        setContentOwned(&helpTextEditor, false);

        // Centre the window on the screen
        centreWithSize(getWidth(), getHeight());
    }

    void closeButtonPressed() override
    {
        // Close the window when the close button is pressed
        setVisible(false);
    }

private:
    juce::TextEditor helpTextEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelpWindow)
};