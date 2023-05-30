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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "MainForm.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

static juce::String ResolveCCName(uint8_t cc)
{
	static juce::Array<juce::String> table;
	if(table.isEmpty())
	{
		table.insertMultiple(0, "", 128);
		for(const auto& line : juce::StringArray::fromLines(BinaryData::Control_Functions_txt))
		{
			if(line.isEmpty()) continue;
			juce::StringArray tokens = juce::StringArray::fromTokens(line, "\t", "");
			if(tokens.size() != 3) continue;
			int ii = tokens[0].getHexValue32();
			if(128 <= ii) continue;
			table.getReference(ii) = tokens[2];
		}
	}
	return (cc < 128u) ? table[cc] : juce::String::formatted("#%u", cc);
}

static juce::String ResolveMFIDName(const uint8_t* px, int cx)
{
	static juce::HashMap<int, juce::String> table;
	if(table.size() == 0)
	{
		for(const auto& line : juce::StringArray::fromLines(BinaryData::Manufacturer_ID_Numbers_txt))
		{
			if(line.isEmpty()) continue;
			juce::StringArray tokens = juce::StringArray::fromTokens(line, "\t", "");
			if(tokens.size() != 3) continue;
			juce::StringArray ks = juce::StringArray::fromTokens(tokens[0], " ", "");
			uint32_t k = 0;
			for(int c = ks.size(), i = 0; i < c; i ++)
			{
				k |= (uint32_t)ks[i].getHexValue32() << (8 * i);
			}
			juce::String v = tokens[2];
			table.set(k, v);
		}
	}
	if(3 <= cx)
	{
		uint32_t k = ((uint32_t)px[2] << 16) | ((uint32_t)px[1] << 8) | (uint32_t)px[0];
		juce::String v = table[k];
		if(v.isNotEmpty()) return v;
	}
	if(1 <= cx)
	{
		uint32_t k = (uint32_t)px[0];
		juce::String v = table[k];
		if(v.isNotEmpty()) return v;
	}
	return {};
}

static juce::String ResolveKeyname(int sf, int mi)
{
	static const char* KMAJ[] = { "Cb", "Gb", "Db", "Ab", "Eb", "Bb", "F", "C", "G", "D", "A", "E", "B", "F#", "C#" };
	static const char* KMIN[] = { "Ab", "Eb", "Bb", "F", "C", "G", "D", "A", "E", "B", "F#", "C#", "G#", "D#", "A#" };
	const char*const* pk = mi ? KMIN : KMAJ;
	juce::String ssf = ((-7 <= sf) && (sf <= 7)) ? juce::String::fromUTF8(pk[sf + 7]) : juce::String::formatted("%d", sf);
	juce::String smi = mi ? "min" : "maj";
	return ssf + " " + smi;
}

enum ColumnIDs { Deltatime = 1, Status, Channel, Data, Count };

class TrackModel : public juce::TableListBoxModel
{
public:
	MainForm* mOwner;
	int mTrackIndex;
	TrackModel(MainForm* owner, int itr) : mOwner(owner), mTrackIndex(itr)
	{
	}
	virtual int getNumRows() override
	{
		return mOwner->getNumRows(mTrackIndex);
	}
	virtual void paintRowBackground(juce::Graphics& g, int row, int width, int height, bool selected) override
	{
		mOwner->paintRowBackground(mTrackIndex, g, row, width, height, selected);
	}
	virtual void paintCell(juce::Graphics& g, int row, int colid, int width, int height, bool selected) override
	{
		mOwner->paintCell(mTrackIndex, g, row, colid, width, height, selected);
	}
	virtual void backgroundClicked(const juce::MouseEvent& me) override
	{
		mOwner->backgroundClicked(mTrackIndex, me);
	}
};

//[/MiscUserDefs]

