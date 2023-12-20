#pragma once

//==============================================================================
inline void gradientRect (juce::Graphics& g, juce::Rectangle<int> rc, juce::Colour c1, juce::Colour c2)
{
    juce::ColourGradient gradient (c1, (float) rc.getX(), (float) rc.getY(), c2, (float) rc.getX(), (float) rc.getBottom(), false);

    g.setGradientFill (gradient);
    g.fillRect (rc);
}

//==============================================================================
/** A header with title text
 */
class ParamHeader : public juce::Component
{
public:
    ParamHeader (const juce::String& name_)
        : name (name_)
    {
        setName (name);
    }

    void setTitle (const juce::String& name_)
    {
        name = name_;
        repaint();
    }

private:
    void paint (juce::Graphics& g)
    {
        auto rc = getLocalBounds();
        gradientRect (g, rc, findColour (PluginLookAndFeel::title1ColourId), findColour (PluginLookAndFeel::title2ColourId));

        g.setColour (findColour (PluginLookAndFeel::whiteColourId).withAlpha (0.15f));
        g.fillRect (rc.removeFromTop (1));

        auto f = juce::Font ().withPointHeight (10.0).withExtraKerningFactor (0.25);

        g.setColour (findColour (PluginLookAndFeel::whiteColourId).withAlpha (0.6f));
        g.drawText (name.toUpperCase(), getLocalBounds(), juce::Justification::centred);
    }

    juce::String name;
};

//==============================================================================
/** A header button with title text
 */
class HeaderButton : public juce::Button,
                     public juce::DragAndDropTarget,
                     private juce::Timer
{
public:
    HeaderButton (const juce::String& name_)
        : juce::Button (name_)
    {
    }
    
    bool isInterestedInDragSource (const SourceDetails&) override
    {
        return true;
    }
    
    void itemDragEnter (const SourceDetails&) override
    {
        startTimer (300);
    }
    
    void itemDragExit (const SourceDetails&) override
    {
        stopTimer();
    }
    
    void itemDropped (const SourceDetails&) override
    {
        stopTimer();
    }
    
    void timerCallback() override
    {
        triggerClick();
    }

private:
    void paintButton (juce::Graphics& g, bool, bool) override
    {
        auto f = juce::Font ().withPointHeight (10.0).withExtraKerningFactor (0.25);

        g.setColour (getToggleState() ? findColour (PluginLookAndFeel::accentColourId).withAlpha (0.6f) : findColour (PluginLookAndFeel::whiteColourId).withAlpha (0.6f));
        g.drawText (getButtonText().toUpperCase(), getLocalBounds(), juce::Justification::centred);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderButton)
};


//==============================================================================
/** A box for knobs with a header
*/
class ParamBox : public MultiParamComponent
{
public:
    ParamBox (const juce::String& name)
        : header (name)
    {
        setName (name);
        addAndMakeVisible (header);
        addAndMakeVisible (frame);

        frame.setInterceptsMouseClicks (false, true);
    }

    void setTitle (const juce::String& name)
    {
        header.setTitle (name);
    }

    void addHeader (const juce::StringArray names, int idx, gin::Parameter::Ptr p)
    {
        setTitle ({});

        for (auto num = 0; auto n : names)
        {
            auto h = new HeaderButton (n);
            h->onClick = [this, num]
            {
                headerParam->setUserValue (float (num));
            };
            header.addAndMakeVisible (h);
            headers.add (h);
            num++;
        }

        watchParam (p);

        headerIndex = idx;
        headerParam = p;

        paramChanged();
    }

    void addEnable (gin::Parameter::Ptr p)
    {
        enableParam = p;

        auto b = new SVGPluginButton (p, Assets::power);
        b->setBounds (6, 6, 12, 12);
        controls.add (b);
        addAndMakeVisible (b);

        watchParam (p);
    }

    void addModSource (juce::Component* c)
    {
        c->setSize (12, 12);
        modSources.add (c);
        addAndMakeVisible (c);
    }

    void addControl (juce::Component* c)
    {
        controls.add (c);
        frame.addAndMakeVisible (c);
    }

    void addControl (juce::Component* p, juce::Component* c)
    {
        controls.add (c);
        p->addAndMakeVisible (c);
    }

    void addControl (juce::Component* c, int x, int y, int cx = 1, int cy = 1)
    {
        c->setBounds (getGridArea (x, y, cx, cy));
        controls.add (c);
        frame.addAndMakeVisible (c);
    }

    void addControl (juce::Component* c, float x, float y, float cx = 1.0f, float cy = 1.0f)
    {
        c->setBounds (getGridArea (x, y, cx, cy));
        controls.add (c);
        frame.addAndMakeVisible (c);
    }

