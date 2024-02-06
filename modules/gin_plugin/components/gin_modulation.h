#pragma once

//==============================================================================
/** A button for enabling modulation learn
*/
class ModulationSourceButton : public juce::Button,
                               private ModMatrix::Listener
{
public:
    ModulationSourceButton (ModMatrix& mm, ModSrcId src = {}, bool poly_ = false)
        : Button (""), modMatrix (mm), source (src), poly (poly_)
    {
        setName ("modSrc");
        modMatrix.addListener (this);
        update();
    }

	ModulationSourceButton (const juce::String& name, ModMatrix& mm, ModSrcId src = {}, bool poly_ = false)
		: Button (""), modMatrix (mm), source (src), poly (poly_)
	{
		setName (name);
		modMatrix.addListener (this);
		update();
	}

    ~ModulationSourceButton() override
    {
        modMatrix.removeListener (this);
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        dragging = false;
        juce::Button::mouseDown (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        juce::Button::mouseDrag (e);
        if (e.mouseWasDraggedSinceMouseDown() && ! dragging)
        {
            dragging = true;

            if (auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this))
                dnd->startDragging ("modSrc" + juce::String (source.id), this);
        }
    }

    void setSource (ModSrcId src, bool p)
    {
        source = src;
        poly = p;
        update();
    }

private:
    void update()
    {
        if (source.isValid())
            setTooltip ("Mod Source: " + modMatrix.getModSrcName (source));
        else
            setTooltip ({});

        learnSourceChanged (modMatrix.getLearn());
    }

    void clicked() override
    {
        if (! dragging)
        {
            if (modMatrix.getLearn() == source)
                modMatrix.disableLearn();
            else
                modMatrix.enableLearn (source);
        }
    }

    void learnSourceChanged (ModSrcId src) override
    {
        setToggleState (src == source, juce::dontSendNotification);
    }

    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        auto c = findColour (getToggleState() ? GinLookAndFeel::whiteColourId : GinLookAndFeel::accentColourId).withAlpha (0.9f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);

        g.setColour (c.withMultipliedAlpha (isEnabled() ? 1.0f : 0.5f));

        auto rc = getLocalBounds().toFloat();
        auto& p = poly ? getPolyPath() : getMonoPath();
        g.fillPath (p, p.getTransformToScaleToFit (rc, true));
    }

    const juce::Path& getMonoPath()
    {
        juce::SharedResourcePointer<Images> images;
        return images->monoPath;
    }

    const juce::Path& getPolyPath()
    {
        juce::SharedResourcePointer<Images> images;
        return images->polyPath;
    }

    ModMatrix& modMatrix;
    ModSrcId source = {};
    bool poly = false;
    bool dragging = false;
};

//==============================================================================
/** A button for the modulation destination
*/
class ModulationDepthSlider : public juce::Slider
{
public:
    ModulationDepthSlider() : juce::Slider (RotaryHorizontalVerticalDrag, NoTextBox)
    {

    }

    ~ModulationDepthSlider() override
    {
    }

    std::function<void ()> onClick;

private:
    void paint (juce::Graphics& g) override
    {
        auto c = juce::Colours::white.withAlpha (0.4f);
        g.setColour (c);

        auto rc = getLocalBounds().toFloat().reduced (1.5f);
        g.fillEllipse (rc);

        if (auto v = float (getValue()); v > 0.0f || v < 0.0f)
        {
            g.setColour (findColour (PluginLookAndFeel::accentColourId, true).withAlpha (0.9f));

            juce::Path p;
            p.addPieSegment (rc, 0.0f, juce::MathConstants<float>::pi * 2 * v, 0.0f);

            g.fillPath (p);
        }
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (e.mouseWasClicked() && e.mods.isPopupMenu() && onClick)
            onClick ();
    }
};

//==============================================================================