//==============================================================================
MainForm::MainForm ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    mFileTitleLabel.reset (new juce::Label (juce::String(),
                                            TRANS("file\n")));
    addAndMakeVisible (mFileTitleLabel.get());
    mFileTitleLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    mFileTitleLabel->setJustificationType (juce::Justification::centredLeft);
    mFileTitleLabel->setEditable (false, false, false);
    mFileTitleLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    mFileTitleLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    mFileTitleLabel->setBounds (8, 8, 64, 24);

    mFilePathLabel.reset (new juce::Label (juce::String(),
                                           juce::String()));
    addAndMakeVisible (mFilePathLabel.get());
    mFilePathLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    mFilePathLabel->setJustificationType (juce::Justification::centredLeft);
    mFilePathLabel->setEditable (false, false, false);
    mFilePathLabel->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    mFilePathLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    mFilePathLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    mBrowseButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (mBrowseButton.get());
    mBrowseButton->setTooltip (TRANS("browse for open"));
    mBrowseButton->setButtonText (TRANS("..."));
    mBrowseButton->setConnectedEdges (juce::Button::ConnectedOnLeft);

    mClearButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (mClearButton.get());
    mClearButton->setTooltip (TRANS("close"));
    mClearButton->setButtonText (TRANS("x"));
    mClearButton->setConnectedEdges (juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);

    mHeaderTitleLabel.reset (new juce::Label (juce::String(),
                                              TRANS("header")));
    addAndMakeVisible (mHeaderTitleLabel.get());
    mHeaderTitleLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    mHeaderTitleLabel->setJustificationType (juce::Justification::centredLeft);
    mHeaderTitleLabel->setEditable (false, false, false);
    mHeaderTitleLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    mHeaderTitleLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    mHeaderTitleLabel->setBounds (8, 40, 56, 24);

    mHeaderValueLabel.reset (new juce::Label (juce::String(),
                                              juce::String()));
    addAndMakeVisible (mHeaderValueLabel.get());
    mHeaderValueLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    mHeaderValueLabel->setJustificationType (juce::Justification::centredLeft);
    mHeaderValueLabel->setEditable (false, false, false);
    mHeaderValueLabel->setColour (juce::Label::outlineColourId, juce::Colour (0xffdddddd));
    mHeaderValueLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    mHeaderValueLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    mSysexLabel.reset (new juce::Label (juce::String(),
                                        TRANS("sysex")));
    addAndMakeVisible (mSysexLabel.get());
    mSysexLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    mSysexLabel->setJustificationType (juce::Justification::centredRight);
    mSysexLabel->setEditable (false, false, false);
    mSysexLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    mSysexLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    mSysexSelPrevButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (mSysexSelPrevButton.get());
    mSysexSelPrevButton->setTooltip (TRANS("select prev sysex"));
    mSysexSelPrevButton->setButtonText (TRANS("<"));

    mSysexSelNextButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (mSysexSelNextButton.get());
    mSysexSelNextButton->setTooltip (TRANS("select next sysex"));
    mSysexSelNextButton->setButtonText (TRANS(">"));

    mSysexSelAllButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (mSysexSelAllButton.get());
    mSysexSelAllButton->setTooltip (TRANS("select all sysex"));
    mSysexSelAllButton->setButtonText (TRANS("all"));

    mSysexExportButton.reset (new juce::TextButton (juce::String()));
    addAndMakeVisible (mSysexExportButton.get());
    mSysexExportButton->setTooltip (TRANS("export all sysex in the file"));
    mSysexExportButton->setButtonText (TRANS("exp"));

    mTracksTabCtrl.reset (new juce::TabbedComponent (juce::TabbedButtonBar::TabsAtTop));
    addAndMakeVisible (mTracksTabCtrl.get());
    mTracksTabCtrl->setTabBarDepth (25);
    mTracksTabCtrl->setCurrentTabIndex (-1);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 800);


    //[Constructor] You can add your own custom stuff here..
	mBrowseButton->onClick = [this]()
	{
		std::shared_ptr<juce::FileChooser> fcdlg = std::make_shared<juce::FileChooser>("open SMF", juce::File(), "*.mid;*.rmi");
		fcdlg->launchAsync(juce::FileBrowserComponent::FileChooserFlags::openMode | juce::FileBrowserComponent::FileChooserFlags::canSelectFiles, [this, fcdlg](const juce::FileChooser& fc) mutable
		{
			juce::File path = fc.getResult();
			if(path == juce::File()) return;
			openContent(path);
			fcdlg.reset();
		});
	};
	mClearButton->onClick = [this]()
	{
		clearContent();
	};
	mSysexSelPrevButton->onClick = [this]()
	{
		int itr = mTracksTabCtrl->getCurrentTabIndex();
		juce::TableListBox* tlb = dynamic_cast<juce::TableListBox*>(mTracksTabCtrl->getTabContentComponent(itr));
		int istart = tlb->getSelectedRow(0);
		std::vector<int> indices = findNextSysexEventIndices(itr, istart, true, false);
		juce::SparseSet<int> ss; for(int i : indices) ss.addRange({ i, i + 1 });
		tlb->setSelectedRows(ss, juce::NotificationType::dontSendNotification);
		if(0 < ss.size()) tlb->scrollToEnsureRowIsOnscreen(ss[0]);
	};
	mSysexSelNextButton->onClick = [this]()
	{
		int itr = mTracksTabCtrl->getCurrentTabIndex();
		juce::TableListBox* tlb = dynamic_cast<juce::TableListBox*>(mTracksTabCtrl->getTabContentComponent(itr));
		int ilast = tlb->getSelectedRow(tlb->getNumSelectedRows() - 1);
		std::vector<int> indices = findNextSysexEventIndices(itr, ilast, true, true);
		juce::SparseSet<int> ss; for(int i : indices) ss.addRange({ i, i + 1 });
		tlb->setSelectedRows(ss, juce::NotificationType::dontSendNotification);
		if(0 < ss.size()) tlb->scrollToEnsureRowIsOnscreen(ss[0]);
	};
	mSysexSelAllButton->onClick = [this]()
	{
		int itr = mTracksTabCtrl->getCurrentTabIndex();
		juce::TableListBox* tlb = dynamic_cast<juce::TableListBox*>(mTracksTabCtrl->getTabContentComponent(itr));
		juce::SparseSet<int> ss;
		int ievsearch = -1;
		for(;;)
		{
			std::vector<int> indices = findNextSysexEventIndices(itr, ievsearch, false, true);
			if(indices.empty()) break;
			for(int i : indices) ss.addRange({ i, i + 1 });
			ievsearch = indices.back();
		}
		tlb->setSelectedRows(ss, juce::NotificationType::dontSendNotification);
		if(0 < ss.size()) tlb->scrollToEnsureRowIsOnscreen(ss[0]);
	};
	mSysexExportButton->onClick = [this]()
	{
		std::shared_ptr<juce::FileChooser> fcdlg = std::make_shared<juce::FileChooser>("sysex save: choose folder");
		fcdlg->launchAsync(juce::FileBrowserComponent::FileChooserFlags::openMode | juce::FileBrowserComponent::FileChooserFlags::canSelectDirectories, [this, fcdlg](const juce::FileChooser& fc) mutable
		{
			juce::File dir = fc.getResult();
			if((dir == juce::File()) || !dir.isDirectory()) return;
			const std::vector<SMFTrack> tracklist = mSMFParser->tracks();
			const std::vector<SMFChunk>& chunklist = mSMFParser->chunks();
			juce::String fnbase = mSmfPath.getFileNameWithoutExtension();
			for(int ctr = (int)tracklist.size(), itr = 0; itr < ctr; ++itr)
			{
				const SMFTrack& track = tracklist[itr];
				int ievsearch = -1;
				for(;;)
				{
					std::vector<int> indices = findNextSysexEventIndices(itr, ievsearch, false, true);
					if(indices.empty()) break;
					juce::FileOutputStream str(dir.getChildFile(fnbase + juce::String::formatted("#%d-%d.syx", itr, indices.front())));
					if(str.openedOk())
					{
						str.setPosition(0);
						str.truncate();
						for(auto iev : indices)
						{
							const SMFEvent& ev = track[iev];
							int ick = ev.ckref.index;
							if(chunklist.size() <= (size_t)ick) break;
							const SMFChunk& ck = chunklist[ick];
							str.write(ck.data(), ck.size());
						}
					}
					ievsearch = indices.back();
				}
			}
			fcdlg.reset();
		});
	};
	mSMFParser.reset(SMFParser::createInstance());
    //[/Constructor]
}

