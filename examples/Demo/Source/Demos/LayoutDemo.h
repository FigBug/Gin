/*
  ==============================================================================

    LayoutDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
inline juce::String layoutTxt = R"~~~(
{
   "components": [
       {
           "id": "a",
           "x": 10,
           "y": 10,
           "w": "parW - 20",
           "h": "(parH - 40) / 3",
           "components": [
               {
                   "id": "e",
                   "x": "10",
                   "y": "10",
                   "r": "parW - 10",
                   "h": "parH / 3"
               },
               {
                   "id": "f",
                   "x": "10",
                   "b": "parH - 10",
                   "r": "parW - 10",
                   "h": "parH / 3"
               }
           ]
       },
       {
           "id": "b",
           "x": 10,
           "y": "prevB + 10",
           "w": "parW - 20",
           "h": "(parH - 40) / 3",
           "components": [
               {
                   "id": "g",
                   "x": 10,
                   "y": 10,
                   "w": "parW / 3",
                   "b": "parH - 10"
               },
               {
                   "id": "h",
                   "r": "parW - 10",
                   "y": 10,
                   "w": "parW / 3",
                   "b": "parH - 10"
               }
           ]
       },
       {
           "id": "c",
           "x": 10,
           "y": "prevB + 10",
           "r": "parW - 10",
           "h": "(parH - 40) / 3",
           "components": [
               {
                   "grid": "c0/1fr,c1/3fr,c2/50px,c3/25px",
                   "bounds": "10,10,parW-20,parH-20",
                   "cols": 4,
                   "rows": 1,
                   "colGap": 1,
                   "rowGap": 0
               }
           ]
       }
   ]
}
)~~~";

struct LayoutDemo : public DemoComponent,
                    private juce::CodeDocument::Listener
{
    LayoutDemo()
    {
        doc.addListener (this);
        setName ("Layout");

        auto a = new ColouredComponent ("a", juce::Colours::green);
        auto b = new ColouredComponent ("b", juce::Colours::green);
        auto c = new ColouredComponent ("c", juce::Colours::green);

        a->addAndMakeVisible (new ColouredComponent ("e", juce::Colours::yellow));
        a->addAndMakeVisible (new ColouredComponent ("f", juce::Colours::yellow));
        b->addAndMakeVisible (new ColouredComponent ("g", juce::Colours::yellow));
        b->addAndMakeVisible (new ColouredComponent ("h", juce::Colours::yellow));
        c->addAndMakeVisible (new ColouredComponent ("c0", juce::Colours::orange));
        c->addAndMakeVisible (new ColouredComponent ("c1", juce::Colours::cyan));
        c->addAndMakeVisible (new ColouredComponent ("c2", juce::Colours::magenta));
        c->addAndMakeVisible (new ColouredComponent ("c3", juce::Colours::red));

        layoutRoot.addAndMakeVisible (a);
        layoutRoot.addAndMakeVisible (b);
        layoutRoot.addAndMakeVisible (c);

        layoutJson.loadContent (layoutTxt);
        addAndMakeVisible (layoutJson);
        addAndMakeVisible (layoutRoot);
        addAndMakeVisible (error);

        error.setInterceptsMouseClicks (false, false);
        error.setJustificationType (juce::Justification::bottomLeft);
        error.setColour (juce::Label::textColourId, juce::Colours::red);
    }

    void codeDocumentTextInserted (const juce::String&, int) override
    {
        update();
    }

    void codeDocumentTextDeleted (int, int) override
    {
        update();
    }

    void update()
    {
        juce::var o;

        auto t = doc.getAllContent();
        auto e = juce::JSON::parse (t, o);

        if (e.wasOk())
        {
            resized();
            error.setText ({}, juce::dontSendNotification);
        }
        else
        {
            error.setText (e.getErrorMessage(), juce::dontSendNotification);
        }
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        layoutJson.setBounds (rc.removeFromLeft (rc.getWidth() / 2 - 4));
        error.setBounds (layoutJson.getBounds());
        rc.removeFromLeft (8);
        layoutRoot.setBounds (rc);

        layout.setLayout (doc.getAllContent());
    }

    class ColouredComponent : public juce::Component
    {
    public:
        ColouredComponent (const juce::String& n, const juce::Colour& c)
            : juce::Component (n), colour (c)
        {
        }

        ~ColouredComponent() override
        {
            deleteAllChildren();
        }

        void paint (juce::Graphics& g) override
        {
            g.fillAll (colour.withMultipliedAlpha (0.8f));
            g.setColour (juce::Colours::white);
            g.drawText (getName(), getLocalBounds(), juce::Justification::centred);
        }

        juce::Colour colour;
    };

    juce::CodeDocument doc;
    juce::CodeEditorComponent layoutJson { doc, nullptr };
    ColouredComponent layoutRoot { "root", juce::Colours::blue };

    gin::LayoutSupport layout { layoutRoot };

    juce::Label error;
};
