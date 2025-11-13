//==============================================================================
/**
    Preset/patch browser component with filtering by author and tags.

    PatchBrowser provides a comprehensive UI for browsing, selecting, and managing
    audio plugin presets. It features three list views for filtering by author,
    tags, and displaying matching presets. The browser supports preset navigation,
    editing, and deletion.

    Key Features:
    - Three-column layout: Authors, Tags, and Presets
    - Multi-criteria filtering (by author and/or tag)
    - Keyboard navigation (next/previous)
    - Context menu support for editing and deleting presets
    - Automatic selection synchronization
    - Integrated with Gin Processor preset system

    UI Layout:
    - Left column: List of preset authors
    - Middle column: List of tags/categories
    - Right column: Filtered list of presets
    - Selection in authors/tags filters the presets list

    Usage:
    @code
    class MyEditor : public ProcessorEditor
    {
        PatchBrowser patchBrowser { processor };

        MyEditor()
        {
            addAndMakeVisible(patchBrowser);
            patchBrowser.refresh(); // Update preset lists
        }
    };
    @endcode

    The browser automatically updates when the processor's preset changes
    and notifies the processor when a new preset is selected.

    @see Processor, ProcessorEditor, TitleBar
*/
class PatchBrowser : public juce::Component
{
public:
    PatchBrowser (Processor& p);
    
    void next();
    void prev();

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
        void listBoxItemClicked (int row, const juce::MouseEvent& e) override;

        PatchBrowser& owner;
    };

    AuthorsModel authorsModel   { *this };
    TagsModel tagsModel         { *this };
    PresetsModel presetsModel   { *this };

    juce::ListBox authors { "", &authorsModel }, tags { "", &tagsModel }, presets { "", &presetsModel };

    bool updatingSelection = false;
    juce::StringArray currentAuthors, currentTags, currentPresets;
    juce::StringArray selectedAuthors, selectedTags;
};
