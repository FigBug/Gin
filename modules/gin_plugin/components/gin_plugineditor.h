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
};

//==============================================================================
/** Base for plugin editor
*/
class ProcessorEditorBase : public juce::AudioProcessorEditor
{
public:
    ProcessorEditorBase (Processor& p, int cx_ = 100, int cy_ = 100)
        : AudioProcessorEditor (p), ginProcessor (p), cx (cx_), cy (cy_)
    {
    }

    void makeResizable (int minX, int minY, int maxX, int maxY)
    {
        addAndMakeVisible (*(resizer = std::make_unique<juce::ResizableCornerComponent> (this, &resizeLimits)));
        resizeLimits.setSizeLimits (minX, minY, maxX, maxY);

        juce::ValueTree state (ginProcessor.state);

        if (state.hasProperty ("width") && state.hasProperty ("height"))
            setSize (state["width"], state["height"]);

        resized();
    }

    void resized() override
    {
        const juce::Rectangle<int> r (getLocalBounds());

        if (resizer != nullptr)
        {
            resizer->setBounds (juce::Rectangle<int> (r).removeFromRight (15).removeFromBottom (15));

            ginProcessor.state.setProperty ("width", getWidth(), nullptr);
            ginProcessor.state.setProperty ("height", getHeight(), nullptr);
        }
    }

    virtual juce::Rectangle<int> getControlsArea();
    virtual juce::Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1);
    juce::Rectangle<int> getFullGridArea();

    int getGridWidth()  { return cx; }
    int getGridHeight() { return cy; }

    juce::ComponentBoundsConstrainer resizeLimits;

protected:
    void setGridSize (int x, int y, int extraWidthPx = 0, int extraHeightPx = 0 );

    Processor& ginProcessor;
    std::unique_ptr<juce::ResizableCornerComponent> resizer;

    const int cx;
    const int cy;

    juce::OwnedArray<ParamComponent> controls;

    int headerHeight = 60, inset = 4;
    int cols = 0, rows = 0, extraWidthPx = 0, extraHeightPx = 0;

    ParamComponent* componentForId (const juce::String& uid);
    ParamComponent* componentForParam (Parameter& param);

    juce::TooltipWindow tooltipWindow { this };
};

//==============================================================================
/** Plugin title bar
 */
