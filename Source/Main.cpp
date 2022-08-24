//
//  Main.cpp
//  SMFInspector
//
//  Created by yu2924 on 2016-12-17
//

#include <JuceHeader.h>
#include "MainForm.h"

#if JUCE_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOUSER
#define NOKERNEL
#include <Windows.h>
#endif

static juce::String SystemDefaultTypefaceName()
{
#if JUCE_WINDOWS
	NONCLIENTMETRICSW ncm = { sizeof(ncm) }; SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	return ncm.lfMessageFont.lfFaceName;
#elif JUCE_MAC
	return "Lucida Grande";
#else
	return "Helvetica";
#endif
}

class MainWindow
	: public juce::DocumentWindow
	, public juce::FileDragAndDropTarget
{
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
public:
	juce::TooltipWindow mTooltipWindow;
	std::unique_ptr<MainForm> mMainForm;
	MainWindow(juce::String name)
		: DocumentWindow(name, juce::Colours::lightgrey, DocumentWindow::allButtons)
	{
		setUsingNativeTitleBar(true);
		setResizable(true, true);
		mMainForm.reset(new MainForm);
		setContentNonOwned(mMainForm.get(), true);
		centreWithSize(getWidth(), getHeight());
		setVisible(true);
	}
	void closeButtonPressed() override
	{
		juce::JUCEApplication::getInstance()->systemRequestedQuit();
	}
	// juce::FileDragAndDropTarget
	virtual bool isInterestedInFileDrag(const juce::StringArray& files) override
	{
		if(files.size() != 1) return false;
		static const juce::StringArray ExtList = { ".mid", ".rmi" };
		return ExtList.contains(juce::File(files[0]).getFileExtension(), true);
	}
	virtual void filesDropped(const juce::StringArray& files, int, int) override
	{
		mMainForm->openContent(juce::File(files[0]));
	}
};

class SMFInspectorApplication
	: public juce::JUCEApplication
{
private:
	std::unique_ptr<MainWindow> mainWindow;
public:
	SMFInspectorApplication() {}
	virtual const juce::String getApplicationName() override { return ProjectInfo::projectName; }
	virtual const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
	virtual bool moreThanOneInstanceAllowed() override { return true; }
	virtual void initialise(const juce::String&) override
	{
		juce::LookAndFeel& lf = juce::LookAndFeel::getDefaultLookAndFeel();
		lf.setDefaultSansSerifTypefaceName(SystemDefaultTypefaceName());
		if(juce::LookAndFeel_V4* lf4 = dynamic_cast<juce::LookAndFeel_V4*>(&lf))
		{
			lf4->setColourScheme(juce::LookAndFeel_V4::getLightColourScheme());
		}
		mainWindow.reset(new MainWindow(getApplicationName()));
	}
	virtual void shutdown() override
	{
		mainWindow = nullptr;
	}
	virtual void systemRequestedQuit() override
	{
		quit();
	}
	virtual void anotherInstanceStarted(const juce::String&) override
	{
	}
};

START_JUCE_APPLICATION(SMFInspectorApplication)
