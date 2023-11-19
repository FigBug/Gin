//==============================================================================
PatchBrowser::PatchBrowser (Processor& p) : proc (p)
{
    addAndMakeVisible (authors);
    addAndMakeVisible (tags);
    addAndMakeVisible (presets);

    authors.setColour (juce::ListBox::outlineColourId, juce::Colours::black);
    tags.setColour (juce::ListBox::outlineColourId, juce::Colours::black);
    presets.setColour (juce::ListBox::outlineColourId, juce::Colours::black);

    authors.setOutlineThickness (1);
    tags.setOutlineThickness (1);
    presets.setOutlineThickness (1);

    authors.setMultipleSelectionEnabled (true);
    tags.setMultipleSelectionEnabled (true);

    auto inst = proc.state.getChildWithName ("instance");
    selectedAuthors = juce::StringArray::fromTokens (inst.getProperty ("selectedAuthors", {}).toString(), "|", "");
    selectedTags    = juce::StringArray::fromTokens (inst.getProperty ("selectedTags", {}).toString(), "|", "");
    selectedPresets = juce::StringArray::fromTokens (inst.getProperty ("selectedPresets", {}).toString(), "|", "");

    refresh();
}

void PatchBrowser::selectionUpdated()
{
    if (updatingSelection)
        return;
    
    selectedAuthors.clear();
    selectedTags.clear();
    selectedPresets.clear();

    for (int i = 0; i < authors.getNumSelectedRows(); i++)
        if (auto row = authors.getSelectedRow (i); row > 0)
            selectedAuthors.add (currentAuthors[row]);

    for (int i = 0; i < tags.getNumSelectedRows(); i++)
        if (auto row = tags.getSelectedRow (i); row > 0)
        selectedTags.add (currentTags[row]);
    
    for (int i = 0; i < presets.getNumSelectedRows(); i++)
        selectedPresets.add (currentPresets[presets.getSelectedRow (i)]);

    auto inst = proc.state.getChildWithName ("instance");
    inst.setProperty ("selectedAuthors",  selectedAuthors.joinIntoString ("|"), nullptr);
    inst.setProperty ("selectedTags",     selectedTags.joinIntoString ("|"), nullptr);
    inst.setProperty ("selectedPresets",  selectedPresets.joinIntoString ("|"), nullptr);
}

void PatchBrowser::refresh()
{
    if (updatingSelection)
        return;
    
    currentAuthors.clear();
    currentTags.clear();
    currentPresets.clear();

    for (auto program : proc.getPrograms())
    {
        if (program->author.isNotEmpty())
            currentAuthors.addIfNotAlreadyThere (program->author);

        if (program->name == "Default")
            continue;

        if (! selectedAuthors.isEmpty())
            if (! selectedAuthors.contains (program->author))
                continue;
        
        for (auto t : program->tags)
            if (t.isNotEmpty())
                currentTags.addIfNotAlreadyThere (t);

        if (! selectedTags.isEmpty())
        {
            bool hasOneTag = false;

            for (auto t : program->tags)
                if (selectedTags.contains (t))
                    hasOneTag = true;

            if (! hasOneTag)
                continue;
        }

        currentPresets.addIfNotAlreadyThere (program->name);
    }

    currentAuthors.sort (true);
    currentTags.sort (true);
    currentPresets.sort (true);
    
    currentAuthors.insert (0, "<All>");
    currentTags.insert (0, "<All>");

    authors.updateContent();
    tags.updateContent();
    presets.updateContent();
    
    juce::ScopedValueSetter<bool> svs (updatingSelection, true);
    
    authors.deselectAllRows();
    if (selectedAuthors.size() > 0)
    {
        for (auto a : selectedAuthors)
            authors.selectRow (currentAuthors.indexOf (a), true, false);
    }
    else
    {
        authors.selectRow (0, true, false);
    }

    tags.deselectAllRows();
    if (selectedTags.size() > 0)
    {
        for (auto t : selectedTags)
            tags.selectRow (currentTags.indexOf (t), true, false);
    }
    else
    {
        tags.selectRow (0, true, false);
    }

    presets.deselectAllRows();
    for (auto p : selectedPresets)
        presets.selectRow (currentPresets.indexOf (p), true, false);

    repaint();
}

