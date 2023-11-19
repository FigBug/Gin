
//==============================================================================
UpdateChecker::UpdateChecker (gin::Processor& slProc_)
  : Thread ("Update"), slProc (slProc_)
{
    if (auto props = slProc.getSettings())
    {
        juce::String url = props->getValue (slProc.processorOptions.pluginName + "_updateUrl");
        int last   = props->getIntValue (slProc.processorOptions.pluginName + "_lastUpdateCheck");

        if (url.isNotEmpty())
        {
            updateUrl = url;
            triggerAsyncUpdate();
        }
        else if (time (nullptr) > last + 86400)
        {
            startTimer (juce::Random::getSystemRandom().nextInt ({1500, 2500}));
        }
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
    juce::URL versionsUrl = juce::URL (slProc.processorOptions.updatesURL).withParameter ("plugin", slProc.processorOptions.pluginName).withParameter ("version", slProc.processorOptions.pluginVersion);
    juce::XmlDocument doc (versionsUrl.readEntireTextStream());
    if (std::unique_ptr<juce::XmlElement> root = doc.getDocumentElement())
    {
        if (auto props = slProc.getSettings())
        {
            props->setValue (slProc.processorOptions.pluginName + "_lastUpdateCheck", int (time (nullptr)));

            auto* child = root->getChildElement (0);
            while (child)
            {
                juce::String name = child->getStringAttribute ("name");
                juce::String ver  = child->getStringAttribute ("num");
                juce::String url  = child->getStringAttribute ("url");

                if (name == slProc.processorOptions.pluginName && versionStringToInt (ver) > versionStringToInt (slProc.processorOptions.pluginVersion))
                {
                    props->setValue (slProc.processorOptions.pluginName + "_updateUrl", url);
                    updateUrl = url;
                    triggerAsyncUpdate();
                    break;
                }

                child = child->getNextElement();
            }
        }
    }
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
    setName ("titlebar");

    programs.setName ("presets");

    addAndMakeVisible (menuButton);
    addAndMakeVisible (browseButton);
    addAndMakeVisible (programs);
    addAndMakeVisible (nextButton);
    addAndMakeVisible (prevButton);
    addAndMakeVisible (addButton);
    addAndMakeVisible (deleteButton);
    addAndMakeVisible (infoButton);

    programs.addListener (this);

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

    if (slProc.processorOptions.useUpdateChecker)
    {
        updateChecker = std::make_unique<UpdateChecker> (slProc);
        updateChecker->onUpdate = [](juce::String) {};
    }

    if (slProc.processorOptions.useNewsChecker)
    {
        newsChecker = std::make_unique<NewsChecker> (slProc);
        newsChecker->onNewsUpdate = [](juce::String) {};
    }

    nextButton.onClick = [this]
    {
        int prog = slProc.getCurrentProgram() + 1;
        if (prog >= slProc.getPrograms().size())
            prog = 0;

        slProc.setCurrentProgram (prog);
    };
    prevButton.onClick = [this]
    {
        int prog = slProc.getCurrentProgram() - 1;
        if (prog < 0)
            prog = slProc.getPrograms().size() - 1;

        slProc.setCurrentProgram (prog);
    };
    browseButton.onClick = [this]
    {
        browseButton.setToggleState (! browseButton.getToggleState(), juce::dontSendNotification);
        editor.showPatchBrowser (browseButton.getToggleState());
    };
    addButton.onClick = [this]
    {
        Program* prog = nullptr;
        
        int progIdx = slProc.getCurrentProgram();
        if (progIdx > 0)
            prog = slProc.getPrograms()[progIdx];
        
        auto w = std::make_shared<gin::PluginAlertWindow> ("Create preset:", "", juce::AlertWindow::NoIcon, getParentComponent());
        w->setLookAndFeel (slProc.lf.get());
        w->addTextEditor ("name", prog != nullptr ? prog->name : juce::String(), "Name:");

        if (hasBrowser)
        {
            w->addTextEditor ("author", prog != nullptr ? prog->author : juce::String(), "Author:");
            w->addTextEditor ("tags", prog != nullptr ? juce::StringArray (prog->tags).joinIntoString (" ") : juce::String(), "Tags:");
        }

        w->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
        w->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));

        w->runAsync (*getParentComponent(), [this, w] (int ret)
        {
            w->setVisible (false);
            if (ret == 1)
            {
                auto txt = juce::File::createLegalFileName (w->getTextEditor ("name")->getText()).trim();
                auto aut = (hasBrowser) ? juce::File::createLegalFileName (w->getTextEditor ("author")->getText()).trim() : juce::String();
                auto tag = (hasBrowser) ? juce::File::createLegalFileName (w->getTextEditor ("tags")->getText()).trim() : juce::String();

                if (slProc.hasProgram (txt))
                {
                    auto wc = std::make_shared<gin::PluginAlertWindow> ("Overwrite preset '" + txt + "'?", "", juce::AlertWindow::NoIcon, this);
                    wc->addButton ("Yes", 1, juce::KeyPress (juce::KeyPress::returnKey));
                    wc->addButton ("No", 0, juce::KeyPress (juce::KeyPress::escapeKey));
                    wc->setLookAndFeel (slProc.lf.get());

                    wc->runAsync (*getParentComponent(), [this, aut, tag, txt, wc] (int r)
                    {
                        wc->setVisible (false);
                        if (r == 1)
                        {
                            slProc.saveProgram (txt, aut, tag);
                            refreshPrograms();
                        }
                    });
                }
                else if (txt.isNotEmpty())
                {
                    slProc.saveProgram (txt, aut, tag);
                    refreshPrograms();
                }
            }
        });
    };
    deleteButton.onClick = [this]
    {
        auto w = std::make_shared<gin::PluginAlertWindow> ("Delete preset '" + slProc.getProgramName (programs.getSelectedItemIndex()) + "'?", "", juce::AlertWindow::NoIcon, getParentComponent());
        w->addButton ("Yes", 1, juce::KeyPress (juce::KeyPress::returnKey));
        w->addButton ("No", 0, juce::KeyPress (juce::KeyPress::escapeKey));
        w->setLookAndFeel (slProc.lf.get());

        w->runAsync (*getParentComponent(), [this, w] (int r)
        {
            w->setVisible (false);
            if (r == 1)
            {
                slProc.deleteProgram (programs.getSelectedItemIndex());
                refreshPrograms();
            }
        });
    };
    infoButton.onClick = [this]
    {
        editor.showAboutInfo();
    };
    menuButton.onClick = [this]
    {
        showMenu();
    };
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

