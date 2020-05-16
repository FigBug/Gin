#pragma once

//==============================================================================
/** A button for enabling modulation learn
*/
class ModulationSourceButton : public Button,
                               private ModMatrix::Listener
{
public:
    ModulationSourceButton (ModMatrix& mm, int src = -1, bool poly_ = false)
        : Button (""), modMatrix (mm), source (src), poly (poly_)
    {
        modMatrix.addListener (this);
        update();
    }

    ~ModulationSourceButton() override
    {
        modMatrix.removeListener (this);
    }

    void setSource (int src, bool p)
    {
        source = src;
        poly = p;
        update();
    }

private:
    void update()
    {
        if (source != -1)
            setTooltip ("Mod Source: " + modMatrix.getModSrcName (source));
        else
            setTooltip ({});

        learnSourceChanged (modMatrix.getLearn());
    }

    void clicked() override
    {
        if (modMatrix.getLearn() == source)
            modMatrix.disableLearn();
        else
            modMatrix.enableLearn (source);
    }

    void learnSourceChanged (int src) override
    {
        setToggleState (src == source, dontSendNotification);
    }

    void paintButton (Graphics& g, bool over, bool down) override
    {
        auto c = getToggleState() ? Colours::white.withAlpha (0.9f) : Colours::white.withAlpha (0.4f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);

        g.setColour (c);

        auto rc = getLocalBounds().toFloat();
        auto& p = poly ? getPolyPath() : getMonoPath();
        g.fillPath (p, p.getTransformToScaleToFit (rc, true));
    }

    const Path& getMonoPath()
    {
        static const unsigned char pathData[] = { 110,109,0,0,192,67,0,0,0,66,108,0,0,128,66,0,0,0,66,98,51,51,229,65,0,0,0,66,0,0,0,0,154,153,114,66,0,0,0,0,0,0,192,66,108,0,0,0,0,0,0,208,67,98,0,0,0,0,205,172,225,67,51,51,229,65,0,0,240,67,0,0,128,66,0,0,240,67,108,0,0,192,67,0,0,240,67,98,205,172,
        209,67,0,0,240,67,0,0,224,67,205,172,225,67,0,0,224,67,0,0,208,67,108,0,0,224,67,0,0,192,66,98,0,0,224,67,154,153,114,66,205,172,209,67,0,0,0,66,0,0,192,67,0,0,0,66,99,109,0,0,96,67,0,0,144,67,98,123,84,78,67,0,0,144,67,0,0,64,67,195,213,136,67,0,0,64,
        67,0,0,128,67,98,0,0,64,67,122,84,110,67,123,84,78,67,0,0,96,67,0,0,96,67,0,0,96,67,98,133,171,113,67,0,0,96,67,0,0,128,67,123,84,110,67,0,0,128,67,0,0,128,67,98,0,0,128,67,194,213,136,67,133,171,113,67,0,0,144,67,0,0,96,67,0,0,144,67,99,101,0,0 };

        static Path path;

        if (path.isEmpty())
            path.loadPathFromData (pathData, sizeof (pathData));

        return path;
    }

    const Path& getPolyPath()
    {
        static const unsigned char pathData[] = { 110,109,0,0,192,67,0,0,0,66,108,0,0,128,66,0,0,0,66,98,51,51,229,65,0,0,0,66,0,0,0,0,154,153,114,66,0,0,0,0,0,0,192,66,108,0,0,0,0,0,0,208,67,98,0,0,0,0,205,172,225,67,51,51,229,65,0,0,240,67,0,0,128,66,0,0,240,67,108,0,0,192,67,0,0,240,67,98,205,172,
        209,67,0,0,240,67,0,0,224,67,205,172,225,67,0,0,224,67,0,0,208,67,108,0,0,224,67,0,0,192,66,98,0,0,224,67,154,153,114,66,205,172,209,67,0,0,0,66,0,0,192,67,0,0,0,66,99,109,0,0,0,67,0,0,192,67,98,246,168,220,66,0,0,192,67,0,0,192,66,195,213,184,67,0,0,
        192,66,0,0,176,67,98,0,0,192,66,61,42,167,67,246,168,220,66,0,0,160,67,0,0,0,67,0,0,160,67,98,133,171,17,67,0,0,160,67,0,0,32,67,61,42,167,67,0,0,32,67,0,0,176,67,98,0,0,32,67,195,213,184,67,133,171,17,67,0,0,192,67,0,0,0,67,0,0,192,67,99,109,0,0,0,67,
        0,0,144,67,98,246,168,220,66,0,0,144,67,0,0,192,66,195,213,136,67,0,0,192,66,0,0,128,67,98,0,0,192,66,122,84,110,67,246,168,220,66,0,0,96,67,0,0,0,67,0,0,96,67,98,133,171,17,67,0,0,96,67,0,0,32,67,123,84,110,67,0,0,32,67,0,0,128,67,98,0,0,32,67,194,213,
        136,67,133,171,17,67,0,0,144,67,0,0,0,67,0,0,144,67,99,109,0,0,0,67,0,0,64,67,98,246,168,220,66,0,0,64,67,0,0,192,66,133,171,49,67,0,0,192,66,0,0,32,67,98,0,0,192,66,123,84,14,67,246,168,220,66,0,0,0,67,0,0,0,67,0,0,0,67,98,133,171,17,67,0,0,0,67,0,0,
        32,67,123,84,14,67,0,0,32,67,0,0,32,67,98,0,0,32,67,133,171,49,67,133,171,17,67,0,0,64,67,0,0,0,67,0,0,64,67,99,109,0,0,160,67,0,0,192,67,98,61,42,151,67,0,0,192,67,0,0,144,67,195,213,184,67,0,0,144,67,0,0,176,67,98,0,0,144,67,61,42,167,67,61,42,151,
        67,0,0,160,67,0,0,160,67,0,0,160,67,98,195,213,168,67,0,0,160,67,0,0,176,67,61,42,167,67,0,0,176,67,0,0,176,67,98,0,0,176,67,195,213,184,67,195,213,168,67,0,0,192,67,0,0,160,67,0,0,192,67,99,109,0,0,160,67,0,0,144,67,98,61,42,151,67,0,0,144,67,0,0,144,
        67,195,213,136,67,0,0,144,67,0,0,128,67,98,0,0,144,67,122,84,110,67,61,42,151,67,0,0,96,67,0,0,160,67,0,0,96,67,98,195,213,168,67,0,0,96,67,0,0,176,67,123,84,110,67,0,0,176,67,0,0,128,67,98,0,0,176,67,194,213,136,67,195,213,168,67,0,0,144,67,0,0,160,
        67,0,0,144,67,99,109,0,0,160,67,0,0,64,67,98,61,42,151,67,0,0,64,67,0,0,144,67,133,171,49,67,0,0,144,67,0,0,32,67,98,0,0,144,67,123,84,14,67,61,42,151,67,0,0,0,67,0,0,160,67,0,0,0,67,98,195,213,168,67,0,0,0,67,0,0,176,67,123,84,14,67,0,0,176,67,0,0,32,
        67,98,0,0,176,67,133,171,49,67,195,213,168,67,0,0,64,67,0,0,160,67,0,0,64,67,99,101,0,0 };

        static Path path;

        if (path.isEmpty())
            path.loadPathFromData (pathData, sizeof (pathData));

        return path;
    }

    ModMatrix& modMatrix;
    int source = -1;
    bool poly = false;
};

