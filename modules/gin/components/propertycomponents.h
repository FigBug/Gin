/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
class PropertyComponentBase : public PropertyComponent,
                              private Value::Listener
{
public:
    PropertyComponentBase (const Value& valueToControl, const String& propertyName)
      : PropertyComponent (propertyName), value (valueToControl)
    {
        value.addListener (this);
    }

protected:
    void valueChanged (Value&) override
    {
        refresh();
    }

    Value value;
};

//==============================================================================*/
class FilePropertyComponent : public PropertyComponentBase
{
public:
    FilePropertyComponent (const Value& valueToControl, const String& propertyName, const String& title_ = "Open", const String pattern_ = "*.*")
      : PropertyComponentBase (valueToControl, propertyName), title (title_), pattern (pattern_)
    {
        addAndMakeVisible (container);

        container.browse.onClick = [this]
        {
            FileChooser box (title, File (value.toString()), pattern);

            if (box.browseForFileToOpen())
                value.setValue (box.getResult().getFullPathName());
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

        TextEditor filename;
        TextButton browse {"..."};
        TextButton clear {"X"};
    };

    Container container;

    String title, pattern;
};

//==============================================================================*/
class ColourPropertyComponent : public PropertyComponentBase
{
public:
    ColourPropertyComponent (const Value& valueToControl, const String& propertyName, bool showAlpha = false)
      : PropertyComponentBase (valueToControl, propertyName), container (value, showAlpha)
    {
        addAndMakeVisible (container);
    }

    void refresh() override
    {
        repaint();
    }

    void paint (Graphics& g) override
    {
        PropertyComponent::paint (g);

        g.setColour (findColour (BooleanPropertyComponent::backgroundColourId));
        g.fillRect (container.getBounds());

        g.setColour (findColour (BooleanPropertyComponent::outlineColourId));
        g.drawRect (container.getBounds());
    }

private:
    class Container : public Component
    {
    public:
        Container (Value& value_, bool a)
          : value (value_), alpha (a)
        {
        }

        void paint (Graphics& g) override
        {
            Colour c = Colour::fromString (value.toString());

            g.setColour (c);
            g.fillRect (getLocalBounds().reduced (4));

            g.setColour (c.contrasting());
            g.drawText (c.toDisplayString (alpha), getLocalBounds(), Justification::centred);
        }

        void mouseUp (const MouseEvent& e) override
        {
            if (e.mouseWasClicked())
            {
                ColourSelector colourSelector (ColourSelector::showColourAtTop | ColourSelector::showSliders | ColourSelector::showColourspace);

                colourSelector.setSize (300, 280);
                colourSelector.setCurrentColour (Colour::fromString (value.toString()), dontSendNotification);

                CallOutBox callOut (colourSelector, getScreenBounds(), nullptr);
                callOut.runModalLoop();

                value = colourSelector.getCurrentColour().toString();
            }
        }

        Value& value;
        bool alpha;
    };

    Container container;
};
