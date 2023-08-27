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
        modMatrix.addListener (this);
        update();
    }

    ~ModulationSourceButton() override
    {
        modMatrix.removeListener (this);
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
        if (modMatrix.getLearn() == source)
            modMatrix.disableLearn();
        else
            modMatrix.enableLearn (source);
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

        g.setColour (c);

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
};

//==============================================================================
/** A button for the modulation destination
*/
class ModulationDestinationButton : public juce::Button
{
public:
    ModulationDestinationButton() : juce::Button ("")
    {
    }

    ~ModulationDestinationButton() override
    {
    }

private:
    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        auto c = getToggleState() ? juce::Colours::white.withAlpha (0.9f) : juce::Colours::white.withAlpha (0.4f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);

        g.setColour (c);

        auto rc = getLocalBounds().toFloat();
        g.fillEllipse (rc);
    }
};

//==============================================================================

/** A button and text readout that shows the current modulation source
*/
class ModulationOverview : public juce::Component,
                           private ModMatrix::Listener
{
public:
    ModulationOverview (ModMatrix& mm)
        : modMatrix (mm)
    {
        modMatrix.addListener (this);
        learnSourceChanged (modMatrix.getLearn ());

        addAndMakeVisible (button);
        addAndMakeVisible (name);
    }

    ~ModulationOverview() override
    {
        modMatrix.removeListener (this);
    }

private:
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
/** A list box of all assigned
*/
class ModMatrixBox : public juce::ListBox,
                     private juce::ListBoxModel,
                     private ModMatrix::Listener
{
public:
    ModMatrixBox (gin::Processor& p, ModMatrix& m)
        : proc (p), modMatrix (m)
    {
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

    juce::Component* refreshComponentForRow (int row, bool, Component* c) override
    {
        auto rowComponent = (Row*)c;
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
            addAndMakeVisible (deleteButton);
            addAndMakeVisible (depth);
            addAndMakeVisible (src);
            addAndMakeVisible (dst);

            depth.setRange (-1.0, 1.0, juce::dontSendNotification);
            depth.getProperties().set ("fromCentre", true);
            depth.addListener (this);

            deleteButton.onClick = [this]
            {
                auto& a = owner.assignments.getReference (row);
                owner.modMatrix.clearModDepth (a.src, ModDstId (a.dst->getModIndex()));
            };
        }

        void sliderValueChanged (juce::Slider*) override
        {
            auto& a = owner.assignments.getReference (row);
            owner.modMatrix.setModDepth (a.src, ModDstId (a.dst->getModIndex()), (float) depth.getValue());
        }

        void update (int idx)
        {
            row = idx;

            if (idx >= 0 && idx < owner.assignments.size())
            {
                auto& a = owner.assignments.getReference (idx);
                src.setText (owner.modMatrix.getModSrcName (a.src), juce::dontSendNotification);
                dst.setText (a.dst->getName (100), juce::dontSendNotification);

                depth.setValue (owner.modMatrix.getModDepth (a.src, ModDstId (a.dst->getModIndex())));
            }
            else
            {
                src.setText ("", juce::dontSendNotification);
                dst.setText ("", juce::dontSendNotification);
            }
        }

        void resized() override
        {
            auto rc = getLocalBounds().reduced (2);

            int h = rc.getHeight();

            deleteButton.setBounds (rc.removeFromLeft (h));
            rc.removeFromLeft (2);
            depth.setBounds (rc.removeFromLeft (50));

            int w = rc.getWidth() / 2;
            src.setBounds (rc.removeFromLeft (w));
            dst.setBounds (rc.removeFromLeft (w));
        }

        ModMatrixBox& owner;
        int row = 0;

        juce::Slider depth { juce::Slider::LinearHorizontal, juce::Slider::NoTextBox };

        juce::Label src;
        juce::Label dst;

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
};
