#pragma once

//==============================================================================
/** A button for enabling modulation learn
*/
class ModulationSourceButton : public juce::Button,
                               private IModMatrix::Listener
{
public:
    ModulationSourceButton (IModMatrix& mm, ModSrcId src = {}, bool poly_ = false)
        : Button (""), modMatrix (mm), source (src), poly (poly_)
    {
        setName ("modSrc");
        modMatrix.addListener (this);
        update();
    }

    ModulationSourceButton (const juce::String& name, IModMatrix& mm, ModSrcId src = {}, bool poly_ = false)
        : Button (""), modMatrix (mm), source (src), poly (poly_)
    {
        setName (name);
        modMatrix.addListener (this);
        update();
    }

    ~ModulationSourceButton() override
    {
        modMatrix.removeListener (this);
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        dragging = false;
        juce::Button::mouseDown (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        juce::Button::mouseDrag (e);
        if (isEnabled() && e.mouseWasDraggedSinceMouseDown() && ! dragging)
        {
            dragging = true;

            if (auto dnd = juce::DragAndDropContainer::findParentDragContainerFor (this))
                dnd->startDragging ("modSrc" + juce::String (source.id), this);
        }
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        dragging = false;
    }

    void setSource (ModSrcId src, bool p)
    {
        source = src;
        poly = p;
        update();
    }

    ModSrcId getSource()
    {
        return source;
    }

    bool isDragging()
    {
        return dragging;
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
        if (! dragging)
        {
            if (modMatrix.getLearn() == source)
                modMatrix.disableLearn();
            else
                modMatrix.enableLearn (source);
        }
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

        g.setColour (c.withMultipliedAlpha (isEnabled() ? 1.0f : 0.5f));

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

    IModMatrix& modMatrix;
    ModSrcId source = {};
    bool poly = false;
    bool dragging = false;
};
