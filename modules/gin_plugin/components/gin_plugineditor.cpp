#include <time.h>

//==============================================================================
UpdateChecker::UpdateChecker (gin::Processor& slProc_)
  : Thread ("Update"), slProc (slProc_)
{
    if (auto props = slProc.getSettings())
    {
       #ifdef JucePlugin_Name
        juce::String url = props->getValue (JucePlugin_Name "_updateUrl");
        int last   = props->getIntValue (JucePlugin_Name "_lastUpdateCheck");

        if (url.isNotEmpty())
        {
            updateUrl = url;
            triggerAsyncUpdate();
        }
        else if (time (nullptr) > last + 86400)
        {
            startTimer (juce::Random::getSystemRandom().nextInt ({1500, 2500}));
        }
       #endif
    }
}

UpdateChecker::~UpdateChecker()
{
    while (isThreadRunning())
        Thread::sleep (10);
}

void UpdateChecker::timerCallback()
{
    stopTimer();
    startThread();
}

void UpdateChecker::run()
{
  #if ! JUCE_IOS
   #ifdef JucePlugin_Name
    juce::URL versionsUrl = juce::URL ("https://socalabs.com/version.xml").withParameter ("plugin", JucePlugin_Name).withParameter ("version", JucePlugin_VersionString);
    juce::XmlDocument doc (versionsUrl.readEntireTextStream());
    if (std::unique_ptr<juce::XmlElement> root = doc.getDocumentElement())
    {
        if (auto props = slProc.getSettings())
        {
            props->setValue (JucePlugin_Name "_lastUpdateCheck", int (time (nullptr)));

            auto* child = root->getChildElement (0);
            while (child)
            {
                juce::String name = child->getStringAttribute ("name");
                juce::String ver  = child->getStringAttribute ("num");
                juce::String url  = child->getStringAttribute ("url");

                if (name == JucePlugin_Name && versionStringToInt (ver) > versionStringToInt (JucePlugin_VersionString))
                {
                    props->setValue (JucePlugin_Name "_updateUrl", url);
                    updateUrl = url;
                    triggerAsyncUpdate();
                    break;
                }

                child = child->getNextElement();
            }
        }
    }
   #endif
  #endif
}

void UpdateChecker::handleAsyncUpdate()
{
    onUpdate (updateUrl);
}

//==============================================================================
NewsChecker::NewsChecker (Processor& slProc_)
    : Thread ("News"), slProc (slProc_)
{
    if (auto props = slProc.getSettings())
    {
        juce::String url = props->getValue ("newsUrl");
        int last   = props->getIntValue ("lastNewsCheck");

        if (url.isNotEmpty())
        {
            newsUrl = url;
            triggerAsyncUpdate();
        }
        else if (time (nullptr) > last + 86400)
        {
            startTimer (juce::Random::getSystemRandom().nextInt ({1500, 2500}));
        }
    }
}

NewsChecker::~NewsChecker()
{
    while (isThreadRunning())
        Thread::sleep (10);
}

void NewsChecker::timerCallback()
{
    stopTimer();
    startThread();
}

void NewsChecker::run()
{
    #if ! JUCE_IOS
    juce::XmlDocument doc (juce::URL ("https://socalabs.com/feed/").readEntireTextStream());
    if (std::unique_ptr<juce::XmlElement> root = doc.getDocumentElement())
    {
        if (auto props = slProc.getSettings())
        {
            if (auto rss = root->getChildByName ("channel"))
            {
                if (auto item = rss->getChildByName ("item"))
                {
                    if (auto link = item->getChildByName ("link"))
                    {
                        props->setValue ("lastNewsCheck", int (time (nullptr)));

                        juce::String url = link->getAllSubText();

                        juce::StringArray readNews = juce::StringArray::fromTokens (props->getValue ("readNews"), "|", "");
                        if (readNews.isEmpty())
                        {
                            readNews.add (url);
                            props->setValue ("readNews", readNews.joinIntoString ("|"));
                        }

                        if (! readNews.contains (url))
                        {
                            props->setValue ("newsUrl", url);

                            newsUrl = url;
                            triggerAsyncUpdate();
                        }
                    }
                }
            }
        }
    }
   #endif
}

void NewsChecker::handleAsyncUpdate()
{
    onNewsUpdate (newsUrl);
}