MainForm::~MainForm()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    mFileTitleLabel = nullptr;
    mFilePathLabel = nullptr;
    mBrowseButton = nullptr;
    mClearButton = nullptr;
    mHeaderTitleLabel = nullptr;
    mHeaderValueLabel = nullptr;
    mSysexLabel = nullptr;
    mSysexSelPrevButton = nullptr;
    mSysexSelNextButton = nullptr;
    mSysexSelAllButton = nullptr;
    mSysexExportButton = nullptr;
    mTracksTabCtrl = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
	mSMFParser = nullptr;
    //[/Destructor]
}

//==============================================================================
void MainForm::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xffefefef));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MainForm::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    mFilePathLabel->setBounds (72, 8, getWidth() - 144, 24);
    mBrowseButton->setBounds (getWidth() - 8 - 32, 8, 32, 24);
    mClearButton->setBounds (getWidth() - 40 - 32, 8, 32, 24);
    mHeaderValueLabel->setBounds (72, 40, getWidth() - 256, 24);
    mSysexLabel->setBounds (getWidth() - 136 - 48, 40, 48, 24);
    mSysexSelPrevButton->setBounds (getWidth() - 104 - 32, 40, 32, 24);
    mSysexSelNextButton->setBounds (getWidth() - 72 - 32, 40, 32, 24);
    mSysexSelAllButton->setBounds (getWidth() - 40 - 32, 40, 32, 24);
    mSysexExportButton->setBounds (getWidth() - 8 - 32, 40, 32, 24);
    mTracksTabCtrl->setBounds (8, 72, getWidth() - 16, getHeight() - 80);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

