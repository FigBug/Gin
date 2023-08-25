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
/** A box for knobs with a header
*/
class ParamBox : public MultiParamComponent
{
public:
    ParamBox (const juce::String& name)
        : header (name)
    {
        setName (name);
        addAndMakeVisible (frame);
        addAndMakeVisible (header);
    }

    void setTitle (const juce::String& name)
    {
        header.setTitle (name);
    }

    void addEnable (gin::Parameter::Ptr p)
    {
        enableParam = p;

        const auto icon = "M6 0H5V6H6V0ZM0 5.50001C0 3.36266 1.21916 1.51007 3 0.599716V1.75778C1.79401 2.56504 1 3.9398 1 5.50001C1 7.98529 3.01472 10 5.5 10C7.98528 10 10 7.98529 10 5.50001C10 3.9398 9.20599 2.56504 8 1.75778V0.599716C9.78084 1.51007 11 3.36266 11 5.50001C11 8.53757 8.53757 11 5.5 11C2.46243 11 0 8.53757 0 5.50001Z";

        auto b = new SVGPluginButton (p, icon);
        b->setBounds (6, 6, 12, 12);
        controls.add (b);
        addAndMakeVisible (b);

        watchParam (p);
    }

    void addControl (Component* c, int x, int y, int cx = 1, int cy = 1)
    {
        c->setBounds (getGridArea (x, y, cx, cy));
        controls.add (c);
        frame.addAndMakeVisible (c);
    }

    void addControl (Component* c, float x, float y, float cx = 1.0f, float cy = 1.0f)
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
            frame.setEnabled (enableParam->getUserValue() > 0.0f);
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
    juce::OwnedArray<Component> controls;
    gin::Parameter::Ptr enableParam = nullptr;
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
