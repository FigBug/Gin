#pragma once

//==============================================================================
class ProcessorEditor;

/** Checks for plugin updates
*/
class UpdateChecker : public juce::Timer,
                      public juce::Thread,
                      private juce::AsyncUpdater
{
public:
    UpdateChecker (Processor& slProc);
    ~UpdateChecker() override;

    juce::String getUpdateUrl() { return updateUrl; }

    std::function<void (juce::String)> onUpdate;

private:
    void handleAsyncUpdate() override;
    void timerCallback() override;
    void run() override;

    Processor& slProc;
    juce::String updateUrl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdateChecker)
};

//==============================================================================
/** Checks Wordpress feed for plugin news
*/
class NewsChecker : public juce::Timer,
                    private juce::Thread,
                    private juce::AsyncUpdater
{
public:
    NewsChecker (Processor& slProc);
    ~NewsChecker() override;

    juce::String getNewsUrl() { return newsUrl; }

    std::function<void (juce::String)> onNewsUpdate;

private:
    void handleAsyncUpdate() override;
    void timerCallback() override;
    void run() override;

    Processor& slProc;
    juce::String newsUrl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewsChecker)
};

//==============================================================================
/** Base for plugin editor
*/
class ProcessorEditorBase : public juce::AudioProcessorEditor
                          , protected juce::FocusChangeListener

{
public:
    ProcessorEditorBase (Processor& p, int cx_, int cy_)
        : AudioProcessorEditor (p), ginProcessor (p), cx (cx_), cy (cy_)
    {
        juce::Desktop::getInstance().addFocusChangeListener (this);
    }

    ~ProcessorEditorBase() override
    {
        juce::Desktop::getInstance().removeFocusChangeListener (this);
    }

    void makeResizable (int minX, int minY, int maxX, int maxY)
    {
        addAndMakeVisible (*(resizer = std::make_unique<juce::ResizableCornerComponent> (this, &resizeLimits)));
        resizeLimits.setSizeLimits (minX, minY, maxX, maxY);

        juce::ValueTree state (ginProcessor.state);

        if (auto inst = state.getChildWithName ("instance"); inst.isValid())
            if (inst.hasProperty ("width") && inst.hasProperty ("height"))
                setSize (inst["width"], inst["height"]);

        resized();
    }

    void resized() override
    {
        const juce::Rectangle<int> r (getLocalBounds());

        if (resizer != nullptr)
        {
            resizer->setBounds (juce::Rectangle<int> (r).removeFromRight (15).removeFromBottom (15));

            auto inst = ginProcessor.state.getOrCreateChildWithName ("instance", nullptr);
            inst.setProperty ("width", getWidth(), nullptr);
            inst.setProperty ("height", getHeight(), nullptr);
        }
    }

    void globalFocusChanged (juce::Component*) override
    {
        if (getUseIncreasedKeyboardAccessibility())
            repaint();
    }

    void paintOverChildren (juce::Graphics& g) override
    {
        if (! getUseIncreasedKeyboardAccessibility()) return;
        auto f = getCurrentlyFocusedComponent();
        if (f == nullptr || ! isParentOf (f)) return;

        auto rc = getLocalArea (f, f->getLocalBounds());

        g.setColour (findColour (PluginLookAndFeel::accentColourId, true).withMultipliedAlpha (0.35f));
        g.fillRect (rc);
    }

    void addControl (ParamComponent* c, int x = 0, int y = 0, int w = 1, int h = 1)
    {
        controls.add (c);
        addAndMakeVisible (c);
        c->setBounds (getGridArea (x, y, w, h));
    }

    virtual juce::Rectangle<int> getControlsArea();
    virtual juce::Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1);
    juce::Rectangle<int> getFullGridArea();

    int getGridWidth()  { return cx; }
    int getGridHeight() { return cy; }

    bool getUseIncreasedKeyboardAccessibility();
    void setUseIncreasedKeyboardAccessibility (bool accessible);

    juce::ComponentBoundsConstrainer resizeLimits;

protected:
    void setGridSize (int x, int y, int extraWidthPx = 0, int extraHeightPx = 0 );

    Processor& ginProcessor;
    std::unique_ptr<juce::ResizableCornerComponent> resizer;

    const int cx;
    const int cy;

    juce::OwnedArray<ParamComponent> controls;

    int headerHeight = 40, inset = 4;
    int cols = 0, rows = 0, extraWidthPx = 0, extraHeightPx = 0;

    ParamComponent* componentForId (const juce::String& uid);
    ParamComponent* componentForParam (Parameter& param);

    juce::TooltipWindow tooltipWindow { this, 0 };
};

//==============================================================================
/** Plugin title bar
 */
class ProcessorEditor;
class TitleBar : public juce::Component,
                 protected juce::ComboBox::Listener,
                 protected juce::ChangeListener
{
public:
    TitleBar (ProcessorEditor&, Processor&);
    ~TitleBar () override;

    void refreshPrograms();
    void setShowBrowser (bool);
    void setShowPresets (bool);
    void setShowMenu (bool);
    void setShowInfo (bool);
    void setBrowseButtonState (bool s) { browseButton.setToggleState (s, juce::dontSendNotification); }

protected:
    void paint (juce::Graphics& g) override;
    void resized() override;

    void showMenu();

    void comboBoxChanged (juce::ComboBox* c) override;
    void changeListenerCallback (juce::ChangeBroadcaster*) override;
    void parentHierarchyChanged() override;

    ProcessorEditor& editor;
    Processor& slProc;

    bool hasPresets = true;
    bool hasBrowser = false;

    juce::ComboBox programs;
    SVGButton addButton { "add", gin::Assets::add };
    SVGButton deleteButton { "delete", gin::Assets::del };
    SVGButton browseButton { "browse", gin::Assets::browse };
    SVGButton nextButton { "next", gin::Assets::next, 15 };
    SVGButton prevButton { "prev", gin::Assets::prev, 15 };

    SVGButton menuButton { "menu", gin::Assets::menu };
    SVGButton infoButton { "info", gin::Assets::info };

    std::unique_ptr<NewsChecker> newsChecker;
    std::unique_ptr<UpdateChecker> updateChecker;
};

//==============================================================================
/** Plugin editor
*/
class ProcessorEditor : public ProcessorEditorBase,
                        protected juce::AsyncUpdater
{
public:
    ProcessorEditor (Processor&) noexcept;
    ProcessorEditor (Processor&, int cx, int cy) noexcept;
    ~ProcessorEditor() override;

    virtual void addMenuItems (juce::PopupMenu&) {}

    virtual void showAboutInfo();

    void refreshProgramsList();
    void refreshPatchBrowser();
    void showPatchBrowser (bool p);

    Processor& slProc;

protected:
    void handleAsyncUpdate() override;
    void paint (juce::Graphics& g) override;
    void resized() override;

    std::unique_ptr<UpdateChecker> updateChecker;
    std::unique_ptr<NewsChecker> newsChecker;

    TitleBar titleBar { *this, slProc };
    PatchBrowser patchBrowser { slProc };
};