//==============================================================================
TitleBar::TitleBar (ProcessorEditor& e, Processor& p)
  : editor (e), slProc (p)
{
    addAndMakeVisible (menuButton);
    addAndMakeVisible (browseButton);
    addAndMakeVisible (programs);
    addAndMakeVisible (nextButton);
    addAndMakeVisible (prevButton);
    addAndMakeVisible (addButton);
    addAndMakeVisible (deleteButton);
    addAndMakeVisible (infoButton);

    programs.addListener (this);
    addButton.addListener (this);
    deleteButton.addListener (this);
    nextButton.addListener (this);
    prevButton.addListener (this);
    browseButton.addListener (this);
    infoButton.addListener (this);
    menuButton.addListener (this);

    programs.setTitle ("Select Preset");
    addButton.setTitle ("Add Preset");
    deleteButton.setTitle ("Delete Preset");
    browseButton.setTitle ("Browse Preset");
    nextButton.setTitle ("Next Preset");
    prevButton.setTitle ("Prev Preset");
    infoButton.setTitle ("Info");
    menuButton.setTitle ("Menu");

    programs.setTooltip ("Select Preset");
    addButton.setTooltip ("Add Preset");
    deleteButton.setTooltip ("Delete Preset");
    browseButton.setTooltip ("Browse Preset");
    nextButton.setTooltip ("Next Preset");
    prevButton.setTooltip ("Prev Preset");
    infoButton.setTooltip ("Info");
    menuButton.setTooltip ("Menu");

    slProc.addChangeListener (this);

    updateChecker = std::make_unique<UpdateChecker> (slProc);
    updateChecker->onUpdate = [](juce::String) {};

    newsChecker = std::make_unique<NewsChecker> (slProc);
    newsChecker->onNewsUpdate = [](juce::String) {};
}

TitleBar::~TitleBar ()
{
    slProc.removeChangeListener (this);
}

void TitleBar::parentHierarchyChanged()
{
    auto a = wantsAccessibleKeyboard (*this);
    programs.setWantsKeyboardFocus (a);
    addButton.setWantsKeyboardFocus (a);
    deleteButton.setWantsKeyboardFocus (a);
    browseButton.setWantsKeyboardFocus (a);
    nextButton.setWantsKeyboardFocus (a);
    prevButton.setWantsKeyboardFocus (a);
    infoButton.setWantsKeyboardFocus (a);
    menuButton.setWantsKeyboardFocus (a);
}

void TitleBar::paint (juce::Graphics& g)
{
    juce::ColourGradient grad (findColour (PluginLookAndFeel::matte1ColourId), 0, 0,
                               findColour (PluginLookAndFeel::matte2ColourId), 0, float (getHeight()), false);

    g.setGradientFill (grad);
    g.fillAll();
}

void TitleBar::setShowBrowser (bool s)
{
    hasBrowser = s;
    resized ();
}

void TitleBar::setShowPresets (bool s)
{
    hasPresets = s;
    resized();
}

void TitleBar::resized()
{
    auto programsRC = getLocalBounds().withSizeKeepingCentre (std::min (getWidth() - 200, 299), 23);

    if (hasPresets)
    {
        programs.setBounds (programsRC);

        int x = programsRC.getRight() + 10;
        addButton.setBounds (x, 10, 19, 19);
        x += 19 + 10;
        deleteButton.setBounds (x, 10, 19, 19);
    }
    else
    {
        programs.setBounds ({});
        addButton.setBounds ({});
        deleteButton.setBounds ({});
    }

    if (hasPresets && hasBrowser)
        browseButton.setBounds (programsRC.getX() - 19 - 10, 10, 19, 19);
    else
        browseButton.setBounds ({});

    if (hasPresets)
    {
        prevButton.setBounds (programsRC.removeFromLeft (programsRC.getHeight()).withSizeKeepingCentre (8, 8));
        nextButton.setBounds (programsRC.removeFromRight (programsRC.getHeight()).withSizeKeepingCentre (8, 8));
    }
    else
    {
        prevButton.setBounds ({});
        nextButton.setBounds ({});
    }

    menuButton.setBounds (11, 11, 17, 17);
    infoButton.setBounds (getWidth() - 19 - 10, 10, 19, 19);
}

void TitleBar::refreshPrograms()
{
    programs.clear();

    for (int i = 0; i < slProc.getNumPrograms(); i++)
        programs.addItem (slProc.getProgramName (i), i + 1);

    programs.setSelectedItemIndex (slProc.getCurrentProgram(), juce::dontSendNotification);
    deleteButton.setEnabled (slProc.getCurrentProgram() != 0);

    editor.refreshPatchBrowser();
}