void PatchBrowser::resized()
{
    auto r = getLocalBounds().reduced (20);

    int w = (r.getWidth() - 10) / 3;

    authors.setBounds (r.removeFromLeft (w));
    presets.setBounds (r.removeFromRight (w));
    tags.setBounds (r.reduced (5, 0));
}

void PatchBrowser::paint (juce::Graphics& g)
{
    g.fillAll (findColour (PluginLookAndFeel::matte1ColourId));
}

void PatchBrowser::deletePreset (int row)
{
    auto p = proc.getProgram (currentPresets[row]);
    if (p == nullptr)
        return;
    
    auto ed = findParentComponentOfClass<ProcessorEditor>();
    
    auto w = std::make_shared<gin::PluginAlertWindow> ("Delete preset '" + p->name + "'?", "", juce::AlertWindow::NoIcon, getParentComponent());
    w->addButton ("Yes", 1, juce::KeyPress (juce::KeyPress::returnKey));
    w->addButton ("No", 0, juce::KeyPress (juce::KeyPress::escapeKey));
    w->setLookAndFeel (&getLookAndFeel());

    w->runAsync (*ed, [this, w, p] (int r)
    {
        w->setVisible (false);
        if (r == 1)
            proc.deleteProgram (proc.getPrograms().indexOf (p));
    });
}

void PatchBrowser::editPreset (int row)
{
    auto p = proc.getProgram (currentPresets[row]);
    if (p == nullptr)
        return;
    
    auto ed = findParentComponentOfClass<ProcessorEditor>();
    
    auto w = std::make_shared<gin::PluginAlertWindow> ("Edit preset:", "", juce::AlertWindow::NoIcon, getParentComponent());
    w->setLookAndFeel (&getLookAndFeel());
    w->addTextEditor ("name", p->name, "Name:");
    w->addTextEditor ("author", p->author, "Author:");
    w->addTextEditor ("tags", p->tags.joinIntoString (" "), "Tags:");

    w->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
    w->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));

    w->runAsync (*ed, [this, w, p, ed] (int ret)
    {
        w->setVisible (false);
        if (ret == 1)
        {
            auto txt = juce::File::createLegalFileName (w->getTextEditor ("name")->getText());
            auto aut = juce::File::createLegalFileName (w->getTextEditor ("author")->getText());
            auto tag = juce::File::createLegalFileName (w->getTextEditor ("tags")->getText());

            if (proc.hasProgram (txt))
            {
                auto wc = std::make_shared<gin::PluginAlertWindow> ("Preset name '" + txt + "' already in use.", "", juce::AlertWindow::NoIcon, this);
                wc->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
                wc->setLookAndFeel (proc.lf.get());

                wc->runAsync (*ed, [wc] (int)
                {
                    wc->setVisible (false);
                });
            }
            else if (txt.isNotEmpty())
            {
                p->loadFromFile (p->getPresetFile (proc.getProgramDirectory()), true);
                p->deleteFromDir (proc.getProgramDirectory());
                p->name = txt;
                p->tags = juce::StringArray::fromTokens (tag, " ", "");
                p->author = aut;
                p->saveToDir (proc.getProgramDirectory());

                proc.updateHostDisplay();
                proc.sendChangeMessage();
            }
        }
    });
}

PatchBrowser::AuthorsModel::AuthorsModel (PatchBrowser& o)
    : owner (o)
{
}

int PatchBrowser::AuthorsModel::getNumRows()
{
    return owner.currentAuthors.size();
}

void PatchBrowser::AuthorsModel::selectedRowsChanged (int)
{
    owner.selectionUpdated();
    owner.refresh();
}

