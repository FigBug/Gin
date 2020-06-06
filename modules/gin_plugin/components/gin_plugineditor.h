#pragma once

//==============================================================================
class ProcessorEditor;

/** Checks for plugin updates
*/
class UpdateChecker : public Timer,
                      public Thread
{
public:
    UpdateChecker (ProcessorEditor& editor_);
    ~UpdateChecker() override;

    void timerCallback() override;
    void run() override;

    ProcessorEditor& editor;
};

//==============================================================================
/** Checks Wordpress feed for plugin news
*/
class NewsChecker : public Timer,
                    private Thread,
                    private AsyncUpdater
{
public:
    NewsChecker (ProcessorEditor& editor_);
    ~NewsChecker() override;

private:
    void handleAsyncUpdate() override;
    void timerCallback() override;
    void run() override;

    ProcessorEditor& editor;
    
    String newsUrl;
};

//==============================================================================
/** Base for plugin editor
*/
class ProcessorEditorBase : public AudioProcessorEditor
{
public:
    ProcessorEditorBase (Processor& p, int cx_ = 100, int cy_ = 100)
        : AudioProcessorEditor (p), proc (p), cx (cx_), cy (cy_)
    {
    }

    void makeResizable (int minX, int minY, int maxX, int maxY)
    {
        addAndMakeVisible (*(resizer = std::make_unique<ResizableCornerComponent> (this, &resizeLimits)));
        resizeLimits.setSizeLimits (minX, minY, maxX, maxY);

        ValueTree state (proc.state);

        if (state.hasProperty ("width") && state.hasProperty ("height"))
            setSize (state["width"], state["height"]);

        resized();
    }

    void resized() override
    {
        const Rectangle<int> r (getLocalBounds());

        if (resizer != nullptr)
        {
            resizer->setBounds (Rectangle<int> (r).removeFromRight (15).removeFromBottom (15));

            proc.state.setProperty ("width", getWidth(), nullptr);
            proc.state.setProperty ("height", getHeight(), nullptr);
        }
    }

    virtual Rectangle<int> getControlsArea();
    virtual Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1);
    Rectangle<int> getFullGridArea();

    int getGridWidth()  { return cx; }
    int getGridHeight() { return cy; }

    ComponentBoundsConstrainer resizeLimits;

protected:
    void setGridSize (int x, int y, int extraWidthPx = 0, int extraHeightPx = 0 );

    Processor& proc;
    std::unique_ptr<ResizableCornerComponent> resizer;

    const int cx;
    const int cy;

    OwnedArray<ParamComponent> controls;

    int headerHeight = 60, inset = 4;
    int cols = 0, rows = 0, extraWidthPx = 0, extraHeightPx = 0;

    ParamComponent* componentForId (const String& uid);
    ParamComponent* componentForParam (Parameter& param);

    SharedResourcePointer<TooltipWindow> tooltipWindow;
};