void TitleBar::buttonClicked (juce::Button* b)
{
    if (b == &nextButton)
    {
        int prog = slProc.getCurrentProgram() + 1;
        if (prog >= slProc.getNumPrograms())
            prog = 0;

        slProc.setCurrentProgram (prog);
    }
    else if (b == &prevButton)
    {
        int prog = slProc.getCurrentProgram() - 1;
        if (prog < 0)
            prog = slProc.getNumPrograms() - 1;

        slProc.setCurrentProgram (prog);
    }
    else if (b == &browseButton)
    {
        b->setToggleState (! b->getToggleState(), juce::dontSendNotification);
        editor.showPatchBrowser (b->getToggleState());
    }
    else if (b == &addButton)
    {
        gin::PluginAlertWindow w ("Create preset:", "", juce::AlertWindow::NoIcon, getParentComponent());
        w.setLookAndFeel (slProc.lf.get());
        w.addTextEditor ("name", "", "Name:");

        if (hasBrowser)
        {
            w.addTextEditor ("author", "", "Author:");
            w.addTextEditor ("tags", "", "Tags:");
        }

        w.addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
        w.addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));

        if (w.runModalLoop (*getParentComponent()) == 1)
        {
            auto txt = juce::File::createLegalFileName (w.getTextEditor ("name")->getText());
            auto aut = (hasBrowser) ? juce::File::createLegalFileName (w.getTextEditor ("author")->getText()) : juce::String();
            auto tag = (hasBrowser) ? juce::File::createLegalFileName (w.getTextEditor ("tags")->getText()) : juce::String();

            if (slProc.hasProgram (txt))
            {
                gin::PluginAlertWindow wc ("Overwrite preset '" + txt + "'?", "", juce::AlertWindow::NoIcon, this);
                wc.addButton ("Yes", 1, juce::KeyPress (juce::KeyPress::returnKey));
                wc.addButton ("No", 0, juce::KeyPress (juce::KeyPress::escapeKey));
                wc.setLookAndFeel (slProc.lf.get());

                if (wc.runModalLoop (*this) == 0)
                    return;
            }

            if (txt.isNotEmpty())
            {
                slProc.saveProgram (txt, aut, tag);
                refreshPrograms();
            }
        }
    }
    else if (b == &deleteButton)
    {
        gin::PluginAlertWindow w ("Delete preset '" + slProc.getProgramName (programs.getSelectedItemIndex()) + "'?", "", juce::AlertWindow::NoIcon, getParentComponent());
        w.addButton ("Yes", 1, juce::KeyPress (juce::KeyPress::returnKey));
        w.addButton ("No", 0, juce::KeyPress (juce::KeyPress::escapeKey));
        w.setLookAndFeel (slProc.lf.get());

        if (w.runModalLoop (*getParentComponent()))
        {
            slProc.deleteProgram (programs.getSelectedItemIndex());
            refreshPrograms();
        }
    }
    else if (b == &infoButton)
    {
        editor.showAboutInfo();
    }
    else if (b == &menuButton)
    {
        showMenu();
    }
}

void TitleBar::showMenu()
{
    juce::PopupMenu m;

    m.addItem ("Visit www.SocaLabs.com", []
    {
        juce::URL ("https://www.socalabs.com").launchInDefaultBrowser();
    });

    m.addSeparator();

    auto updateUrl = updateChecker->getUpdateUrl();
    m.addItem ("Get update", updateUrl.isNotEmpty(), false, [this, updateUrl]
    {
        juce::URL (updateUrl).launchInDefaultBrowser();

       #ifdef JucePlugin_Name
        if (auto props = slProc.getSettings())
            props->setValue (JucePlugin_Name "_updateUrl", "");
       #endif
    });

    auto newsUrl = newsChecker->getNewsUrl();
    m.addItem ("Read news", newsUrl.isNotEmpty(), false, [this, newsUrl]
    {
        juce::URL (newsUrl).launchInDefaultBrowser();

        if (auto props = slProc.getSettings())
        {
            props->setValue ("newsUrl", "");

            auto readNews = juce::StringArray::fromTokens (props->getValue ("readNews"), "|", "");
            readNews.add (newsUrl);
            props->setValue ("readNews", readNews.joinIntoString ("|"));
        }
    });

    m.addSeparator();

    m.addItem ("Accessible Keyboard", true, editor.getUseIncreasedKeyboardAccessibility(), [this]
    {
        editor.setUseIncreasedKeyboardAccessibility (! editor.getUseIncreasedKeyboardAccessibility());
    });

    m.setLookAndFeel ( &getLookAndFeel());
    m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (menuButton).withDeletionCheck (menuButton));
}

void TitleBar::comboBoxChanged (juce::ComboBox* c)
{
    if (c == &programs)
    {
        int idx = programs.getSelectedItemIndex();
        deleteButton.setEnabled (idx != 0);
        slProc.setCurrentProgram (idx);
    }
}

void TitleBar::changeListenerCallback (juce::ChangeBroadcaster*)
{
    refreshPrograms();
}

//==============================================================================
juce::Rectangle<int> ProcessorEditorBase::getFullGridArea()
{
    return juce::Rectangle<int> (inset, headerHeight + inset, cx * cols + extraWidthPx, cy * rows + extraHeightPx);
}