// --------------------------------------------------------------------------------
// internals

void MainForm::clearContent()
{
	mFilePathLabel->setText("", juce::NotificationType::dontSendNotification);
	mHeaderValueLabel->setText("", juce::NotificationType::dontSendNotification);
	mTracksTabCtrl->clearTabs();
	mTrackModels.clear();
	mSMFParser->clear();
	mSmfPath = juce::File();
}

void MainForm::openContent(const juce::File& path)
{
	clearContent();
	std::string err;
	if(mSMFParser->load(path.getFullPathName().toWideCharPointer(), &err))
	{
		mSmfPath = path;
		mFilePathLabel->setText(path.getFullPathName(), juce::NotificationType::dontSendNotification);
		juce::String hdr;
		hdr += juce::String::formatted("format=%d, ", mSMFParser->format());
		hdr += juce::String::formatted("numtracks=%u, ", (unsigned int)mSMFParser->tracks().size());
		SMFDivision division = mSMFParser->division();
		if(division.type == SMFDivision::metrical)	hdr += juce::String::formatted("division=%d units/qnote", division.qnotediv);
		else										hdr += juce::String::formatted("division={%d frames/sec, %d units/frame}", -division.timeres.fps, division.timeres.framediv);
		mHeaderValueLabel->setText(hdr, juce::NotificationType::dontSendNotification);
		for(int ctr = (int)mSMFParser->tracks().size(), itr = 0; itr < ctr; ++itr)
		{
			juce::TableListBox* tablectrl = new juce::TableListBox();
			TrackModel* trmodel = new TrackModel(this, itr);
			mTrackModels.add(trmodel);
			tablectrl->setModel(trmodel);
			tablectrl->setMultipleSelectionEnabled(true);
			tablectrl->setHeaderHeight(25);
			tablectrl->setRowHeight(20);
			tablectrl->getHeader().addColumn("deltatime", ColumnIDs::Deltatime, 50, 30, -1, juce::TableHeaderComponent::notSortable);
			tablectrl->getHeader().addColumn("status", ColumnIDs::Status, 100, 30, -1, juce::TableHeaderComponent::notSortable);
			tablectrl->getHeader().addColumn("channel", ColumnIDs::Channel, 50, 30, -1, juce::TableHeaderComponent::notSortable);
			tablectrl->getHeader().addColumn("data", ColumnIDs::Data, 300, 30, -1, juce::TableHeaderComponent::notSortable);
			mTracksTabCtrl->addTab(juce::String::formatted("%d", mTracksTabCtrl->getNumTabs()), juce::Colours::white, tablectrl, true);
		}
		if(0 < mTracksTabCtrl->getNumTabs()) mTracksTabCtrl->setCurrentTabIndex(0, false);
	}
	else
	{
		juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "ERROR", juce::String(err));
	}
}

int MainForm::findNextSysexEventIndex(int itr, int iev, bool wrap, bool forward) const
{
	if(mSMFParser->tracks().size() <= (size_t)itr) return -1;
	const SMFTrack& track = mSMFParser->tracks()[itr];
	if(track.empty()) return -1;
	int cev = (int)track.size();
	if(forward)
	{
		int i = juce::jlimit(0, cev - 1, iev + 1), istart = i;
		for(;;)
		{
			if(track[i].status == 0xf0) return i;
			++i;
			if(cev <= i) { if(wrap) i = 0; else break; }
			if(i == istart) break;
		}
	}
	else
	{
		int i = juce::jlimit(0, cev - 1, iev - 1), istart = i;
		for(;;)
		{
			if(track[i].status == 0xf0) return i;
			--i;
			if(i < 0) { if(wrap) i = cev - 1; else break; }
			if(i == istart) break;
		}
	}
	return -1;
}