/** A button and text readout that shows the current modulation source
*/
class ModulationOverview : public juce::Component,
                           private ModMatrix::Listener,
                           private juce::Timer
{
public:
    ModulationOverview (ModMatrix& mm)
        : modMatrix (mm)
    {
        modMatrix.addListener (this);
        learnSourceChanged (modMatrix.getLearn ());

        addAndMakeVisible (button);
        
        name.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (name);
    }

    ~ModulationOverview() override
    {
        modMatrix.removeListener (this);
    }

private:
    void visibilityChanged() override
    {
        if (isVisible())
            startTimerHz (60);
        else
            stopTimer();
        
        timerCallback();
    }
    
    void timerCallback() override
    {
        auto phase = float (std::fmod (juce::Time::getMillisecondCounterHiRes() / 2000.0f, 1.0f));
        
        auto c1 = findColour (GinLookAndFeel::whiteColourId);
        auto c2 = findColour (GinLookAndFeel::accentColourId);
        
        if (phase < 0.5f)
            phase *= 2.0f;
        else
            phase = 1.0f - ((phase - 0.5f) * 2.0f);
        
        auto col = c1.overlaidWith (c2.withAlpha (phase));
        
        button.setColour (GinLookAndFeel::whiteColourId, col);
        name.setColour (juce::Label::textColourId, col);
    }
    
    void mouseUp (const juce::MouseEvent& e) override
    {
        if (e.mouseWasClicked())
            modMatrix.disableLearn();
    }
    
    void resized() override
    {
        auto rc = getLocalBounds();
        button.setBounds (rc.removeFromLeft (rc.getHeight()));
        rc.removeFromLeft (4);
        name.setBounds (rc);
    }

    void learnSourceChanged (ModSrcId src) override
    {
        setVisible (src.isValid());
        button.setSource (src, ! src.isValid() ? false : modMatrix.getModSrcPoly (src));

        if (src.isValid())
            name.setText (modMatrix.getModSrcName (src), juce::dontSendNotification);
        else
            name.setText ({}, juce::dontSendNotification);
    }

    ModMatrix& modMatrix;

    ModulationSourceButton button { modMatrix, {} };
    juce::Label name;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationOverview)
};

//==============================================================================
/** A list box of all mod sources
*/
class ModSrcListBox : public juce::ListBox,
                      private juce::ListBoxModel
{
public:
    ModSrcListBox (ModMatrix& m)
        : modMatrix (m)
    {
        setName ("modlist");
        setModel (this);
        updateContent();
        setRowHeight (16);
    }

    int getNumRows() override
    {
        return modMatrix.getNumModSources();
    }

    void paintListBoxItem (int, juce::Graphics&, int, int, bool) override {}

    Component* refreshComponentForRow (int row, bool, Component* c) override
    {
        auto rowComponent = (Row*)c;
        if (rowComponent == nullptr)
            rowComponent = new Row (modMatrix);

        rowComponent->update (row);
        return rowComponent;
    }

private:
    class Row : public Component
    {
    public:
        Row (ModMatrix& m)
            : modMatrix (m)
        {
            addAndMakeVisible (text);
            addAndMakeVisible (src);
        }

        void update (int idx)
        {
            text.setText (modMatrix.getModSrcName (ModSrcId (idx)), juce::dontSendNotification);
            src.setSource (ModSrcId (idx), modMatrix.getModSrcPoly (ModSrcId (idx)));
        }

        void resized() override
        {
            auto rc = getLocalBounds().reduced (2);
            src.setBounds (rc.removeFromRight (rc.getHeight()));
            rc.removeFromRight (4);
            text.setBounds (rc);
        }

        ModMatrix& modMatrix;

        juce::Label text;
        ModulationSourceButton src { modMatrix };
    };

    ModMatrix& modMatrix;
};

//==============================================================================
/** A button that displays mod curve
*/
class ModCurveButton : public juce::Button
{
public:
    ModCurveButton() : juce::Button ("")
    {
    }
    
    void setCurve (ModMatrix::Function c)
    {
        curve = c;
        repaint();
    }
    
    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        auto c = findColour (getToggleState() ? GinLookAndFeel::whiteColourId : GinLookAndFeel::accentColourId).withAlpha (0.9f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);
        
        g.setColour (c.withMultipliedAlpha (isEnabled() ? 1.0f : 0.5f));
        
        auto rc = getLocalBounds().toFloat().reduced (1.0f);
        
        juce::Path p;
        for (auto x = 0.0f; x <= rc.getWidth(); x += 0.5f)
        {
            auto y = (1.0f - ModMatrix::shape (x / rc.getWidth(), curve, false)) * rc.getHeight();
            
            if (x == 0.0f)
                p.startNewSubPath (rc.getX() + x, rc.getY() + y);
            else
                p.lineTo (rc.getX() + x, rc.getY() + y);
        }
        
        g.strokePath (p, juce::PathStrokeType (1.0f));
    }
    
private:
    ModMatrix::Function curve;
};

