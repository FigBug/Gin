#pragma once

//==============================================================================
/** An alert window that doesn't add itself to the desktop. Safer for use in
    plugins.
*/
class PluginAlertWindow : public juce::AlertWindow
{
public:
    PluginAlertWindow (const juce::String& title,
                       const juce::String& message,
                       AlertIconType iconType,
                       Component* associatedComponent = nullptr)
        : AlertWindow (title, message, iconType, associatedComponent)
    {
        removeFromDesktop ();
    }

   #if JUCE_MODAL_LOOPS_PERMITTED
    int runModalLoop (juce::Component& parent)
    {
        std::optional<juce::Rectangle<int>> originalSize;
        
        auto rc = getLocalBounds();
        if (rc.getWidth() > parent.getWidth() || rc.getHeight() > parent.getHeight())
        {
            originalSize = parent.getBounds();

            auto w = std::max (rc.getWidth()  + 50, getWidth());
            auto h = std::max (rc.getHeight() + 50, getHeight());
            
            parent.setSize (w, h);
        }
        
        blur = std::make_unique<BlurryComp> (parent.createComponentSnapshot (parent.getLocalBounds()));
        blur->setAlwaysOnTop (true);
        blur->setBounds (parent.getLocalBounds());
        parent.addAndMakeVisible (*blur);

        blur->addChildComponent (this);
        setBounds (blur->getLocalBounds().withSizeKeepingCentre (getWidth(), getHeight()));

        setDropShadowEnabled (false);
        int ret = AlertWindow::runModalLoop();

        blur->removeChildComponent (blur.get());
        blur = nullptr;
        
        if (originalSize.has_value())
            parent.setSize (originalSize->getWidth(), originalSize->getHeight());

        setVisible (false);

        return ret;
    }
   #endif

    void runAsync (juce::Component& parent, std::function<void (int)> callback)
    {
        std::optional<juce::Rectangle<int>> originalSize;
        
        auto rc = getLocalBounds();
        if (rc.getWidth() > parent.getWidth() || rc.getHeight() > parent.getHeight())
        {
            originalSize = parent.getBounds();

            auto w = std::max (rc.getWidth()  + 50, getWidth());
            auto h = std::max (rc.getHeight() + 50, getHeight());
            
            parent.setSize (w, h);
        }
        
        blur = std::make_unique<BlurryComp> (parent.createComponentSnapshot (parent.getLocalBounds()));
        blur->setAlwaysOnTop (true);
        blur->setBounds (parent.getLocalBounds());
        parent.addAndMakeVisible (*blur);

        blur->addChildComponent (this);
        setBounds (blur->getLocalBounds().withSizeKeepingCentre (getWidth(), getHeight()));

        setDropShadowEnabled (false);

        enterModalState (true, new Callback ([this, callback, originalSize, &parent] (int ret)
        {
            blur->removeChildComponent (blur.get());
            blur = nullptr;

            setVisible (false);
            
            if (originalSize.has_value())
                parent.setSize (originalSize->getWidth(), originalSize->getHeight());

            callback (ret);
        }));
    }

private:
    class Callback : public juce::ModalComponentManager::Callback
    {
    public:
        Callback (std::function<void (int)> callback_)
            : callback (callback_)
        {

        }

        void modalStateFinished (int returnValue) override
        {
            callback (returnValue);
        }

        std::function<void (int)> callback;
    };

    class BlurryComp : public juce::Component
    {
    public:
        BlurryComp (juce::Image img) : background (img)
        {
            applyStackBlur (img, 4);
        }

        void paint (juce::Graphics& g) override
        {
            g.drawImage (background, getLocalBounds().toFloat());
        }

        juce::Image background;
    };

    std::unique_ptr<BlurryComp> blur;
};