//==============================================================================
/** Plugin editor
*/
class ProcessorEditor : public ProcessorEditorBase,
                        protected Button::Listener,
                        protected ComboBox::Listener,
                        protected ChangeListener
{
public:
    ProcessorEditor (Processor&, int cx = 100, int cy = 100) noexcept;
    ~ProcessorEditor() override;

    void updateReady (String updateUrl);
    void newsReady (String newsUrl);

    Processor& slProc;

protected:
    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* b) override;
    void comboBoxChanged (ComboBox* c) override;
    void changeListenerCallback (ChangeBroadcaster*) override;

    std::unique_ptr<UpdateChecker> updateChecker;
    std::unique_ptr<NewsChecker> newsChecker;

    ComboBox programs;
    TextButton addButton {"svg:M416 208H272V64c0-17.67-14.33-32-32-32h-32c-17.67 0-32 14.33-32 32v144H32c-17.67 0-32 14.33-32 32v32c0 17.67 14.33 32 32 32h144v144c0 17.67 14.33 32 32 32h32c17.67 0 32-14.33 32-32V304h144c17.67 0 32-14.33 32-32v-32c0-17.67-14.33-32-32-32z"};
    TextButton deleteButton {"svg:M432 32H312l-9.4-18.7A24 24 0 0 0 281.1 0H166.8a23.72 23.72 0 0 0-21.4 13.3L136 32H16A16 16 0 0 0 0 48v32a16 16 0 0 0 16 16h416a16 16 0 0 0 16-16V48a16 16 0 0 0-16-16zM53.2 467a48 48 0 0 0 47.9 45h245.8a48 48 0 0 0 47.9-45L416 128H32z"};
    TextButton browseButton {"svg:M464 128H272l-64-64H48C21.49 64 0 85.49 0 112v288c0 26.51 21.49 48 48 48h416c26.51 0 48-21.49 48-48V176c0-26.51-21.49-48-48-48z"};
    TextButton nextButton {"svg:M0 384.662V127.338c0-17.818 21.543-26.741 34.142-14.142l128.662 128.662c7.81 7.81 7.81 20.474 0 28.284L34.142 398.804C21.543 411.404 0 402.48 0 384.662z"};
    TextButton prevButton {"svg:M192 127.338v257.324c0 17.818-21.543 26.741-34.142 14.142L29.196 270.142c-7.81-7.81-7.81-20.474 0-28.284l128.662-128.662c12.599-12.6 34.142-3.676 34.142 14.142z"};

    TextButton socaButton {"svg:M288 32C128.94 32 0 89.31 0 160v192c0 70.69 128.94 128 288 128s288-57.31 288-128V160c0-70.69-128.94-128-288-128zm-82.99 158.36c-4.45 16.61-14.54 30.57-28.31 40.48C100.23 217.46 48 190.78 48 160c0-30.16 50.11-56.39 124.04-70.03l25.6 44.34c9.86 17.09 12.48 36.99 7.37 56.05zM288 240c-21.08 0-41.41-1-60.89-2.7 8.06-26.13 32.15-45.3 60.89-45.3s52.83 19.17 60.89 45.3C329.41 239 309.08 240 288 240zm64-144c0 35.29-28.71 64-64 64s-64-28.71-64-64V82.96c20.4-1.88 41.8-2.96 64-2.96s43.6 1.08 64 2.96V96zm46.93 134.9c-13.81-9.91-23.94-23.9-28.4-40.54-5.11-19.06-2.49-38.96 7.38-56.04l25.65-44.42C477.72 103.5 528 129.79 528 160c0 30.83-52.4 57.54-129.07 70.9z"};
    TextButton helpButton {"svg:M20 424.229h20V279.771H20c-11.046 0-20-8.954-20-20V212c0-11.046 8.954-20 20-20h112c11.046 0 20 8.954 20 20v212.229h20c11.046 0 20 8.954 20 20V492c0 11.046-8.954 20-20 20H20c-11.046 0-20-8.954-20-20v-47.771c0-11.046 8.954-20 20-20zM96 0C56.235 0 24 32.235 24 72s32.235 72 72 72 72-32.235 72-72S135.764 0 96 0z"};
    TextButton newsButton {"svg:M128.081 415.959c0 35.369-28.672 64.041-64.041 64.041S0 451.328 0 415.959s28.672-64.041 64.041-64.041 64.04 28.673 64.04 64.041zm175.66 47.25c-8.354-154.6-132.185-278.587-286.95-286.95C7.656 175.765 0 183.105 0 192.253v48.069c0 8.415 6.49 15.472 14.887 16.018 111.832 7.284 201.473 96.702 208.772 208.772.547 8.397 7.604 14.887 16.018 14.887h48.069c9.149.001 16.489-7.655 15.995-16.79zm144.249.288C439.596 229.677 251.465 40.445 16.503 32.01 7.473 31.686 0 38.981 0 48.016v48.068c0 8.625 6.835 15.645 15.453 15.999 191.179 7.839 344.627 161.316 352.465 352.465.353 8.618 7.373 15.453 15.999 15.453h48.068c9.034-.001 16.329-7.474 16.005-16.504z"};
    TextButton updateButton {"svg:M537.6 226.6c4.1-10.7 6.4-22.4 6.4-34.6 0-53-43-96-96-96-19.7 0-38.1 6-53.3 16.2C367 64.2 315.3 32 256 32c-88.4 0-160 71.6-160 160 0 2.7.1 5.4.2 8.1C40.2 219.8 0 273.2 0 336c0 79.5 64.5 144 144 144h368c70.7 0 128-57.3 128-128 0-61.9-44-113.6-102.4-125.4zm-132.9 88.7L299.3 420.7c-6.2 6.2-16.4 6.2-22.6 0L171.3 315.3c-10.1-10.1-2.9-27.3 11.3-27.3H248V176c0-8.8 7.2-16 16-16h48c8.8 0 16 7.2 16 16v112h65.4c14.2 0 21.4 17.2 11.3 27.3z"};

    String additionalProgramming;
    String updateUrl;
    String newsUrl;

    PatchBrowser patchBrowser { slProc };

    void refreshPrograms();
};
