#pragma once

//==============================================================================
class PowerButton : public PluginButton
{
public:
    PowerButton (Parameter* p) : PluginButton (p)
    {
    }

private:
    void paintButton (Graphics& g, bool over, bool down) override
    {
        auto c = getToggleState() ? Colours::white.withAlpha (0.9f) : Colours::white.withAlpha (0.4f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);

        g.setColour (c);

        auto rc = getLocalBounds().toFloat();
        auto& p = getPath();
        g.fillPath (p, p.getTransformToScaleToFit (rc, true));
    }

    const Path& getPath()
    {
        static const unsigned char pathData[] = { 110,109,0,0,208,68,0,0,96,68,113,0,0,208,68,0,128,131,68,0,96,200,68,0,64,149,68,113,0,192,192,68,0,0,167,68,0,224,179,68,0,224,179,68,113,0,0,167,68,0,192,192,68,0,64,149,68,0,96,200,68,113,0,128,131,68,0,0,208,68,0,0,96,68,0,0,208,68,113,0,0,57,68,
        0,0,208,68,0,128,21,68,0,96,200,68,113,0,0,228,67,0,192,192,68,0,128,176,67,0,224,179,68,113,0,0,122,67,0,0,167,68,0,0,61,67,0,64,149,68,113,0,0,0,67,0,128,131,68,0,0,0,67,0,0,96,68,113,0,0,0,67,0,128,50,68,0,128,80,67,0,64,10,68,113,0,128,144,67,0,0,
        196,67,0,128,217,67,0,128,141,67,113,0,0,239,67,0,0,123,67,0,160,4,68,0,0,129,67,113,0,192,17,68,0,128,132,67,0,128,25,68,0,0,154,67,113,0,128,33,68,0,0,175,67,0,160,31,68,0,64,201,67,113,0,192,29,68,0,128,227,67,0,64,19,68,0,128,243,67,113,0,128,245,
        67,0,64,12,68,0,192,218,67,0,0,39,68,113,0,0,192,67,0,192,65,68,0,0,192,67,0,0,96,68,113,0,0,192,67,0,0,122,68,0,64,212,67,0,208,136,68,113,0,128,232,67,0,160,148,68,0,128,5,68,0,64,157,68,113,0,192,22,68,0,224,165,68,0,96,46,68,0,240,170,68,113,0,0,
        70,68,0,0,176,68,0,0,96,68,0,0,176,68,113,0,0,122,68,0,0,176,68,0,208,136,68,0,240,170,68,113,0,160,148,68,0,224,165,68,0,64,157,68,0,64,157,68,113,0,224,165,68,0,160,148,68,0,240,170,68,0,208,136,68,113,0,0,176,68,0,0,122,68,0,0,176,68,0,0,96,68,113,
        0,0,176,68,0,192,65,68,0,80,169,68,0,0,39,68,113,0,160,162,68,0,64,12,68,0,96,150,68,0,128,243,67,113,0,32,145,68,0,128,227,67,0,48,144,68,0,64,201,67,113,0,64,143,68,0,0,175,67,0,64,147,68,0,0,154,67,113,0,32,151,68,0,128,132,67,0,192,157,68,0,0,129,
        67,113,0,96,164,68,0,0,123,67,0,160,169,68,0,128,141,67,113,0,224,187,68,0,0,196,67,0,240,197,68,0,64,10,68,113,0,0,208,68,0,128,50,68,0,0,208,68,0,0,96,68,99,109,0,0,128,68,0,0,0,67,108,0,0,128,68,0,0,64,68,113,0,0,128,68,0,0,77,68,0,128,118,68,0,128,
        86,68,113,0,0,109,68,0,0,96,68,0,0,96,68,0,0,96,68,113,0,0,83,68,0,0,96,68,0,128,73,68,0,128,86,68,113,0,0,64,68,0,0,77,68,0,0,64,68,0,0,64,68,108,0,0,64,68,0,0,0,67,113,0,0,64,68,0,0,152,66,0,128,73,68,0,0,24,66,113,0,0,83,68,0,0,0,0,0,0,96,68,0,0,0,
        0,113,0,0,109,68,0,0,0,0,0,128,118,68,0,0,24,66,113,0,0,128,68,0,0,152,66,0,0,128,68,0,0,0,67,99,101,0,0 };

        static Path path;

        if (path.isEmpty())
            path.loadPathFromData (pathData, sizeof (pathData));

        return path;
    }
};

//==============================================================================
class ControlBox : public Component
{
public:
    using ParamComponentPtr = ParamComponent*;

    ControlBox (GinAudioProcessorEditor& e) : editor (e)
    {
    }

    Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1)
    {
        auto cx = editor.getGridWidth();
        auto cy = editor.getGridHeight();
        return Rectangle<int> (x * cx, y * cy, w * cx, h * cy);
    }

    void add (ParamComponent* c)
    {
        controls.add (c);
        addAndMakeVisible (c);
        pages[c] = 0;
    }

    void add (int page, ParamComponent* c)
    {
        controls.add (c);
        addAndMakeVisible (c);
        pages[c] = page;
    }

    void setPage (int page)
    {
        for (auto itr : pages)
            itr.first->setVisible (itr.second == page);
    }