std::vector<int> MainForm::findNextSysexEventIndices(int itr, int iev, bool wrap, bool forward) const
{
	iev = findNextSysexEventIndex(itr, iev, wrap, forward);
	if(iev < 0) return {};
	const std::vector<SMFChunk>& chunklist = mSMFParser->chunks();
	const SMFTrack& track = mSMFParser->tracks()[itr];
	std::vector<int> indices;
	for(int cev = (int)track.size(); iev < cev; ++iev)
	{
		if((track[iev].status != 0xf0) && (track[iev].status != 0xf7)) continue;
		indices.push_back(iev);
		int ick = track[iev].ckref.index;
		if(chunklist.size() <= (size_t)ick) break;
		const SMFChunk& ck = chunklist[ick];
		if(ck.back() == 0xf7) break;
	}
	return indices;
}

// --------------------------------------------------------------------------------
// juce::TableListBoxModel reflections

int MainForm::getNumRows(int itr)
{
	const std::vector<SMFTrack>& tracks = mSMFParser->tracks();
	if(tracks.size() <= (size_t)itr) return 0;
	return (int)mSMFParser->tracks()[itr].size();
}

void MainForm::paintRowBackground(int, juce::Graphics& g, int, int width, int height, bool selected)
{
	g.setColour(juce::Colour(selected ? 0xffc0c0c0 : 0xffffffff));
	g.fillRect(0, 0, width, height);
}