void PatchBrowser::AuthorsModel::paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected)
{
    auto rc = juce::Rectangle<int> (0, 0, w, h);

    if (selected)
        g.setColour (owner.findColour (PluginLookAndFeel::accentColourId, true).withAlpha (0.5f));
    else if (row % 2 == 0)
        g.setColour (owner.findColour (PluginLookAndFeel::matte1ColourId, true));
    else
        g.setColour (owner.findColour (PluginLookAndFeel::matte1ColourId, true).overlaidWith (juce::Colours::white.withAlpha (0.02f)));

    g.fillRect (rc);

    g.setColour (owner.findColour (PluginLookAndFeel::whiteColourId, true).withAlpha (0.9f));
    g.setFont (juce::Font (14));
    g.drawText (owner.currentAuthors[row], rc.reduced (4, 0), juce::Justification::centredLeft);
}

PatchBrowser::TagsModel::TagsModel (PatchBrowser& o)
    : owner (o)
{
}

int PatchBrowser::TagsModel::getNumRows()
{
    return owner.currentTags.size();
}

void PatchBrowser::TagsModel::selectedRowsChanged (int)
{
    owner.selectionUpdated();
    owner.refresh();
}

void PatchBrowser::TagsModel::paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected)
{
    auto rc = juce::Rectangle<int> (0, 0, w, h);

    if (selected)
        g.setColour (owner.findColour (PluginLookAndFeel::accentColourId, true).withAlpha (0.5f));
    else if (row % 2 == 0)
        g.setColour (owner.findColour (PluginLookAndFeel::matte1ColourId, true));
    else
        g.setColour (owner.findColour (PluginLookAndFeel::matte1ColourId, true).overlaidWith (juce::Colours::white.withAlpha (0.02f)));

    g.fillRect (rc);

    g.setColour (owner.findColour (PluginLookAndFeel::whiteColourId, true).withAlpha (0.9f));
    g.setFont (juce::Font (14));
    g.drawText (owner.currentTags[row], rc.reduced (4, 0), juce::Justification::centredLeft);
}

PatchBrowser::PresetsModel::PresetsModel (PatchBrowser& o)
    : owner (o)
{
}

int PatchBrowser::PresetsModel::getNumRows()
{
    return owner.currentPresets.size();
}

void PatchBrowser::PresetsModel::selectedRowsChanged (int)
{
    owner.selectionUpdated();
}

void PatchBrowser::PresetsModel::paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected)
{
    auto rc = juce::Rectangle<int> (0, 0, w, h);

    if (selected)
        g.setColour (owner.findColour (PluginLookAndFeel::accentColourId, true).withAlpha (0.5f));
    else if (row % 2 == 0)
        g.setColour (owner.findColour (PluginLookAndFeel::matte1ColourId, true));
    else
        g.setColour (owner.findColour (PluginLookAndFeel::matte1ColourId, true).overlaidWith (juce::Colours::white.withAlpha (0.02f)));

    g.fillRect (rc);

    g.setColour (owner.findColour (PluginLookAndFeel::whiteColourId, true).withAlpha (0.9f));
    g.setFont (juce::Font (14));
    g.drawText (owner.currentPresets[row], rc.reduced (4, 0), juce::Justification::centredLeft);
}

void PatchBrowser::PresetsModel::listBoxItemDoubleClicked (int row, const juce::MouseEvent&)
{
    owner.proc.setCurrentProgram (owner.currentPresets[row]);
}

void PatchBrowser::PresetsModel::listBoxItemClicked (int row, const juce::MouseEvent& e)
{
    if ( ! e.mouseWasClicked() || ! e.mods.isPopupMenu())
        return;
    
    auto p = owner.proc.getProgram (owner.currentPresets[row]);
    if (p == nullptr)
        return;
    
    auto f = p->getPresetFile (owner.proc.getProgramDirectory());
    
    juce::PopupMenu m;
    m.setLookAndFeel (&owner.getLookAndFeel());
    
    m.addItem ("Edit Preset...", [this, row] { owner.editPreset (row); });
    m.addItem ("Delete Preset...", [this, row] { owner.deletePreset (row); });
    
    m.addSeparator();
    
   #if JUCE_MAC
    m.addItem ("Reveal in finder...", [f] { f.revealToUser(); });
   #elif JUCE_WINDOWS
    m.addItem ("Show in Explorer...", [f] { f.revealToUser(); });
   #else
    m.addItem ("Show file...", [f] { f.revealToUser(); });
   #endif
    
    m.showMenuAsync ({});
}


