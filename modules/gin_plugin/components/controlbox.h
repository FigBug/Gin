#pragma once

class ControlBox : public Component
{
public:
    using ParamComponentPtr = ParamComponent*;

    ControlBox()
    {
    }

    Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1)
    {
        if (auto p = findParentComponentOfClass<GinAudioProcessorEditor>())
        {
            auto rc = p->getGridArea (0, 0);
            int cx = rc.getWidth();
            int cy = rc.getHeight();
            return Rectangle<int> (x * cx, y * cy, w * cx, h * cy);
        }

        return {};
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

    OwnedArray<ParamComponent> controls;
    std::map<ParamComponent*, int> pages;
};
