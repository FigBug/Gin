#pragma once

/** A header with power button, name and mod destinations
 */
 class HeaderItem : public juce::Component
{
public:
    HeaderItem (juce::String name_, Parameter* enable, ModMatrix& mm, ModSrcId monoSrc = {}, ModSrcId polySrc = {})
        : name (name_)
    {
        if (enable)
        {
            power = std::make_unique<PowerButton> (enable);
            addAndMakeVisible (*power);
        }
        if (monoSrc.isValid())
        {
            monoSource = std::make_unique<ModulationSourceButton>(mm, monoSrc, false);
            addAndMakeVisible (*monoSource);
        }
        if (polySrc.isValid())
        {
            polySource = std::make_unique<ModulationSourceButton> (mm, polySrc, true);
            addAndMakeVisible (*polySource);
        }
    }

    HeaderItem (juce::String name_, Parameter* enable = nullptr)
        : name (name_)
    {
        if (enable)
        {
            power = std::make_unique<PowerButton> (enable);
            addAndMakeVisible (*power);
        }
    }

    void setSelected (bool s)
    {
        selected = s;
        repaint();
    }

    std::function<void()> onClick;

private:
    void mouseDown (const juce::MouseEvent&) override
    {
        if (onClick)
        {
            onClick();
        }
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour (findColour (GinLookAndFeel::whiteColourId).withAlpha (selected ? 0.9f : 0.3f));
        g.drawRoundedRectangle (getLocalBounds().toFloat(), getHeight() / 2.0f, 1.0f);

        g.setFont (14.0f);
        g.drawText (name, getLocalBounds(), juce::Justification::centred, true);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (5, 0);
        r = r.withSizeKeepingCentre (r.getWidth(), 11);

        if (power)
        {
            power->setBounds (r.removeFromLeft (11));
        }

        if (polySource)
        {
            polySource->setBounds (r.removeFromRight (11));
            r.removeFromRight (3);
        }
        if (monoSource)
        {
            monoSource->setBounds (r.removeFromRight (11));
        }
    }

    std::unique_ptr<PowerButton> power;
    std::unique_ptr<ModulationSourceButton> monoSource;
    std::unique_ptr<ModulationSourceButton> polySource;

    bool selected = false;
    juce::String name;
};

/** Holds a row of headers
*/
class HeaderRow : public juce::Component
{
public:
    HeaderRow ()
    {
    }

    void addItem (HeaderItem& item)
    {
        addAndMakeVisible (item);
        items.add (&item);
    }

private:
    void resized() override
    {
        using juce::operator""_px;
        using juce::operator""_fr;
        
        juce::Grid grid;
        grid.columnGap = 3_px;

        for (int i = 0; i < items.size(); i++)
            grid.templateColumns.add (juce::Grid::TrackInfo ( 1_fr ));

        grid.templateRows = { juce::Grid::TrackInfo ( 1_fr ) };

        for ( auto& i : items )
            grid.items.add ( juce::GridItem ( i ) );

        grid.performLayout ( getLocalBounds ().reduced (4, 3) );
    }
    
    juce::Array<HeaderItem*> items;
};
