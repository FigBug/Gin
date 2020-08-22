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

    int runModalLoop (Component& parent)
    {
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

        setVisible (false);

        return ret;
    }

private:
    class BlurryComp : public Component
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
