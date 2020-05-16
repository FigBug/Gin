#pragma once

//==============================================================================
class ProcessorEditor;
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
class ProcessorEditorBase : public AudioProcessorEditor
{
public:
    ProcessorEditorBase (Processor& p)
      : AudioProcessorEditor (p), proc (p)
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

    ComponentBoundsConstrainer resizeLimits;

private:
    Processor& proc;
    std::unique_ptr<ResizableCornerComponent> resizer;
};

//==============================================================================
class ProcessorEditor : public ProcessorEditorBase,
                        protected Button::Listener,
                        protected ComboBox::Listener
{
public:
    ProcessorEditor (Processor&, int cx = 100, int cy = 100) noexcept;
    ~ProcessorEditor() override;

    void updateReady (String updateUrl);
    void newsReady (String newsUrl);

    Processor& slProc;

    virtual Rectangle<int> getControlsArea();
    virtual Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1);
    Rectangle<int> getFullGridArea();

    int getGridWidth()  { return cx; }
    int getGridHeight() { return cy; }
    
protected:
    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* b) override;
    void comboBoxChanged (ComboBox* c) override;

    void setGridSize (int x, int y, int extraWidthPx = 0, int extraHeightPx = 0 );

    const int cx;
    const int cy;

    int headerHeight = 60, inset = 4;
    int cols = 0, rows = 0, extraWidthPx = 0, extraHeightPx = 0;

    std::unique_ptr<UpdateChecker> updateChecker;
    std::unique_ptr<NewsChecker> newsChecker;

    OwnedArray<ParamComponent> controls;

    ParamComponent* componentForId (const String& uid);
    ParamComponent* componentForParam (Parameter& param);

    ComboBox programs;
    TextButton addButton {"A"};
    TextButton deleteButton {"D"};
    TextButton socaButton {"S"};
    TextButton helpButton {"H"};
    TextButton newsButton {"N"};
    TextButton updateButton {"U"};

    SharedResourcePointer<TooltipWindow> tooltipWindow;

    String additionalProgramming;
    String updateUrl;
    String newsUrl;

    void refreshPrograms();
};
