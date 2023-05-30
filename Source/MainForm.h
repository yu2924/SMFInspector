/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "SMFParser.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainForm  : public juce::Component
{
public:
    //==============================================================================
    MainForm ();
    ~MainForm() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    // internals
    void clearContent();
    void openContent(const juce::File& path);
    int findNextSysexEventIndex(int itr, int iev, bool wrap, bool forward) const;
    std::vector<int> findNextSysexEventIndices(int itr, int iev, bool wrap, bool forward) const;
    // juce::TableListBoxModel reflections
    int getNumRows(int itr);
    void paintRowBackground(int itr, juce::Graphics& g, int row, int width, int height, bool selected);
    void paintCell(int itr, juce::Graphics& g, int row, int colid, int width, int height, bool selected);
    void backgroundClicked(int itr, const juce::MouseEvent& me);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    juce::File mSmfPath;
	std::unique_ptr<SMFParser> mSMFParser;
	juce::OwnedArray<juce::TableListBoxModel> mTrackModels;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> mFileTitleLabel;
    std::unique_ptr<juce::Label> mFilePathLabel;
    std::unique_ptr<juce::TextButton> mBrowseButton;
    std::unique_ptr<juce::TextButton> mClearButton;
    std::unique_ptr<juce::Label> mHeaderTitleLabel;
    std::unique_ptr<juce::Label> mHeaderValueLabel;
    std::unique_ptr<juce::Label> mSysexLabel;
    std::unique_ptr<juce::TextButton> mSysexSelPrevButton;
    std::unique_ptr<juce::TextButton> mSysexSelNextButton;
    std::unique_ptr<juce::TextButton> mSysexSelAllButton;
    std::unique_ptr<juce::TextButton> mSysexExportButton;
    std::unique_ptr<juce::TabbedComponent> mTracksTabCtrl;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainForm)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

