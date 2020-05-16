#pragma once

/** Just a simple header with a title
*/
class ControlHeader : public Component
{
public:
    ControlHeader (const String& name_ = {})
        : name (name_)
    {
    }

private:
    void paint (Graphics& g) override
    {
        g.setColour (Colours::white.withAlpha (0.2f));
        g.fillAll ();

        g.setColour (Colours::white);
        g.drawText (name, getLocalBounds(), Justification::centred);
    }

    String name;
};
