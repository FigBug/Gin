/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/**
    Base class for property components that use juce::Value for data binding.

    PropertyComponentBase extends JUCE's PropertyComponent to automatically
    sync with a Value object, handling the listener registration and refresh
    logic. Derived classes only need to implement the refresh() method to
    update their UI when the value changes.

    @see FilePropertyComponent, ColourPropertyComponent
*/
class PropertyComponentBase : public juce::PropertyComponent,
                              private juce::Value::Listener
{
public:
    PropertyComponentBase (const juce::Value& valueToControl, const juce::String& propertyName)
      : PropertyComponent (propertyName), value (valueToControl)
    {
        value.addListener (this);
    }

    juce::Value& getValueObject() {
        return value;
    }

protected:
    void valueChanged (juce::Value&) override
    {
        refresh();
    }

    juce::Value value;
};

//==============================================================================
/**
    Property component for selecting files with a browse button.

    FilePropertyComponent provides a file path editor with browse and clear
    buttons. It displays the selected file path and allows users to browse
    for files using a native file chooser dialog.

    Usage:
    @code
    Value filePathValue;
    FilePropertyComponent fileProp(filePathValue, "Audio File", "Open Audio",  "*.wav;*.mp3;*.aif");
    propertyPanel.addProperty(&fileProp);
    @endcode

    @see PropertyComponentBase, juce::FileChooser
*/
class FilePropertyComponent : public PropertyComponentBase
{
public:
    FilePropertyComponent (const juce::Value& valueToControl, const juce::String& propertyName, const juce::String& title_ = "Open", const juce::String pattern_ = "*.*")
      : PropertyComponentBase (valueToControl, propertyName), title (title_), pattern (pattern_)
    {
        addAndMakeVisible (container);

        container.browse.onClick = [this]
        {
            box = std::make_unique<juce::FileChooser> (title, juce::File (value.toString()), pattern);

            auto folderChooserFlags = juce::FileBrowserComponent::openMode;

            box->launchAsync (folderChooserFlags, [this] (const juce::FileChooser&)
            {
                value.setValue (box->getResult().getFullPathName());
            });
        };

        container.clear.onClick = [this] { value.setValue (""); };
    }

    void refresh() override
    {
        container.filename.setText (value.toString());
    }

private:
    class Container : public Component
    {
    public:
        Container()
        {
            addAndMakeVisible (filename);
            addAndMakeVisible (browse);
            addAndMakeVisible (clear);

            filename.setReadOnly (true);
        }

        void resized() override
        {
            auto rc = getLocalBounds();
            clear.setBounds (rc.removeFromRight (rc.getHeight()));
            browse.setBounds (rc.removeFromRight (rc.getHeight()));
            rc.removeFromRight (3);
            filename.setBounds (rc);
        }

        juce::TextEditor filename;
        juce::TextButton browse {"..."};
        juce::TextButton clear {"X"};
    };

    Container container;

    juce::String title, pattern;

    std::unique_ptr<juce::FileChooser> box;
};

//==============================================================================
/**
    Property component for selecting colors with a visual color picker.

    ColourPropertyComponent displays a color swatch that shows the current
    color and opens a color selector popup when clicked. The color value is
    stored as a string (via Colour::toString()) in the bound Value object.

    Usage:
    @code
    Value colorValue("ff0080ff"); // ARGB hex string
    ColourPropertyComponent colorProp(colorValue, "Background Color", true); // true = show alpha
    propertyPanel.addProperty(&colorProp);
    @endcode

    @see PropertyComponentBase, juce::ColourSelector
*/
class ColourPropertyComponent : public PropertyComponentBase
{
public:
    ColourPropertyComponent (const juce::Value& valueToControl, const juce::String& propertyName, bool showAlpha = false)
      : PropertyComponentBase (valueToControl, propertyName), container (value, showAlpha)
    {
        addAndMakeVisible (container);
    }

    void refresh() override
    {
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        PropertyComponent::paint (g);

        g.setColour (findColour (juce::BooleanPropertyComponent::backgroundColourId));
        g.fillRect (container.getBounds());

        g.setColour (findColour (juce::BooleanPropertyComponent::outlineColourId));
        g.drawRect (container.getBounds());
    }

private:
    class ColourSelectorEx : public juce::ColourSelector
    {
    public:
        ColourSelectorEx (int flags = (showAlphaChannel | showColourAtTop | showSliders | showColourspace),
                          int edgeGap = 4,
                          int gapAroundColourSpaceComponent = 7)
            : juce::ColourSelector ( flags, edgeGap, gapAroundColourSpaceComponent )
        {
        }

        ~ColourSelectorEx() override
        {
            if (onDismiss)
                onDismiss ();

        }

        std::function<void ()> onDismiss;
    };

    class Container : public Component
    {
    public:
        Container (juce::Value& value_, bool a)
          : value (value_), alpha (a)
        {
        }

        void paint (juce::Graphics& g) override
        {
            juce::Colour c = juce::Colour::fromString (value.toString());

            g.setColour (c);
            g.fillRect (getLocalBounds().reduced (4));

            g.setColour (c.contrasting());
            g.drawText (c.toDisplayString (alpha), getLocalBounds(), juce::Justification::centred);
        }

        void mouseUp (const juce::MouseEvent& e) override
        {
            if (e.mouseWasClicked())
            {
                auto colourSelector = std::make_unique<ColourSelectorEx> (juce::ColourSelector::showColourAtTop | juce::ColourSelector::showSliders | juce::ColourSelector::showColourspace);

                colourSelector->setSize (300, 280);
                colourSelector->setCurrentColour (juce::Colour::fromString (value.toString()), juce::dontSendNotification);
                colourSelector->onDismiss = [this, cs = colourSelector.get()]()
                {
                    value = cs->getCurrentColour().toString();
                };

                juce::CallOutBox::launchAsynchronously (std::move (colourSelector), getScreenBounds(), nullptr);
            }
        }

        juce::Value& value;
        bool alpha;
    };

    Container container;
};