class ProcessorEditor;
class TitleBar : public juce::Component,
                 protected juce::Button::Listener,
                 protected juce::ComboBox::Listener,
                 protected juce::ChangeListener
{
public:
    TitleBar (ProcessorEditor&, Processor&);
    ~TitleBar () override;

    void refreshPrograms();
    void setShowBrowser (bool);

protected:
    void paint (juce::Graphics& g) override;
    void resized() override;

    void showMenu();

    void buttonClicked (juce::Button* b) override;
    void comboBoxChanged (juce::ComboBox* c) override;
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    ProcessorEditor& editor;
    Processor& slProc;

    bool hasBrowser = false;

    juce::ComboBox programs;
    juce::TextButton addButton {"svg:M384 250v12c0 6.6-5.4 12-12 12h-98v98c0 6.6-5.4 12-12 12h-12c-6.6 0-12-5.4-12-12v-98h-98c-6.6 0-12-5.4-12-12v-12c0-6.6 5.4-12 12-12h98v-98c0-6.6 5.4-12 12-12h12c6.6 0 12 5.4 12 12v98h98c6.6 0 12 5.4 12 12zm120 6c0 137-111 248-248 248S8 393 8 256 119 8 256 8s248 111 248 248zm-32 0c0-119.9-97.3-216-216-216-119.9 0-216 97.3-216 216 0 119.9 97.3 216 216 216 119.9 0 216-97.3 216-216z"};
    juce::TextButton deleteButton {"svg:M140 274c-6.6 0-12-5.4-12-12v-12c0-6.6 5.4-12 12-12h232c6.6 0 12 5.4 12 12v12c0 6.6-5.4 12-12 12H140zm364-18c0 137-111 248-248 248S8 393 8 256 119 8 256 8s248 111 248 248zm-32 0c0-119.9-97.3-216-216-216-119.9 0-216 97.3-216 216 0 119.9 97.3 216 216 216 119.9 0 216-97.3 216-216z"}; 
    juce::TextButton browseButton {"svg:M194.74 96l54.63 54.63c6 6 14.14 9.37 22.63 9.37h192c8.84 0 16 7.16 16 16v224c0 8.84-7.16 16-16 16H48c-8.84 0-16-7.16-16-16V112c0-8.84 7.16-16 16-16h146.74M48 64C21.49 64 0 85.49 0 112v288c0 26.51 21.49 48 48 48h416c26.51 0 48-21.49 48-48V176c0-26.51-21.49-48-48-48H272l-54.63-54.63c-6-6-14.14-9.37-22.63-9.37H48z"};
    juce::TextButton nextButton {"svg:M17.525 36.465l-7.071 7.07c-4.686 4.686-4.686 12.284 0 16.971L205.947 256 10.454 451.494c-4.686 4.686-4.686 12.284 0 16.971l7.071 7.07c4.686 4.686 12.284 4.686 16.97 0l211.051-211.05c4.686-4.686 4.686-12.284 0-16.971L34.495 36.465c-4.686-4.687-12.284-4.687-16.97 0z"};
    juce::TextButton prevButton {"svg:M238.475 475.535l7.071-7.07c4.686-4.686 4.686-12.284 0-16.971L50.053 256 245.546 60.506c4.686-4.686 4.686-12.284 0-16.971l-7.071-7.07c-4.686-4.686-12.284-4.686-16.97 0L10.454 247.515c-4.686 4.686-4.686 12.284 0 16.971l211.051 211.05c4.686 4.686 12.284 4.686 16.97-.001z"};

    juce::TextButton menuButton {"svg:M16 132h416c8.837 0 16-7.163 16-16V76c0-8.837-7.163-16-16-16H16C7.163 60 0 67.163 0 76v40c0 8.837 7.163 16 16 16zm0 160h416c8.837 0 16-7.163 16-16v-40c0-8.837-7.163-16-16-16H16c-8.837 0-16 7.163-16 16v40c0 8.837 7.163 16 16 16zm0 160h416c8.837 0 16-7.163 16-16v-40c0-8.837-7.163-16-16-16H16c-8.837 0-16 7.163-16 16v40c0 8.837 7.163 16 16 16z"};
    juce::TextButton infoButton {"svg:M256 40c118.621 0 216 96.075 216 216 0 119.291-96.61 216-216 216-119.244 0-216-96.562-216-216 0-119.203 96.602-216 216-216m0-32C119.043 8 8 119.083 8 256c0 136.997 111.043 248 248 248s248-111.003 248-248C504 119.083 392.957 8 256 8zm-36 344h12V232h-12c-6.627 0-12-5.373-12-12v-8c0-6.627 5.373-12 12-12h48c6.627 0 12 5.373 12 12v140h12c6.627 0 12 5.373 12 12v8c0 6.627-5.373 12-12 12h-72c-6.627 0-12-5.373-12-12v-8c0-6.627 5.373-12 12-12zm36-240c-17.673 0-32 14.327-32 32s14.327 32 32 32 32-14.327 32-32-14.327-32-32-32z"};

    std::unique_ptr<NewsChecker> newsChecker;
    std::unique_ptr<UpdateChecker> updateChecker;
};

//==============================================================================
/** Plugin editor
*/
class ProcessorEditor : public ProcessorEditorBase
{
public:
    ProcessorEditor (Processor&, int cx = 100, int cy = 100) noexcept;
    ~ProcessorEditor() override;

    void showAboutInfo();

    void refreshPatchBrowser();
    void showPatchBrowser (bool p);

    Processor& slProc;

protected:
    void paint (juce::Graphics& g) override;
    void resized() override;

    std::unique_ptr<UpdateChecker> updateChecker;
    std::unique_ptr<NewsChecker> newsChecker;

    juce::String additionalProgramming;

    TitleBar titleBar { *this, slProc };
    PatchBrowser patchBrowser { slProc };

    void refreshPrograms();
};
