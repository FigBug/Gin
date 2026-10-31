/*
  ==============================================================================

    TransitionsDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
inline juce::String transitionsTxt = R"~~~(
{
    "transitions": {
        "glide": "650 inOutCubic",
        "fade": "400 inOutQuad",
        "quick": "150 outQuad"
    },
    "components": [
        { "id": "moveButton", "x": 10, "y": 10, "w": 120, "h": 28 },
        { "id": "fadeButton", "xd": 10 },
        { "id": "colourButton", "xd": 10 },
        { "id": "textButton", "xd": 10 },
        { "id": "zoomButton", "xd": 10 },
        { "id": "enabledToggle", "xd": 10, "w": 160 },

        { "id": "box", "x": "10 + side * (parW - 220)", "y": 60, "w": 200, "h": "80 + side * 40", "scale": "1 + zoom * 0.25",
          "boxShadow": "0 4 10 0.5", "borderRadius": 8, "states": { "hover": { "boxShadow": "0 12 24 0.6" } },
          "transition": { "bounds": "glide", "scale": "glide", "boxShadow": "quick" } },
        { "id": "badge", "x": 10, "y": 200, "w": 200, "h": 40, "alpha": "shown", "transition": { "alpha": "fade" } },
        { "id": "card", "x": 10, "y": 260, "w": "parW - 20", "h": 120, "scale": "1 + zoom * 0.05", "origin": "top left",
          "transition": { "fill": "fade", "title": "quick", "scale": "quick" } },
        { "id": "photo", "x": 10, "y": 400, "w": 240, "h": 160,
          "states": { "hover": { "imageFilter": "contrast 40 saturation 140", "scale": 1.03 }, "pressed": { "scale": 0.98 } },
          "transition": { "imageFilter": "quick", "scale": "quick" } },
        { "id": "codeTiles", "x": 260, "y": 400, "r": "parW - 10", "h": 160 }
    ]
}
)~~~";

struct TransitionsDemo : public DemoComponent
{
    TransitionsDemo()
    {
        setName ("Transitions");

        moveButton.setButtonText ("Move");
        fadeButton.setButtonText ("Fade");
        colourButton.setButtonText ("Colours");
        textButton.setButtonText ("Title");
        zoomButton.setButtonText ("Zoom");

        for (auto b : { &moveButton, &fadeButton, &colourButton, &textButton, &zoomButton })
            addAndMakeVisible (b);

        // Gin draws no blur itself; JUCE's own drop shadow is enough for the demo
        gin::BoxShadow::setRenderer (*this, [] (juce::Graphics& g, const juce::Path& outline, const gin::ShadowSpec& s)
        {
            juce::DropShadow (s.colour, juce::roundToInt (s.blur), s.offset.roundToInt()).drawForPath (g, outline);
        });

        // Every transition in the app, off for slow machines
        enabledToggle.setButtonText ("Transitions");
        enabledToggle.setToggleState (gin::Transition::isGlobalEnabled(), juce::dontSendNotification);
        enabledToggle.onClick = [this] { gin::Transition::setGlobalEnabled (enabledToggle.getToggleState()); };
        addAndMakeVisible (enabledToggle);

        addAndMakeVisible (box);
        addAndMakeVisible (badge);
        addAndMakeVisible (card);
        addAndMakeVisible (photo);
        addAndMakeVisible (codeTiles);

        layout.setConstant ("side", 0);
        layout.setConstant ("shown", 1);
        layout.setConstant ("zoom", 0);

        zoomButton.onClick = [this]
        {
            zoom = 1 - zoom;
            layout.setConstant ("zoom", zoom);
            resized();
        };

        moveButton.onClick = [this]
        {
            side = 1 - side;
            layout.setConstant ("side", side);
            resized();
        };

        fadeButton.onClick = [this]
        {
            shown = 1 - shown;
            layout.setConstant ("shown", shown);
            resized();
        };

        colourButton.onClick = [this] { card.nextColours(); };
        textButton.onClick = [this] { card.nextTitle(); };
    }

    void resized() override
    {
        layout.setLayout (transitionsTxt);
    }

    class Box : public juce::Component
    {
    public:
        Box (const juce::String& n, juce::Colour c) : juce::Component (n), colour (c)
        {
        }

        void paint (juce::Graphics& g) override
        {
            g.setColour (colour);
            g.fillRoundedRectangle (getLocalBounds().toFloat(), 8.0f);
            g.setColour (juce::Colours::white);
            g.drawText (getName(), getLocalBounds(), juce::Justification::centred);
        }

        juce::Colour colour;
    };

    // A gradient that blends to the next colours and a title that crossfades
    class Card : public juce::Component
    {
    public:
        Card() : juce::Component ("card")
        {
            fill.setValue (gradient (0));
            title.setValue (titles[0]);
        }

        void nextColours()
        {
            colourIndex = (colourIndex + 1) % 3;
            fill.setTarget (gradient (colourIndex));
        }

        void nextTitle()
        {
            titleIndex = (titleIndex + 1) % int (std::size (titles));
            title.setTarget (titles[titleIndex]);
        }

        void paint (juce::Graphics& g) override
        {
            auto rc = getLocalBounds().toFloat();

            gin::fillGradient (g, rc, 12.0f, fill);
            gin::drawText (g, title, rc, juce::Font (juce::FontOptions (28.0f)), juce::Colours::white, juce::Justification::centred);
        }

    private:
        static juce::ColourGradient gradient (int idx)
        {
            // Relative points: the top left to the right edge, see gin::fillGradient
            juce::ColourGradient grad;
            grad.point1 = { 0.0f, 0.0f };
            grad.point2 = { 1.0f, 0.3f };

            if (idx == 0)
            {
                grad.addColour (0.0, juce::Colours::orange);
                grad.addColour (1.0, juce::Colours::purple);
            }
            else if (idx == 1)
            {
                grad.addColour (0.0, juce::Colours::teal);
                grad.addColour (0.5, juce::Colours::skyblue);
                grad.addColour (1.0, juce::Colours::navy);
            }
            else
            {
                grad.addColour (0.0, juce::Colours::darkred);
                grad.addColour (1.0, juce::Colours::gold);
            }

            return grad;
        }

        inline static const char* titles[] = { "Sunrise", "Ocean", "Ember" };

        int colourIndex = 0, titleIndex = 0;

        gin::Animated<juce::ColourGradient> fill { *this, "fill" };
        gin::Crossfade<juce::String> title { *this, "title" };
    };

    // An image that boosts its contrast while hovered; the filter, the zoom and their
    // timing are the layout's "states" and "transition"
    class Photo : public juce::Component
    {
    public:
        Photo() : juce::Component ("photo")
        {
            image.setImage (gin::MipMap (juce::ImageFileFormat::loadFrom (BinaryData::Leaf_jpg, BinaryData::Leaf_jpgSize)));
        }

        void paint (juce::Graphics& g) override
        {
            image.draw (g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
        }

    private:
        gin::AnimatedImageFilter image { *this };
    };

    // Tiles built in code, no layout json for them: the presets, the transition and the
    // tiles' states are set from code and every tile added afterwards takes them
    class CodeTiles : public juce::Component
    {
    public:
        CodeTiles() : juce::Component ("codeTiles")
        {
            addAndMakeVisible (grid);

            gin::Transition::setPresets (grid, R"({ "hover": "80 inOutQuad" })");
            gin::Transition::set (grid, juce::JSON::parse (R"({ "descendants": { "scale": "hover", "hover": "hover" } })"));
            gin::States::setForChildren (grid, juce::JSON::parse (R"({ "hover": { "scale": 1.1 }, "pressed": { "scale": 0.95 } })"));

            for (auto i = 0; i < 3; i++)
                grid.addAndMakeVisible (tiles.add (new Tile (juce::Colour::fromHSV (float (i) / 3.0f, 0.6f, 0.8f, 1.0f))));
        }

        void resized() override
        {
            grid.setBounds (getLocalBounds());

            auto rc = getLocalBounds().reduced (10);
            const auto w = rc.getWidth() / 3;

            for (auto t : tiles)
                t->setBounds (rc.removeFromLeft (w).reduced (6));
        }

    private:
        class Tile : public juce::Component
        {
        public:
            Tile (juce::Colour c) : colour (c) {}

            // The zoom is the grid's states; the colour lift isn't a state value, so code
            void mouseEnter (const juce::MouseEvent&) override  { lift.setTarget (1.0f); }
            void mouseExit (const juce::MouseEvent&) override   { lift.setTarget (0.0f); }

            void paint (juce::Graphics& g) override
            {
                g.setColour (colour.brighter (0.4f * lift));
                g.fillRoundedRectangle (getLocalBounds().toFloat(), 8.0f);
            }

        private:
            juce::Colour colour;
            gin::Animated<float> lift { *this, "hover" };
        };

        juce::Component grid;
        juce::OwnedArray<Tile> tiles;
    };

    int side = 0, shown = 1, zoom = 0;

    juce::TextButton moveButton { "moveButton" }, fadeButton { "fadeButton" }, colourButton { "colourButton" }, textButton { "textButton" },
                     zoomButton { "zoomButton" };
    juce::ToggleButton enabledToggle { "enabledToggle" };

    Box box { "box", juce::Colours::green };
    Box badge { "badge", juce::Colours::orange };
    Card card;
    Photo photo;
    CodeTiles codeTiles;

    gin::LayoutSupport layout { *this };
};