void MainForm::paintCell(int itr, juce::Graphics& g, int row, int colid, int width, int height, bool)
{
	const std::vector<SMFTrack>& tracklist = mSMFParser->tracks();
	if(tracklist.size() <= itr) return;
	const SMFTrack& track = tracklist[itr];
	if(track.size() <= (size_t)row) return;
	const SMFEvent& ev = track[row];
	juce::String text;
	juce::Justification align = juce::Justification::centredLeft;
	juce::Colour textcolour = juce::Colours::black;
	switch(colid)
	{
		case ColumnIDs::Deltatime:
			align = juce::Justification::centredRight;
			text = juce::String::formatted("%u", ev.deltatime);
			break;
		case ColumnIDs::Status:
			if(ev.status < 0xf0)
			{
				switch(ev.status & 0xf0)
				{
					case 0x80: text = "note off"; break;
					case 0x90: text = "note on"; break;
					case 0xa0: text = "key aftertouch"; break;
					case 0xb0: text = "control change"; break;
					case 0xc0: text = "program change"; break;
					case 0xd0: text = "channel aftertouch"; break;
					case 0xe0: text = "pitch"; break;
					default: text = juce::String::formatted("invalid (%02x)", ev.status); break;
				}
			}
			else
			{
				switch(ev.status)
				{
					case 0xf0: text = "system exclusive"; break;
					case 0xf7: text = "system exclusive (partial)"; break;
					case 0xff: text = "metadata"; break;
					default: text = juce::String::formatted("invalid (%02x)", ev.status); break;
				}
			}
			break;
		case ColumnIDs::Channel:
			if(ev.status < 0xf0)
			{
				align = juce::Justification::centredRight;
				text = juce::String::formatted("%u", ev.status & 0x0f);
			}
			break;
		case ColumnIDs::Data:
			if(ev.status < 0xf0)
			{
				switch(ev.status & 0xf0)
				{
					case 0x80: text = juce::String::formatted("k=%u v=%u", ev.chdata[0], ev.chdata[1]); break;
					case 0x90: text = juce::String::formatted("k=%u v=%u", ev.chdata[0], ev.chdata[1]); break;
					case 0xa0: text = juce::String::formatted("k=%u v=%u", ev.chdata[0], ev.chdata[1]); break;
					case 0xb0: text = juce::String::formatted("%hs (%u) v=%u", ResolveCCName(ev.chdata[0]).toRawUTF8(), ev.chdata[0], ev.chdata[1]); break;
					case 0xc0: text = juce::String::formatted("v=%u", ev.chdata[0]); break;
					case 0xd0: text = juce::String::formatted("v=%u", ev.chdata[0]); break;
					case 0xe0: text = juce::String::formatted("v=%d", (int16_t)(((uint16_t)ev.chdata[1] << 7) | (uint16_t)ev.chdata[0]) - 8192); break;
				}
			}
			else
			{
				switch(ev.status)
				{
					case 0xf0:
					{
						const std::vector<SMFChunk>& chunklist = mSMFParser->chunks();
						if(chunklist.size() <= (size_t)ev.ckref.index) return;
						const SMFChunk& ckdata = chunklist[ev.ckref.index];
						if(1 < ckdata.size()) text = "\"" + ResolveMFIDName(ckdata.data() + 1, (int)ckdata.size() - 1) + "\" ";
						text += juce::String::formatted("length=%u: ", (unsigned int)ckdata.size());
						text += juce::String::toHexString(ckdata.data(), std::min(256, (int)ckdata.size()), 1);
						break;
					}
					case 0xf7:
					{
						const std::vector<SMFChunk>& chunklist = mSMFParser->chunks();
						if(chunklist.size() <= (size_t)ev.ckref.index) return;
						const SMFChunk& ckdata = chunklist[ev.ckref.index];
						text += juce::String::formatted("length=%u: ", (unsigned int)ckdata.size());
						text += juce::String::toHexString(ckdata.data(), std::min(256, (int)ckdata.size()), 1);
						break;
					}
					case 0xff:
					{
						const std::vector<SMFChunk>& chunklist = mSMFParser->chunks();
						if(chunklist.size() <= (size_t)ev.ckref.index) return;
						const SMFChunk& ckdata = chunklist[ev.ckref.index];
						const uint8_t* pck = (const uint8_t*)ckdata.data();
						int cck = (int)ckdata.size();
						switch(ev.ckref.type)
						{
							case 0x00:
								text = "sequence number: ";
								if(cck == 2) text += juce::String::formatted("%u", ((uint16_t)pck[0] << 8) | (uint16_t)pck[1]);
								else text += juce::String::formatted("invalid length (%d)", cck);
								break;
							case 0x01: text = "text: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x02: text = "copyright: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x03: text = "track name: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x04: text = "instrument name: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x05: text = "lyric: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x06: text = "marker: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x07: text = "cue point: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x08: text = "program: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x09: text = "device: " + juce::String(std::string((const char*)pck, cck)); break;
							case 0x20:
								text = "channel prefix: ";
								if(cck == 1) text += juce::String::formatted("%u", pck[0]);
								else text += juce::String::formatted("invalid length (%d)", cck);
								break;
							case 0x21:
								text = "port: ";
								if(cck == 1) text += juce::String::formatted("%u", pck[0]);
								else text += juce::String::formatted("invalid length (%d)", cck);
								break;
							case 0x2f: text = "end of track"; break;
							case 0x51:
							{
								text = "tempo: ";
								if(cck == 3)
								{
									uint32_t tttttt = ((uint32_t)pck[0] << 16) | ((uint32_t)pck[1] << 8) | (uint32_t)pck[2];
									text += juce::String::formatted("bpm=%.2f (%u us/qn)", 60.0 * 1000000.0 / (double)tttttt, tttttt);
								}
								else text += juce::String::formatted("invalid length (%d)", cck);
								break;
							}
							case 0x54:
							{
								text = "smpte offset: ";
								if(cck == 5)
								{
									uint8_t hr = pck[0], mn = pck[1], se = pck[2], fr = pck[3], ff = pck[4];
									text += juce::String::formatted("%02u:%02u:%02u.%02u:%02u", hr, mn, se, fr, ff);
								}
								else text += juce::String::formatted("invalid length (%d)", cck);
								break;
							}
							case 0x58:
							{
								text = "time signature: ";
								if(cck == 4)
								{
									uint8_t nn = pck[0], dd = pck[1], cc = pck[2], bb = pck[3];
									text += juce::String::formatted("%u/%u, %u clk/tick, %u qn/32n", nn, 1 << dd, cc, bb);
								}
								else text += juce::String::formatted("invalid length (%d)", cck);
								break;
							}
							case 0x59:
							{
								text = "key signature: ";
								if(cck == 2)
								{
									int8_t sf = pck[0], mi = pck[1];
									text += juce::String::formatted("%hs (%d %hs)", ResolveKeyname(sf, mi).toRawUTF8(), sf, mi ? "min" : "maj");
								}
								else text += juce::String::formatted("invalid length (%d)", cck);
								break;
							}
							default:
							{
								text += juce::String::formatted("type=0x%02x length=%u: ", ev.ckref.type, (int)ckdata.size());
								text += juce::String::toHexString(ckdata.data(), std::min(256, (int)ckdata.size()), 1);
								break;
							}
						}
						break;
					}
					default:
						text += "invalid data";
						textcolour = juce::Colours::red;
						break;
				}
			}
			break;
		default:
			break; // unexpected
	}
	g.setColour(textcolour);
	g.drawText(text, 4, 0, width - 8, height, align, true);
	g.setColour(juce::Colour(0xffdddddd));
	g.drawHorizontalLine(height - 1, 0, (float)width);
	g.drawVerticalLine(width - 1, 0, (float)height);
}