//==============================================================================
/** A button for the modulation destination
*/
class ModulationDestinationButton : public Button
{
public:
    ModulationDestinationButton() : Button ("")
    {
    }

    ~ModulationDestinationButton() override
    {
    }

private:
    void paintButton (Graphics& g, bool over, bool down) override
    {
        auto c = getToggleState() ? Colours::white.withAlpha (0.9f) : Colours::white.withAlpha (0.4f);
        if (over || down)
            c = c.withMultipliedAlpha (1.2f);

        g.setColour (c);

        auto rc = getLocalBounds().toFloat();
        auto p = getPath();
        g.fillPath (p, p.getTransformToScaleToFit (rc, true));
    }

    const Path& getPath()
    {
        static const unsigned char pathData[] = { 110,109,0,0,192,67,0,0,0,66,108,0,0,128,66,0,0,0,66,98,51,51,229,65,0,0,0,66,0,0,0,0,154,153,114,66,0,0,0,0,0,0,192,66,108,0,0,0,0,0,0,208,67,98,0,0,0,0,205,172,225,67,51,51,229,65,0,0,240,67,0,0,128,66,0,0,240,67,108,0,0,192,67,0,0,240,67,98,205,172,
        209,67,0,0,240,67,0,0,224,67,205,172,225,67,0,0,224,67,0,0,208,67,108,0,0,224,67,0,0,192,66,98,0,0,224,67,154,153,114,66,205,172,209,67,0,0,0,66,0,0,192,67,0,0,0,66,99,109,0,0,96,67,0,0,144,67,98,123,84,78,67,0,0,144,67,0,0,64,67,195,213,136,67,0,0,64,
        67,0,0,128,67,98,0,0,64,67,122,84,110,67,123,84,78,67,0,0,96,67,0,0,96,67,0,0,96,67,98,133,171,113,67,0,0,96,67,0,0,128,67,123,84,110,67,0,0,128,67,0,0,128,67,98,0,0,128,67,194,213,136,67,133,171,113,67,0,0,144,67,0,0,96,67,0,0,144,67,99,101,0,0 };

        static Path path;

        if (path.isEmpty())
            path.loadPathFromData (pathData, sizeof (pathData));

        return path;
    }
};