juce::Rectangle<int> ProcessorEditorBase::getGridArea (int x, int y, int w, int h)
{
    return juce::Rectangle<int> (inset + x * cx, headerHeight + y * cy + inset, w * cx, h * cy);
}

ParamComponent* ProcessorEditorBase::componentForId (const juce::String& uid)
{
    for (auto* c : controls)
    {
        if (c->getUid() == uid)
            return c;
    }
    return nullptr;
}

ParamComponent* ProcessorEditorBase::componentForParam (Parameter& param)
{
    auto uid = param.getUid();
    for (auto c : controls)
    {
        if (c->getUid() == uid)
            return c;
    }
    return nullptr;
}

void ProcessorEditorBase::setGridSize (int x, int y, int extraWidthPx_, int extraHeightPx_)
{
    cols = x;
    rows = y;
    extraWidthPx  = extraWidthPx_;
    extraHeightPx = extraHeightPx_;

    setSize (x * cx + inset * 2 + extraWidthPx,
             y * cy + inset * 2 + headerHeight + extraHeightPx);
}

juce::Rectangle<int> ProcessorEditorBase::getControlsArea()
{
    return getLocalBounds();
}

bool ProcessorEditorBase::getUseIncreasedKeyboardAccessibility()
{
    if (auto props = ginProcessor.getSettings())
        return props->getBoolValue ("useIncreasedKeyboardAccessibility", false);
    return false;
}

void ProcessorEditorBase::setUseIncreasedKeyboardAccessibility (bool accessible)
{
    if (auto props = ginProcessor.getSettings())
        props->setValue ("useIncreasedKeyboardAccessibility", accessible);

    std::function<void (juce::Component&)> update = [&] (juce::Component& c)
    {
        c.parentHierarchyChanged();
        for (auto child : c.getChildren())
            update (*child);
    };

    update (*this);

    repaint();
}

bool wantsAccessibleKeyboard (juce::Component& c)
{
    if (auto p = c.findParentComponentOfClass<ProcessorEditorBase>())
        return p->getUseIncreasedKeyboardAccessibility();

    return false;
}

//==============================================================================
ProcessorEditor::ProcessorEditor (Processor& p) noexcept
  : ProcessorEditorBase (p, 56, 70), slProc (p)
{
    setLookAndFeel (slProc.lf.get());

    tooltipWindow.setMillisecondsBeforeTipAppears (2000);

    addAndMakeVisible (titleBar);
    addChildComponent (patchBrowser);

    titleBar.refreshPrograms();
}

ProcessorEditor::~ProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void ProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient grad (findColour (PluginLookAndFeel::matte1ColourId), 0, 0,
                               findColour (PluginLookAndFeel::matte2ColourId), 0, float (getHeight()), false);

    g.setGradientFill (grad);
    g.fillAll();

    auto rc = getLocalBounds();

    g.setColour (findColour (PluginLookAndFeel::blackColourId));
    g.fillRect (rc.removeFromTop (1));

    rc.removeFromTop (headerHeight - 1);

    g.setColour (findColour (PluginLookAndFeel::blackColourId));
    g.fillRect (rc.removeFromTop (1));

    g.setColour (findColour (PluginLookAndFeel::grey30ColourId));
    g.fillRect (rc.removeFromTop (1));
}

void ProcessorEditor::resized()
{
    ProcessorEditorBase::resized();

    auto rc = getLocalBounds ().reduced (1);

    titleBar.setBounds (rc.removeFromTop (headerHeight - 1));

    patchBrowser.setBounds (getFullGridArea());
}

void ProcessorEditor::showAboutInfo()
{
juce::String msg;

  #ifdef JucePlugin_Name
   #if JUCE_DEBUG
    msg += JucePlugin_Name " v" JucePlugin_VersionString " (" __TIME__ " " __DATE__ ")\n\n";
   #else
    msg += JucePlugin_Name " v" JucePlugin_VersionString " (" __DATE__ ")\n\n";
   #endif
  #endif
    msg += "Roland Rabien\nDavid Rowland\nRAW Material Software JUCE Framework\n";
    if (additionalProgramming.isNotEmpty())
        msg += additionalProgramming;
    msg += "\n";
    msg += "Copyright ";
    msg += juce::String (&__DATE__[7]);

    gin::PluginAlertWindow w ("---- About ----", msg, juce::AlertWindow::NoIcon, this);
    w.addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
    w.setLookAndFeel (slProc.lf.get());

    w.runModalLoop (*this);
}

void ProcessorEditor::refreshPatchBrowser()
{
    patchBrowser.refresh();
}

void ProcessorEditor::showPatchBrowser (bool p)
{
    patchBrowser.toFront (false);
    patchBrowser.setVisible (p);
}
