/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
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

//==============================================================================*/
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

//==============================================================================*/
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