//==============================================================================
/** A list box of all assigned
*/
class ModMatrixBox : public juce::ListBox,
                     private juce::ListBoxModel,
                     private ModMatrix::Listener
{
public:
    ModMatrixBox (gin::Processor& p, ModMatrix& m, int dw = 50)
        : proc (p), modMatrix (m), depthWidth (dw)
    {
        setName ("matrix");
        setModel (this);
        setRowHeight (16);
        refresh();

        modMatrix.addListener (this);
    }

    ~ModMatrixBox() override
    {
        modMatrix.removeListener (this);
    }

private:
    void refresh()
    {
        assignments.clear();

        auto& params = proc.getPluginParameters();
        for (gin::Parameter* p : params)
            for (auto s : modMatrix.getModSources (p))
                assignments.add ({s, p});
        
        updateContent();
        repaint();
    }

    int getNumRows() override
    {
        return assignments.size();
    }

    void paintListBoxItem (int, juce::Graphics&, int, int, bool) override {}

    juce::Component* refreshComponentForRow (int row, bool, juce::Component* c) override
    {
        auto rowComponent = dynamic_cast<Row*>(c);
        if (rowComponent == nullptr)
            rowComponent = new Row (*this);

        rowComponent->update (row);
        return rowComponent;
    }

    void modMatrixChanged() override
    {
        refresh();
    }

    class Row : public juce::Component,
                private juce::Slider::Listener
    {
    public:
        Row (ModMatrixBox& o)
            : owner (o)
        {
            addAndMakeVisible (enableButton);
            addAndMakeVisible (deleteButton);
            addAndMakeVisible (curveButton);
            addAndMakeVisible (depth);
            addAndMakeVisible (src);
            addAndMakeVisible (dst);

            depth.setRange (-1.0, 1.0, 0.01);
            depth.getProperties().set ("fromCentre", true);
            depth.addListener (this);
            depth.setSliderSnapsToMousePosition (false);
            depth.setMouseDragSensitivity (750);
            depth.setPopupDisplayEnabled (true, true, findParentComponentOfClass<juce::AudioProcessorEditor>());
            depth.setDoubleClickReturnValue (true, 0.0);

            enableButton.onClick = [this]
            {
                if (row >= 0 && row < owner.assignments.size())
                {
                    auto& a = owner.assignments.getReference (row);
                    
                    auto e = owner.modMatrix.getModEnable (a.src, ModDstId (a.dst->getModIndex()));
                    owner.modMatrix.setModEnable (a.src, ModDstId (a.dst->getModIndex()), ! e);
                    enableButton.setToggleState (! e, juce::dontSendNotification);
                }
            };

            deleteButton.onClick = [this]
            {
                if (row >= 0 && row < owner.assignments.size())
                {
                    auto& a = owner.assignments.getReference (row);
                    owner.modMatrix.clearModDepth (a.src, ModDstId (a.dst->getModIndex()));
                }
            };
            
            curveButton.onClick = [this]
            {
                if (row >= 0 && row < owner.assignments.size())
                {
                    auto& a = owner.assignments.getReference (row);
                    auto f = owner.modMatrix.getModFunction (a.src, ModDstId (a.dst->getModIndex()));
                    
                    auto set = [this] (ModMatrix::Function func)
                    {
                        auto& aa = owner.assignments.getReference (row);
                        owner.modMatrix.setModFunction (aa.src, ModDstId (aa.dst->getModIndex()), func);
                    };
                    
                    juce::PopupMenu m;
                    
                    m.addItem ("Linear", true, f == ModMatrix::Function::linear, [set] { set (ModMatrix::Function::linear); });
                    m.addItem ("Quadratic In", true, f == ModMatrix::Function::quadraticIn, [set] { set (ModMatrix::Function::quadraticIn); });
                    m.addItem ("Quadratic In/Out", true, f == ModMatrix::Function::quadraticInOut, [set] { set (ModMatrix::Function::quadraticInOut); });
                    m.addItem ("Quadratic Out", true, f == ModMatrix::Function::quadraticOut, [set] { set (ModMatrix::Function::quadraticOut); });
                    m.addItem ("Sine In", true, f == ModMatrix::Function::sineIn, [set] { set (ModMatrix::Function::sineIn); });
                    m.addItem ("Sine In Out", true, f == ModMatrix::Function::sineInOut, [set] { set (ModMatrix::Function::sineInOut); });
                    m.addItem ("Sine Out", true, f == ModMatrix::Function::sineOut, [set] { set (ModMatrix::Function::sineOut); });
                    m.addItem ("Exponential In", true, f == ModMatrix::Function::exponentialIn, [set] { set (ModMatrix::Function::exponentialIn); });
                    m.addItem ("Exponential In/Out", true, f == ModMatrix::Function::exponentialInOut, [set] { set (ModMatrix::Function::exponentialInOut); });
                    m.addItem ("Exponential Out", true, f == ModMatrix::Function::exponentialOut, [set] { set (ModMatrix::Function::exponentialOut); });
                    m.addSeparator();
                    m.addItem ("Inv Linear", true, f == ModMatrix::Function::invLinear, [set] { set (ModMatrix::Function::invLinear); });
                    m.addItem ("Inv Quadratic In", true, f == ModMatrix::Function::invQuadraticIn, [set] { set (ModMatrix::Function::invQuadraticIn); });
                    m.addItem ("Inv Quadratic In/Out", true, f == ModMatrix::Function::invQuadraticInOut, [set] { set (ModMatrix::Function::invQuadraticInOut); });
                    m.addItem ("Inv Quadratic Out", true, f == ModMatrix::Function::invQuadraticOut, [set] { set (ModMatrix::Function::invQuadraticOut); });
                    m.addItem ("Inv Sine In", true, f == ModMatrix::Function::invSineIn, [set] { set (ModMatrix::Function::invSineIn); });
                    m.addItem ("Inv Sine In/Out", true, f == ModMatrix::Function::invSineInOut, [set] { set (ModMatrix::Function::invSineInOut); });
                    m.addItem ("Inv Sine Out", true, f == ModMatrix::Function::invSineOut, [set] { set (ModMatrix::Function::invSineOut); });
                    m.addItem ("Inv Exponential In", true, f == ModMatrix::Function::invExponentialIn, [set] { set (ModMatrix::Function::invExponentialIn); });
                    m.addItem ("Inv Exponential In/Out", true, f == ModMatrix::Function::invExponentialInOut, [set] { set (ModMatrix::Function::invExponentialInOut); });
                    m.addItem ("Inv Exponential Out", true, f == ModMatrix::Function::invExponentialOut, [set] { set (ModMatrix::Function::invExponentialOut); });
                    
                    m.showMenuAsync ({});
                }
            };
        }

        void sliderValueChanged (juce::Slider*) override
        {
            if (row >= 0 && row < owner.assignments.size())
            {
                auto& a = owner.assignments.getReference (row);
                owner.modMatrix.setModDepth (a.src, ModDstId (a.dst->getModIndex()), (float) depth.getValue());
            }
        }

        void update (int idx)
        {
            row = idx;

            if (idx >= 0 && idx < owner.assignments.size())
            {
                auto& a = owner.assignments.getReference (idx);
                src.setText (owner.modMatrix.getModSrcName (a.src), juce::dontSendNotification);
                dst.setText (a.dst->getName (100), juce::dontSendNotification);

                auto e = owner.modMatrix.getModEnable (a.src, ModDstId (a.dst->getModIndex()));
                enableButton.setToggleState (e, juce::dontSendNotification);

                depth.setValue (owner.modMatrix.getModDepth (a.src, ModDstId (a.dst->getModIndex())));
                curveButton.setCurve (owner.modMatrix.getModFunction (a.src, ModDstId (a.dst->getModIndex())));
            }
            else
            {
                src.setText ("", juce::dontSendNotification);
                dst.setText ("", juce::dontSendNotification);
                curveButton.setCurve (ModMatrix::Function::linear);
            }
        }

        void resized() override
        {
            auto rc = getLocalBounds().reduced (2);

            int h = rc.getHeight();

            enableButton.setBounds (rc.removeFromLeft (h));
            deleteButton.setBounds (rc.removeFromRight (h));
            rc.removeFromLeft (2);
            depth.setBounds (rc.removeFromLeft (owner.depthWidth));
            curveButton.setBounds (rc.removeFromLeft (h));

            int w = rc.getWidth() / 2;
            src.setBounds (rc.removeFromLeft (w));
            dst.setBounds (rc.removeFromLeft (w));
        }

        ModMatrixBox& owner;
        int row = 0;

        juce::Slider depth { juce::Slider::LinearHorizontal, juce::Slider::NoTextBox };

        juce::Label src;
        juce::Label dst;
        
        ModCurveButton curveButton;

        SVGButton enableButton { "enable", Assets::power };
        SVGButton deleteButton { "delete", Assets::del };
    };

    struct Assignment
    {
        ModSrcId src = {};
        Parameter* dst = nullptr;
    };

    gin::Processor& proc;
    gin::ModMatrix& modMatrix;
    juce::Array<Assignment> assignments;
    int depthWidth = 50;
};