private:
    void paint (Graphics& g) override
    {
        g.setColour (Colours::white.withAlpha (0.2f));
        g.drawRect (getLocalBounds());
    }

    GinAudioProcessorEditor& editor;

    OwnedArray<ParamComponent> controls;
    std::map<ParamComponent*, int> pages;
};

//==============================================================================
class PagedControlBox : public Component
{
public:
    PagedControlBox (GinAudioProcessorEditor& e)
        : editor (e)
    {
        addAndMakeVisible (cover);
        pagesOpen[0] = 1.0f;
    }

    void addPage (const String& name, int rows, int cols)
    {
        auto cx = editor.getGridWidth();
        auto cy = editor.getGridHeight();

        auto p = new PageComponent (*this, name);
        p->setSize (cx * rows + 15, cy * cols);
        addAndMakeVisible (p);
        pages.add (p);

        cover.toFront (false);
    }

    void addPageEnable (int pageIdx, gin::Parameter::Ptr p)
    {
        auto page = pages[pageIdx];
        page->setEnable (p);
    }

    void addControl (int pageIdx, ParamComponent* c, int x, int y, int cx = 1, int cy = 1)
    {
        auto page = pages[pageIdx];
        c->setBounds (page->getGridArea (editor, x, y, cx, cy));
        page->controls.add (c);
        page->addAndMakeVisible (c);
    }

private:
    void paint (Graphics& g) override
    {
        g.setColour (Colours::white.withAlpha (0.2f));
        g.drawRect (getLocalBounds());
    }

    void resized () override
    {
        int x = 0;
        for (int i = 0; i < pages.size(); i++)
        {
            auto page = pages[i];
            page->setTopLeftPosition (x, 0);

            float state = i == pages.size() - 1 ? 1.0f : pagesOpen[i];
            if (state == 0.0f)
                x += 15;
            else
                x += int (15 + (page->getWidth() - 15) * easeSineInOut (state));
        }

        if (x < getWidth())
            cover.setBounds (x, 0, getWidth() - x, getHeight());
        else
            cover.setBounds ({});
    }

    struct PageComponent : public Component,
                           private Timer
    {
        PageComponent (PagedControlBox& o, const String& n)
            : owner (o), name (n)
        {
            setOpaque (true);
        }

        void paint (Graphics& g) override
        {
            g.fillAll (Colours::black);

            auto rc = getLocalBounds().removeFromLeft (15.0f);
            g.setColour (Colours::white.withAlpha (0.1f));
            g.fillRect (rc);

            g.setColour (Colours::black);
            g.fillRect (rc.removeFromRight (1));

            g.setColour (Colours::white);

            Graphics::ScopedSaveState state (g);
            g.addTransform (AffineTransform::rotation (-MathConstants<float>::halfPi,
                                                       7.5f, getHeight() * 0.5f));

            g.setFont (11);
            auto rcText = Rectangle<float> (-getHeight() / 2.0f, getHeight () / 2.0f - 9.5f, getHeight (), 15.0f );
            g.drawText (name, rcText, Justification::centred);
        }

        Rectangle<int> getGridArea (GinAudioProcessorEditor& e, int x, int y, int w = 1, int h = 1)
        {
            auto cx = e.getGridWidth();
            auto cy = e.getGridHeight();
            return Rectangle<int> (x * cx + 15, y * cy, w * cx, h * cy);
        }

        void mouseDown (const MouseEvent& e) override
        {
            auto rc = getLocalBounds().removeFromLeft (15.0f);
            if (rc.contains (e.getPosition()))
            {
                startTimerHz (30);

                int pageIdx = owner.pages.indexOf (this);
                opening = owner.pagesOpen[pageIdx] < 0.5f;
            }
        }

        void timerCallback() override
        {
            int pageIdx = owner.pages.indexOf (this);
            auto delta = opening ? 1.0f / 0.333f / 30.0f : - 1.0f / 0.333f / 30.0f;
            owner.pagesOpen[pageIdx] = jlimit (0.0f, 1.0f, owner.pagesOpen[pageIdx] + delta);

            if (owner.pagesOpen[pageIdx] == 0 || owner.pagesOpen[pageIdx] == 1)
                stopTimer();

            owner.resized();
        }

        void setEnable (gin::Parameter::Ptr enable)
        {
            enableButton = std::make_unique<PowerButton> (enable);
            addAndMakeVisible (*enableButton);

            auto rc = Rectangle<int> (2, 2, 11, 11);
            enableButton->setBounds (rc);
        }

        PagedControlBox& owner;
        String name;

        std::unique_ptr<PowerButton> enableButton;
        OwnedArray<ParamComponent> controls;
        bool opening = false;
    };

    struct Cover : public Component
    {
        Cover()                             { setOpaque (true);             }
        void paint (Graphics& g) override   { g.fillAll (Colours::black);   }
    };

    GinAudioProcessorEditor& editor;

    Cover cover;
    OwnedArray<PageComponent> pages;

    std::map<int, float> pagesOpen;
};
