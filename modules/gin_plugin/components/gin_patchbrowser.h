//==============================================================================
class PatchBrowser : public juce::Component
{
public:
    PatchBrowser (Processor& p);

    void selectionUpdated();
    void refresh();
    void resized() override;

    void paint (juce::Graphics& g) override;
    
    void editPreset (int row);
    void deletePreset (int row);

    Processor& proc;

    struct AuthorsModel : public juce::ListBoxModel
    {
        AuthorsModel (PatchBrowser& o);
        int getNumRows() override;
        void selectedRowsChanged (int) override;
        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override;

        PatchBrowser& owner;
    };

    struct TagsModel : public juce::ListBoxModel
    {
        TagsModel (PatchBrowser& o);

        int getNumRows() override;
        void selectedRowsChanged (int) override;
        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override;
        
        PatchBrowser& owner;
    };

    struct PresetsModel : public juce::ListBoxModel
    {
        PresetsModel (PatchBrowser& o);

        int getNumRows() override;
        void selectedRowsChanged (int) override;
        void paintListBoxItem (int row, juce::Graphics& g, int w, int h, bool selected) override;
        void listBoxItemDoubleClicked (int row, const juce::MouseEvent&) override;
        void listBoxItemClicked (int row, const juce::MouseEvent& e) override;

        PatchBrowser& owner;
    };

    AuthorsModel authorsModel   { *this };
    TagsModel tagsModel         { *this };
    PresetsModel presetsModel   { *this };

    juce::ListBox authors { "", &authorsModel }, tags { "", &tagsModel }, presets { "", &presetsModel };

    bool updatingSelection = false;
    juce::StringArray currentAuthors, currentTags, currentPresets;
    juce::StringArray selectedAuthors, selectedTags, selectedPresets;
};
