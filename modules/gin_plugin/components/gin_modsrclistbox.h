#pragma once

//==============================================================================
/** A list box of all mod sources
*/
class ModSrcListBox : public juce::ListBox,
                      private juce::ListBoxModel
{
public:
    ModSrcListBox (IModMatrix& m)
        : modMatrix (m)
    {
        setName ("modlist");
        setModel (this);
        updateContent();
        setRowHeight (16);
    }

    /** Shades every other row, matching the PatchBrowser lists. Off by default. */
    void setDrawAlternateRows (bool b)  { drawAlternateRows = b; repaint(); }

    int getNumRows() override
    {
        return modMatrix.getNumModSources();
    }

    void paintListBoxItem (int row, juce::Graphics& g, int, int, bool) override
    {
        if (drawAlternateRows && row % 2 == 1)
            g.fillAll (juce::Colours::white.withAlpha (0.02f));
    }

    Component* refreshComponentForRow (int row, bool, Component* c) override
    {
        auto rowComponent = (Row*)c;
        if (rowComponent == nullptr)
            rowComponent = new Row (modMatrix);

        rowComponent->update (row);
        return rowComponent;
    }

private:
    class Row : public Component
    {
    public:
        Row (IModMatrix& m)
            : modMatrix (m)
        {
            addAndMakeVisible (text);
            addAndMakeVisible (src);
        }

        void update (int idx)
        {
            text.setText (modMatrix.getModSrcName (ModSrcId (idx)), juce::dontSendNotification);
            src.setSource (ModSrcId (idx), modMatrix.getModSrcPoly (ModSrcId (idx)));
        }

        void resized() override
        {
            auto rc = getLocalBounds().reduced (2);
            src.setBounds (rc.removeFromRight (rc.getHeight()));
            rc.removeFromRight (4);
            text.setBounds (rc);
        }

        IModMatrix& modMatrix;

        juce::Label text;
        ModulationSourceButton src { modMatrix };
    };

    IModMatrix& modMatrix;
    bool drawAlternateRows = false;
};