    ParamHeader& getHeader() { return header; }

protected:
    void paramChanged () override
    {
        MultiParamComponent::paramChanged();

        if (enableParam)
        {
            frame.setEnabled (enableParam->getUserValue() > 0.0f);
            for (auto ms : modSources)
                ms->setEnabled (enableParam->getUserValue() > 0.0f);
        }
        if (headerParam)
        {
            setVisible (headerParam->getUserValueInt() == headerIndex);
            for (auto i = 0; auto h : headers)
            {
                h->setToggleState (i == headerParam->getUserValueInt(), juce::dontSendNotification);
                i++;
            }
        }
    }

    void paint (juce::Graphics& g) override
    {
        auto rc = getLocalBounds().withTrimmedTop (23);
        gradientRect (g, rc, findColour (PluginLookAndFeel::matte1ColourId), findColour (PluginLookAndFeel::matte2ColourId));
    }

    void resized() override
    {
        header.setBounds (getLocalBounds().removeFromTop (23));
        frame.setBounds (getLocalBounds());

        auto rc = header.getLocalBounds ().withSizeKeepingCentre (header.getWidth() - 6, 12);

        for (auto c : modSources)
        {
            c->setBounds (rc.removeFromRight (12));
            rc.removeFromRight (4);
        }

        if (headers.size() > 0)
        {
            auto w = 50;
            auto rh = header.getLocalBounds().withSizeKeepingCentre (w * headers.size(), header.getHeight());

            for (auto h : headers)
                h->setBounds (rh.removeFromLeft (w));
        }
    }

    juce::Rectangle<int> getGridArea (int x, int y, int cx = 1, int cy = 1)
    {
        return { x * 56, 23 + y * 70, cx * 56, cy * 70 };
    }

    juce::Rectangle<int> getGridArea (float x, float y, float cx = 1.0f, float cy = 1.0f)
    {
        return { int (x * 56), int (23 + y * 70), int (cx * 56), int (cy * 70) };
    }

    ParamHeader header;
    juce::Component frame;
    juce::OwnedArray<Component> controls, modSources;
    gin::Parameter::Ptr enableParam = nullptr;

    juce::OwnedArray<HeaderButton> headers;
    gin::Parameter::Ptr headerParam = nullptr;
    int headerIndex = 0;
};

//==============================================================================
/** A box for graphs without a header
 */
class ParamArea : public MultiParamComponent
{
public:
    ParamArea (const juce::String& name_ = {})
        : name (name_)
    {
    }

    void addControl (Component* c, int x, int y, int cx = 1, int cy = 1)
    {
        c->setBounds (getGridArea (x, y, cx, cy));
        controls.add (c);
        addAndMakeVisible (c);
    }

    void addControl (Component* c)
    {
        controls.add (c);
        addAndMakeVisible (c);
    }

private:
    void paint (juce::Graphics& g) override
    {
        auto rc = getLocalBounds();
        gradientRect (g, rc, findColour (PluginLookAndFeel::glass1ColourId), findColour (PluginLookAndFeel::glass2ColourId));

        g.setColour (findColour (PluginLookAndFeel::whiteColourId).withAlpha (0.15f));
        g.fillRect (rc.removeFromTop (1));

        if (name.isNotEmpty())
        {
            g.setColour (findColour (PluginLookAndFeel::grey45ColourId));
            g.drawText (name.toUpperCase(), getLocalBounds().removeFromTop (23), juce::Justification::centred);
        }
    }

    juce::Rectangle<int> getGridArea (int x, int y, int cx = 1, int cy = 1)
    {
        return { x * 56, 23 + y * 70, cx * 56, cy * 70 };
    }

    juce::OwnedArray<Component> controls;
    juce::String name;
};

//==============================================================================
/** A group of control boxes
 */
class BoxArea : public juce::Component
{
public:
    BoxArea()
    {
    }

    void addBox (Component* c)
    {
        pageMap[c] = 0;
        boxes.add (c);
        addAndMakeVisible (c);
        resized();
    }

    void addBox (int page, Component* c)
    {
        pageMap[c] = page;
        boxes.add (c);
        addAndMakeVisible (c);
        resized();
    }

    void setPage (int page)
    {
        for (auto itr : pageMap)
            itr.first->setVisible (itr.second == page);

        resized();
    }

private:
    void resized() override
    {
        auto rc = juce::Rectangle<int> (0, 0, 1000000, getHeight());
        for (auto c : boxes)
        {
            if (c->isVisible())
            {
                c->setBounds (rc.removeFromLeft (c->getWidth()));
                rc.removeFromLeft (1);
            }
        }
    }

    juce::Array<Component*> boxes;
    std::map<Component*, int> pageMap;
};
