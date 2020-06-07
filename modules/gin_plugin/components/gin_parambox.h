#pragma once

//==============================================================================
inline void gradientRect (Graphics& g, Rectangle<int> rc, Colour c1, Colour c2)
{
    ColourGradient gradient (c1, (float) rc.getX(), (float) rc.getY(), c2, (float) rc.getX(), (float) rc.getBottom(), false);

    g.setGradientFill (gradient);
    g.fillRect (rc);
}

//==============================================================================
/** A header with optional power button
 */
class ParamHeader : public Component
{
public:
    ParamHeader (const String& name_)
        : name (name_)
    {

    }

private:
    void paint (Graphics& g) override
    {
        auto rc = getLocalBounds();
        gradientRect (g, rc, findColour (PluginLookAndFeel::title1ColourId), findColour (PluginLookAndFeel::title2ColourId));

        g.setColour (findColour (PluginLookAndFeel::whiteColourId).withAlpha (0.15f));
        g.fillRect (rc.removeFromTop (1));

        auto f = Font ().withPointHeight (10.0).withExtraKerningFactor (0.25);

        g.setColour (findColour (PluginLookAndFeel::whiteColourId).withAlpha (0.6f));
        g.drawText (name.toUpperCase(), getLocalBounds(), Justification::centred);
    }

    String name;
};

//==============================================================================
/** A box for knobs with a header
*/
class ParamBox : public MultiParamComponent
{
public:
    ParamBox (const String& name)
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
    void paint (Graphics& g) override
    {
        auto rc = getLocalBounds().withTrimmedTop (23);
        gradientRect (g, rc, findColour (PluginLookAndFeel::matte1ColourId), findColour (PluginLookAndFeel::matte2ColourId));
    }

    void resized() override
    {
        header.setBounds (getLocalBounds().removeFromTop (23));
    }

    Rectangle<int> getGridArea (int x, int y, int cx = 1, int cy = 1)
    {
        return { x * 56, 23 + y * 70, cx * 56, cy * 70 };
    }

    Rectangle<int> getGridArea (float x, float y, float cx = 1.0f, float cy = 1.0f)
    {
        return { int (x * 56), int (23 + y * 70), int (cx * 56), int (cy * 70) };
    }

    ParamHeader header;
    OwnedArray<Component> controls;
};

//==============================================================================
/** A box for graphs without a header
 */
class ParamArea : public MultiParamComponent
{
public:
    ParamArea (const String& name_ = {})
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
    void paint (Graphics& g) override
    {
        auto rc = getLocalBounds();
        gradientRect (g, rc, findColour (PluginLookAndFeel::glass1ColourId), findColour (PluginLookAndFeel::glass2ColourId));

        g.setColour (findColour (PluginLookAndFeel::whiteColourId).withAlpha (0.15f));
        g.fillRect (rc.removeFromTop (1));

        if (name.isNotEmpty())
        {
            g.setColour (findColour (PluginLookAndFeel::grey45ColourId));
            g.drawText (name.toUpperCase(), getLocalBounds().removeFromTop (23), Justification::centred);
        }
    }

    Rectangle<int> getGridArea (int x, int y, int cx = 1, int cy = 1)
    {
        return { x * 56, 23 + y * 70, cx * 56, cy * 70 };
    }

    OwnedArray<Component> controls;
    String name;
};

//==============================================================================
/** A group of control boxes
 */
class BoxArea : public Component
{
public:
    BoxArea()
    {
    }

    void addBox (Component* c)
    {
        boxes.add (c);
        addAndMakeVisible (c);
        resized();
    }

private:
    void resized() override
    {
        auto rc = Rectangle<int> (0, 0, 1000000, getHeight());
        for (auto c : boxes)
        {
            if (c->isVisible())
            {
                c->setBounds (rc.removeFromLeft (c->getWidth()));
                rc.removeFromLeft (1);
            }
        }
    }

    Array<Component*> boxes;
};