void MainForm::backgroundClicked(int itr, const juce::MouseEvent&)
{
	juce::TableListBox* tlb = dynamic_cast<juce::TableListBox*>(mTracksTabCtrl->getTabContentComponent(itr));
	tlb->deselectAllRows();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainForm" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="800">
  <BACKGROUND backgroundColour="ffefefef"/>
  <LABEL name="" id="52ec90cc3fc42fbd" memberName="mFileTitleLabel" virtualName=""
         explicitFocusOrder="0" pos="8 8 64 24" edTextCol="ff000000" edBkgCol="0"
         labelText="file&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="2c9a4d2e164d338" memberName="mFilePathLabel" virtualName=""
         explicitFocusOrder="0" pos="72 8 144M 24" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="" id="e1c9526f45f3ce02" memberName="mBrowseButton" virtualName=""
              explicitFocusOrder="0" pos="8Rr 8 32 24" tooltip="browse for open"
              buttonText="..." connectedEdges="1" needsCallback="0" radioGroupId="0"/>
  <TEXTBUTTON name="" id="4f4fd7a1e2940019" memberName="mClearButton" virtualName=""
              explicitFocusOrder="0" pos="40Rr 8 32 24" tooltip="close" buttonText="x"
              connectedEdges="3" needsCallback="0" radioGroupId="0"/>
  <LABEL name="" id="4c18ec22b35da66d" memberName="mHeaderTitleLabel"
         virtualName="" explicitFocusOrder="0" pos="8 40 56 24" edTextCol="ff000000"
         edBkgCol="0" labelText="header" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="5745bca684c16ed9" memberName="mHeaderValueLabel"
         virtualName="" explicitFocusOrder="0" pos="72 40 256M 24" outlineCol="ffdddddd"
         edTextCol="ff000000" edBkgCol="0" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="df49b6d9e6e2eb5a" memberName="mSysexLabel" virtualName=""
         explicitFocusOrder="0" pos="136Rr 40 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="sysex" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <TEXTBUTTON name="" id="6d08d9ac4987b198" memberName="mSysexSelPrevButton"
              virtualName="" explicitFocusOrder="0" pos="104Rr 40 32 24" tooltip="select prev sysex"
              buttonText="&lt;" connectedEdges="0" needsCallback="0" radioGroupId="0"/>
  <TEXTBUTTON name="" id="18fa02020e2ac36e" memberName="mSysexSelNextButton"
              virtualName="" explicitFocusOrder="0" pos="72Rr 40 32 24" tooltip="select next sysex"
              buttonText="&gt;" connectedEdges="0" needsCallback="0" radioGroupId="0"/>
  <TEXTBUTTON name="" id="35ca86ae5dccaa77" memberName="mSysexSelAllButton"
              virtualName="" explicitFocusOrder="0" pos="40Rr 40 32 24" tooltip="select all sysex"
              buttonText="all" connectedEdges="0" needsCallback="0" radioGroupId="0"/>
  <TEXTBUTTON name="" id="54ccde4bb8106069" memberName="mSysexExportButton"
              virtualName="" explicitFocusOrder="0" pos="8Rr 40 32 24" tooltip="export all sysex in the file"
              buttonText="exp" connectedEdges="0" needsCallback="0" radioGroupId="0"/>
  <TABBEDCOMPONENT name="" id="98986f40488f8861" memberName="mTracksTabCtrl" virtualName=""
                   explicitFocusOrder="0" pos="8 72 16M 80M" orientation="top" tabBarDepth="25"
                   initialTab="-1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

