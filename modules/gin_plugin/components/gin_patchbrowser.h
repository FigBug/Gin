//==============================================================================
class PatchBrowser : public juce::Component
{
public:
    PatchBrowser (Processor& p) : proc (p)
    {
        addAndMakeVisible (authors);
        addAndMakeVisible (tags);
        addAndMakeVisible (presets);

        authors.setColour (juce::ListBox::outlineColourId, juce::Colours::white);
        tags.setColour (juce::ListBox::outlineColourId, juce::Colours::white);
        presets.setColour (juce::ListBox::outlineColourId, juce::Colours::white);

        authors.setOutlineThickness (1);
        tags.setOutlineThickness (1);
        presets.setOutlineThickness (1);

        authors.setMultipleSelectionEnabled (true);
        authors.setClickingTogglesRowSelection (true);
        tags.setMultipleSelectionEnabled (true);
        tags.setClickingTogglesRowSelection (true);

        refresh();
    }

    void updateSelection()
    {
        selectedAuthors.clear();
        selectedTags.clear();

        for (int i = 0; i < authors.getNumSelectedRows(); i++)
            selectedAuthors.add (currentAuthors[authors.getSelectedRow (i)]);

        for (int i = 0; i < tags.getNumSelectedRows(); i++)
            selectedTags.add (currentTags[tags.getSelectedRow (i)]);
    }

    void refresh()
    {
        currentAuthors.clear();
        currentTags.clear();
        currentPresets.clear();

        for (auto program : proc.getPrograms())
        {
            if (program->author.isNotEmpty())
                currentAuthors.addIfNotAlreadyThere (program->author);

            for (auto t : program->tags)
                if (t.isNotEmpty())
                    currentTags.addIfNotAlreadyThere (t);

            if (program->name == "Default")
                continue;

            if (! selectedAuthors.isEmpty())
                if (! selectedAuthors.contains (program->author))
                    continue;

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

        authors.updateContent();
        tags.updateContent();
        presets.updateContent();

        repaint();
    }

    void resized() override
    {
        int w = (getWidth() - 10) / 3;

        auto r = getLocalBounds();
        authors.setBounds (r.removeFromLeft (w));
        presets.setBounds (r.removeFromRight (w));
        tags.setBounds (r.reduced (5, 0));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
    }

    Processor& proc;

    struct AuthorsModel : public juce::ListBoxModel
    {
        AuthorsModel (PatchBrowser& o) : owner (o) {}

        int getNumRows() override
        {
            return owner.currentAuthors.size();
        }

        void selectedRowsChanged (int) override
        {
            owner.updateSelection();
            owner.refresh();
        }

        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override
        {
            auto c1 = owner.findColour (juce::TextButton::textColourOnId);
            auto c2 = owner.findColour (juce::TextButton::textColourOffId);

            if (selected)
                std::swap (c1, c2);

            auto rc = juce::Rectangle<int> (0, 0, w, h);

            g.setColour (c1);
            g.fillRect (rc);

            g.setColour (c2);
            g.setFont (juce::Font (14));
            g.drawText (owner.currentAuthors[row], rc.reduced (4, 0), juce::Justification::centredLeft);
        }

        PatchBrowser& owner;
    };

    struct TagsModel : public juce::ListBoxModel
    {
        TagsModel (PatchBrowser& o) : owner (o) {}

        int getNumRows() override
        {
            return owner.currentTags.size();
        }

        void selectedRowsChanged (int) override
        {
            owner.updateSelection();
            owner.refresh();
        }

        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override
        {
            auto c1 = owner.findColour (juce::TextButton::textColourOnId);
            auto c2 = owner.findColour (juce::TextButton::textColourOffId);

            if (selected)
                std::swap (c1, c2);

            auto rc = juce::Rectangle<int> (0, 0, w, h);

            g.setColour (c1);
            g.fillRect (rc);

            g.setColour (c2);
            g.setFont (juce::Font (14));
            g.drawText (owner.currentTags[row], rc.reduced (4, 0), juce::Justification::centredLeft);
        }

        PatchBrowser& owner;
    };

    struct PresetsModel : public juce::ListBoxModel
    {
        PresetsModel (PatchBrowser& o) : owner (o) {}

        int getNumRows() override
        {
            return owner.currentPresets.size();
        }

        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override
        {
            auto c1 = owner.findColour (juce::TextButton::textColourOnId);
            auto c2 = owner.findColour (juce::TextButton::textColourOffId);

            if (selected)
                std::swap (c1, c2);

            auto rc = juce::Rectangle<int> (0, 0, w, h);

            g.setColour (c1);
            g.fillRect (rc);

            g.setColour (c2);
            g.setFont (juce::Font (14));
            g.drawText (owner.currentPresets[row], rc.reduced (4, 0), juce::Justification::centredLeft);
        }

        void listBoxItemDoubleClicked (int row, const juce::MouseEvent&) override
        {
            owner.proc.setCurrentProgram (owner.currentPresets[row]);
        }

        PatchBrowser& owner;
    };

    AuthorsModel authorsModel   { *this };
    TagsModel tagsModel         { *this };
    PresetsModel presetsModel   { *this };

    juce::ListBox authors { "", &authorsModel }, tags { "", &tagsModel }, presets { "", &presetsModel };

    juce::StringArray currentAuthors, currentTags, currentPresets;
    juce::StringArray selectedAuthors, selectedTags;
};