//==============================================================================

/** A button and text readout that shows the current modulation source
*/
class ModulationOverview : public Component,
                           private ModMatrix::Listener
{
public:
    ModulationOverview (ModMatrix& mm)
        : modMatrix (mm)
    {
        modMatrix.addListener (this);
        learnSourceChanged (modMatrix.getLearn ());

        addAndMakeVisible (button);
        addAndMakeVisible (name);
    }

    ~ModulationOverview() override
    {
        modMatrix.removeListener (this);
    }

private:
    void resized() override
    {
        auto rc = getLocalBounds();
        button.setBounds (rc.removeFromLeft (rc.getHeight()));
        rc.removeFromLeft (4);
        name.setBounds (rc);
    }

    void learnSourceChanged (int src) override
    {
        setVisible (src != -1);
        button.setSource (src, src == -1 ? false : modMatrix.getModSrcPoly (src));

        if (src != -1)
            name.setText (modMatrix.getModSrcName (src), dontSendNotification);
        else
            name.setText ({}, dontSendNotification);
    }

    ModMatrix& modMatrix;

    ModulationSourceButton button { modMatrix, -1 };
    Label name;
};

//==============================================================================
/** A list box of al mod sources
*/
class ModSrcListBox : public ListBox,
					  private ListBoxModel
{
public:
	ModSrcListBox (ModMatrix& m)
		: modMatrix (m)
	{
		setModel (this);
		updateContent();
		setRowHeight (16);
	}

	int getNumRows() override
	{
		return modMatrix.getNumModSources();
	}

	void paintListBoxItem (int, Graphics&, int, int, bool) override {}

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
		Row (ModMatrix& m)
			: modMatrix (m)
		{
			addAndMakeVisible (text);
			addAndMakeVisible (src);
		}

		void update (int idx)
		{
			text.setText (modMatrix.getModSrcName (idx), dontSendNotification);
			src.setSource (idx, modMatrix.getModSrcPoly (idx));
		}

		void resized() override
		{
			auto rc = getLocalBounds().reduced (2);
			src.setBounds (rc.removeFromRight (rc.getHeight()));
			rc.removeFromRight (4);
			text.setBounds (rc);
		}

		ModMatrix& modMatrix;

		Label text;
		ModulationSourceButton src { modMatrix };
	};

	ModMatrix& modMatrix;
};
