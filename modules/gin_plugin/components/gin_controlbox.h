#pragma once

//==============================================================================
/** A box for holding controls on a grid
*/
class ControlBox : public MultiParamComponent
{
public:
    ControlBox (ProcessorEditor& e) : editor (e)
    {
    }

    juce::Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1)
    {
        auto cx = editor.getGridWidth();
        auto cy = editor.getGridHeight();
        return juce::Rectangle<int> (x * cx, y * cy, w * cx, h * cy);
    }

    void add (Component* c)
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
    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::white.withAlpha (0.2f));
        g.drawRect (getLocalBounds());
    }

    ProcessorEditor& editor;

    juce::OwnedArray<Component> controls;
    std::map<Component*, int> pages;
};

//==============================================================================
/** A control for holding pags of controls on a grid
    Pages slide over top of each other to make room
*/
class PagedControlBox : public MultiParamComponent
{
public:
    using ParamComponentPtr = ParamComponent*;
    
    PagedControlBox (ProcessorEditor& e)
        : editor (e)
    {
        addAndMakeVisible (cover);
        pagesOpen[0] = 1.0f;
    }

    void addPage (const juce::String& name, int rows, int cols)
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

        watchParam (p);
    }

    void addBottomButton (int pageIdx, Component* c)
    {
        auto page = pages[pageIdx];
        page->addBottomButton (c);
    }

    void addControl (int pageIdx, Component* c, int x, int y, int cx = 1, int cy = 1)
    {
        auto page = pages[pageIdx];
        c->setBounds (page->getGridArea (editor, x, y, cx, cy));
        page->controls.add (c);
        page->controlsParent.addAndMakeVisible (c);
    }
    
    void setPageOpen (int page, bool open)
    {
        pagesOpen[page] = open ? 1.0f : 0.0f;
    }

protected:
    //==============================================================================
    void paramChanged() override
    {
        MultiParamComponent::paramChanged();

        for (auto p : pages)
            p->paramChanged();
    }
    
private:
    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::white.withAlpha (0.2f));
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

    //==============================================================================
    struct PageComponent : public juce::Component,
                           private juce::Timer
    {
        PageComponent (PagedControlBox& o, const juce::String& n)
            : owner (o), name (n)
        {
            setOpaque (true);
            addAndMakeVisible (controlsParent);
            controlsParent.setInterceptsMouseClicks (false, true);
        }

        void resized() override
        {
            controlsParent.setBounds (getLocalBounds());
        }

        void paint (juce::Graphics& g) override
        {
            g.fillAll (juce::Colours::black);

            auto rc = getLocalBounds().removeFromLeft (15);
            g.setColour (juce::Colours::white.withAlpha (0.1f));
            g.fillRect (rc);

            g.setColour (juce::Colours::black);
            g.fillRect (rc.removeFromRight (1));

            g.setColour (juce::Colours::white);

            juce::Graphics::ScopedSaveState state (g);
            g.addTransform (juce::AffineTransform::rotation (-juce::MathConstants<float>::halfPi,
                                                       7.5f, getHeight() * 0.5f));

            g.setFont (11);
            auto rcText = juce::Rectangle<float> (-getHeight() / 2.0f, getHeight () / 2.0f - 9.5f, (float) getHeight (), 15.0f );
            g.drawText (name, rcText, juce::Justification::centred);
        }

        juce::Rectangle<int> getGridArea (ProcessorEditor& e, int x, int y, int w = 1, int h = 1)
        {
            auto cx = e.getGridWidth();
            auto cy = e.getGridHeight();
            return juce::Rectangle<int> (x * cx + 15, y * cy, w * cx, h * cy);
        }

        void mouseDown (const juce::MouseEvent& e) override
        {
            auto rc = getLocalBounds().removeFromLeft (15);
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
            owner.pagesOpen[pageIdx] = juce::jlimit (0.0f, 1.0f, owner.pagesOpen[pageIdx] + delta);

            if (juce::approximatelyEqual (owner.pagesOpen[pageIdx], 0.0f) || juce::approximatelyEqual (owner.pagesOpen[pageIdx], 1.0f))
                stopTimer();

            owner.resized();
        }

        void setEnable (gin::Parameter::Ptr enable_)
        {
            enable = enable_;

            enableButton = std::make_unique<PowerButton> (enable);
            addAndMakeVisible (*enableButton);

            auto rc = juce::Rectangle<int> (2, 2, 11, 11);
            enableButton->setBounds (rc);
        }

        void paramChanged()
        {
            controlsParent.setEnabled (enable == nullptr ? true : enable->isOn());
        }

        void addBottomButton (Component* c)
        {
            controlsParent.addAndMakeVisible (c);
            bottomButtons.add (c);

            auto rc = getLocalBounds().removeFromLeft (15);
            for (auto b : bottomButtons)
                b->setBounds (rc.removeFromBottom (15).withSizeKeepingCentre (11, 11));
        }

        PagedControlBox& owner;
        juce::String name;

        gin::Parameter::Ptr enable = nullptr;
        std::unique_ptr<PowerButton> enableButton;

        juce::Component controlsParent;
        juce::OwnedArray<juce::Component> controls, bottomButtons;
        bool opening = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageComponent)
    };

    //==============================================================================
    struct Cover : public Component
    {
        Cover()                                 { setOpaque (true);                 }
        void paint (juce::Graphics& g) override { g.fillAll (juce::Colours::black); }
    };

    //==============================================================================
    ProcessorEditor& editor;

    Cover cover;
    juce::OwnedArray<PageComponent> pages;

    std::map<int, float> pagesOpen;
};
