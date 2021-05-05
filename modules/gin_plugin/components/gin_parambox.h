#pragma once

//==============================================================================
inline void gradientRect (juce::Graphics& g, juce::Rectangle<int> rc, juce::Colour c1, juce::Colour c2)
{
    juce::ColourGradient gradient (c1, (float) rc.getX(), (float) rc.getY(), c2, (float) rc.getX(), (float) rc.getBottom(), false);

    g.setGradientFill (gradient);
    g.fillRect (rc);
}

//==============================================================================
/** A header with optional power button
 */
class ParamHeader : public juce::Component
{
public:
    ParamHeader (const juce::String& name_)
        : name (name_)
    {

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
        addAndMakeVisible (header);
    }

    void addEnable (gin::Parameter::Ptr)
    {
    }

    void addControl (Component* c, int x, int y, int cx = 1, int cy = 1)
    {
        c->setBounds (getGridArea (x, y, cx, cy));
        controls.add (c);
        addAndMakeVisible (c);
    }

    void addControl (Component* c, float x, float y, float cx = 1.0f, float cy = 1.0f)
    {
        c->setBounds (getGridArea (x, y, cx, cy));
        controls.add (c);
        addAndMakeVisible (c);
    }

private:
    void paint (juce::Graphics& g) override
    {
        auto rc = getLocalBounds().withTrimmedTop (23);
        gradientRect (g, rc, findColour (PluginLookAndFeel::matte1ColourId), findColour (PluginLookAndFeel::matte2ColourId));
    }

    void resized() override
    {
        header.setBounds (getLocalBounds().removeFromTop (23));
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
    juce::OwnedArray<Component> controls;
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