void TitleBar::setShowMenu (bool s)
{
    menuButton.setVisible (s);
}

void TitleBar::setShowInfo (bool s)
{
    infoButton.setVisible (s);
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
        auto sz = programsRC.getHeight();
        prevButton.setBounds (programsRC.removeFromLeft (sz).withSizeKeepingCentre (sz, sz));
        nextButton.setBounds (programsRC.removeFromRight (sz).withSizeKeepingCentre (sz, sz));
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

    for (int i = 0; i < slProc.getPrograms().size(); i++)
    {
        programs.addItem (slProc.getProgramName (i), i + 1);
        if (i == 0)
            programs.addSeparator();
    }

    programs.setSelectedItemIndex (slProc.getCurrentProgram(), juce::dontSendNotification);
    deleteButton.setEnabled (slProc.getCurrentProgram() != 0);

    editor.refreshPatchBrowser();
}

void TitleBar::showMenu()
{
    juce::PopupMenu m;
    m.setLookAndFeel (&getLookAndFeel());

    if (slProc.processorOptions.urlTitle.isNotEmpty())
    {
        m.addItem (slProc.processorOptions.urlTitle, [this]
        {
            juce::URL (slProc.processorOptions.url).launchInDefaultBrowser();
        });
    }

    m.addSeparator();

    if (updateChecker)
    {
        auto updateUrl = updateChecker->getUpdateUrl();
        m.addItem ("Get update", updateUrl.isNotEmpty(), false, [this, updateUrl]
        {
            juce::URL (updateUrl).launchInDefaultBrowser();

            if (auto props = slProc.getSettings())
                props->setValue (slProc.processorOptions.pluginName + "_updateUrl", "");
        });
    }

    if (newsChecker)
    {
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
    }

    m.addSeparator();

    m.addItem ("Accessible Keyboard", true, editor.getUseIncreasedKeyboardAccessibility(), [this]
    {
        editor.setUseIncreasedKeyboardAccessibility (! editor.getUseIncreasedKeyboardAccessibility());
    });
    
    editor.addMenuItems (m);

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

    triggerAsyncUpdate();
}

ProcessorEditor::ProcessorEditor (Processor& p, int cx_, int cy_) noexcept
    : ProcessorEditorBase (p, cx_, cy_), slProc (p)
{
    setLookAndFeel (slProc.lf.get());

    tooltipWindow.setMillisecondsBeforeTipAppears (2000);

    addAndMakeVisible (titleBar);
    addChildComponent (patchBrowser);

    titleBar.refreshPrograms();

    triggerAsyncUpdate();
}

ProcessorEditor::~ProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void ProcessorEditor::handleAsyncUpdate()
{
    if (ginProcessor.state.getChildWithName ("instance").getProperty ("browserOpen", false))
    {
        titleBar.setBrowseButtonState (true);
        showPatchBrowser (true);
    }
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

   #if JUCE_DEBUG
    msg += slProc.processorOptions.pluginName + " v" + slProc.processorOptions.pluginVersion + " (" __TIME__ " " __DATE__ ")\n\n";
   #else
    msg += slProc.processorOptions.pluginName + " v" + slProc.processorOptions.pluginVersion + " (" __DATE__ ")\n\n";
   #endif
    msg += slProc.processorOptions.programmingCredits.joinIntoString ("\n");

    msg += "\n\n";
    msg += "Copyright ";
    msg += juce::String (&__DATE__[7]);

    auto w = std::make_shared<gin::PluginAlertWindow> ("---- About ----", msg, juce::AlertWindow::NoIcon, this);
    w->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
    w->setLookAndFeel (slProc.lf.get());

    w->runAsync (*this, [w] (int)
    {
        w->setVisible (false);
    });
}

void ProcessorEditor::refreshProgramsList()
{
    titleBar.refreshPrograms();
}

void ProcessorEditor::refreshPatchBrowser()
{
    patchBrowser.refresh();
}

void ProcessorEditor::showPatchBrowser (bool p)
{
    patchBrowser.toFront (false);
    patchBrowser.setVisible (p);

    ginProcessor.state.getChildWithName ("instance").setProperty ("browserOpen", p, nullptr);
}
