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
        auto c = findColour (GinLookAndFeel::accentColourId).withAlpha (getToggleState() ? 0.9f : 0.4f);
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
        auto p = getPath();
        g.fillPath (p, p.getTransformToScaleToFit (rc, true));
    }

    const juce::Path& getPath()
    {
        static const unsigned char pathData[] = { 110,109,0,0,192,67,0,0,0,66,108,0,0,128,66,0,0,0,66,98,51,51,229,65,0,0,0,66,0,0,0,0,154,153,114,66,0,0,0,0,0,0,192,66,108,0,0,0,0,0,0,208,67,98,0,0,0,0,205,172,225,67,51,51,229,65,0,0,240,67,0,0,128,66,0,0,240,67,108,0,0,192,67,0,0,240,67,98,205,172,
        209,67,0,0,240,67,0,0,224,67,205,172,225,67,0,0,224,67,0,0,208,67,108,0,0,224,67,0,0,192,66,98,0,0,224,67,154,153,114,66,205,172,209,67,0,0,0,66,0,0,192,67,0,0,0,66,99,109,0,0,96,67,0,0,144,67,98,123,84,78,67,0,0,144,67,0,0,64,67,195,213,136,67,0,0,64,
        67,0,0,128,67,98,0,0,64,67,122,84,110,67,123,84,78,67,0,0,96,67,0,0,96,67,0,0,96,67,98,133,171,113,67,0,0,96,67,0,0,128,67,123,84,110,67,0,0,128,67,0,0,128,67,98,0,0,128,67,194,213,136,67,133,171,113,67,0,0,144,67,0,0,96,67,0,0,144,67,99,101,0,0 };

        static juce::Path path;

        if (path.isEmpty())
            path.loadPathFromData (pathData, sizeof (pathData));

        return path;
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
/** A list box of al mod sources
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

        juce::TextButton deleteButton {"svg:M432 32H312l-9.4-18.7A24 24 0 0 0 281.1 0H166.8a23.72 23.72 0 0 0-21.4 13.3L136 32H16A16 16 0 0 0 0 48v32a16 16 0 0 0 16 16h416a16 16 0 0 0 16-16V48a16 16 0 0 0-16-16zM53.2 467a48 48 0 0 0 47.9 45h245.8a48 48 0 0 0 47.9-45L416 128H32z"};
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
