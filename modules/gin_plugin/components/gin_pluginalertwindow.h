#pragma once

//==============================================================================
/** An alert window that doesn't add itself to the desktop. Safer for use in
    plugins.
*/
class PluginAlertWindow : public AlertWindow
{
public:
    PluginAlertWindow (const String& title,
                       const String& message,
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
        BlurryComp (Image img) : background (img)
        {
            applyStackBlur (img, 4);
        }

        void paint (Graphics& g) override
        {
            g.drawImage (background, getLocalBounds().toFloat());
        }

        Image background;
    };

    std::unique_ptr<BlurryComp> blur;
};
