#pragma once

//==============================================================================
/** A list box of all assigned modulations
*/
class ModMatrixBox : public juce::ListBox,
                     private juce::ListBoxModel,
                     private IModMatrix::Listener
{
public:
    ModMatrixBox (gin::Processor& p, IModMatrix& m, int dw = 50)
        : proc (p), modMatrix (m), depthWidth (dw)
    {
        setName ("matrix");
        setTitle ("Mod Matrix");
        setModel (this);
        setRowHeight (16);
        refresh();

        modMatrix.addListener (this);
    }

    ~ModMatrixBox() override
    {
        modMatrix.removeListener (this);
    }

    /** Shades every other row, matching the PatchBrowser lists. Off by default. */
    void setDrawAlternateRows (bool b)  { drawAlternateRows = b; repaint(); }

private:
    void refresh()
    {
        assignments.clear();

        auto& params = proc.getPluginParameters();
        for (gin::Parameter* p : params)
            for (auto s : modMatrix.getModSources (p))
                assignments.add ({s, p});

        updateContent();
        repaint();
    }

    int getNumRows() override
    {
        return assignments.size();
    }

    void paintListBoxItem (int row, juce::Graphics& g, int, int, bool) override
    {
        if (drawAlternateRows && row % 2 == 1)
            g.fillAll (juce::Colours::white.withAlpha (0.02f));
    }

    juce::String getNameForRow (int row) override
    {
        if (row < 0 || row >= assignments.size())
            return {};

        auto& a = assignments.getReference (row);
        auto dst = ModDstId (a.dst->getModIndex());

        auto d = juce::roundToInt (modMatrix.getModDepth (a.src, dst) * 100.0f);

        auto name = modMatrix.getModSrcName (a.src) + " to " + a.dst->getName (100);
        name += ", depth " + juce::String (d > 0 ? "+" : "") + juce::String (d) + "%";
        name += modMatrix.getModBipolarMapping (a.src, dst) ? ", bipolar" : ", unipolar";
        name += ", curve " + getCurveName (modMatrix.getModFunction (a.src, dst));
        if (! modMatrix.getModEnable (a.src, dst))
            name += ", off";

        return name;
    }

    static juce::String getCurveName (ModFunction f)
    {
        for (auto& c : curveNames)
            if (c.first == f)
                return c.second;

        return {};
    }

    static constexpr std::pair<ModFunction, const char*> curveNames[] =
    {
        { ModFunction::linear,              "Linear" },
        { ModFunction::quadraticIn,         "Quadratic In" },
        { ModFunction::quadraticInOut,      "Quadratic In/Out" },
        { ModFunction::quadraticOut,        "Quadratic Out" },
        { ModFunction::sineIn,              "Sine In" },
        { ModFunction::sineInOut,           "Sine In Out" },
        { ModFunction::sineOut,             "Sine Out" },
        { ModFunction::exponentialIn,       "Exponential In" },
        { ModFunction::exponentialInOut,    "Exponential In/Out" },
        { ModFunction::exponentialOut,      "Exponential Out" },
        { ModFunction::invLinear,           "Inv Linear" },
        { ModFunction::invQuadraticIn,      "Inv Quadratic In" },
        { ModFunction::invQuadraticInOut,   "Inv Quadratic In/Out" },
        { ModFunction::invQuadraticOut,     "Inv Quadratic Out" },
        { ModFunction::invSineIn,           "Inv Sine In" },
        { ModFunction::invSineInOut,        "Inv Sine In/Out" },
        { ModFunction::invSineOut,          "Inv Sine Out" },
        { ModFunction::invExponentialIn,    "Inv Exponential In" },
        { ModFunction::invExponentialInOut, "Inv Exponential In/Out" },
        { ModFunction::invExponentialOut,   "Inv Exponential Out" },
    };

    juce::Component* refreshComponentForRow (int row, bool, juce::Component* c) override
    {
        auto rowComponent = dynamic_cast<Row*>(c);
        if (rowComponent == nullptr)
            rowComponent = new Row (*this);

        rowComponent->update (row);
        return rowComponent;
    }

    void modMatrixChanged() override
    {
        refresh();
    }

    class Row : public juce::Component,
                private juce::Slider::Listener
    {
    public:
        Row (ModMatrixBox& o)
            : owner (o)
        {
            addAndMakeVisible (enableButton);
            addAndMakeVisible (deleteButton);
            addAndMakeVisible (curveButton);
            addAndMakeVisible (biPolarButton);
            addAndMakeVisible (depth);
            addAndMakeVisible (src);
            addAndMakeVisible (dst);

            enableButton.setTitle ("Enable");
            enableButton.setToggleable (true);
            deleteButton.setTitle ("Delete");
            biPolarButton.setTitle ("Bipolar");
            biPolarButton.setToggleable (true);
            depth.setTitle ("Depth");

            depth.setRange (-1.0, 1.0);
            depth.getProperties().set ("fromCentre", true);
            depth.getProperties().set ("fullRect", true);
            depth.addListener (this);
            depth.setSliderSnapsToMousePosition (false);
            depth.setMouseDragSensitivity (750);
            depth.setPopupDisplayEnabled (true, true, findParentComponentOfClass<juce::AudioProcessorEditor>());
            depth.setDoubleClickReturnValue (true, 0.0);
            depth.onValueChange = [this]
            {
                auto& a = owner.assignments.getReference (row);
                auto parameter = a.dst;

                auto dstId = ModDstId (parameter->getModIndex());

                auto range = parameter->getUserRange();
                if (range.interval <= 0.0f || juce::ModifierKeys::currentModifiers.isShiftDown())
                {
                    owner.modMatrix.setModDepth (a.src, dstId, float (depth.getValue()));
                }
                else
                {
                    auto uv = range.convertFrom0to1 (std::clamp (float (parameter->getValue() + depth.getValue()), 0.0f, 1.0f));
                    auto nv = range.convertTo0to1 (range.snapToLegalValue (uv));

                    auto d = nv - parameter->getValue();

                    owner.modMatrix.setModDepth (a.src, dstId, d);
                    depth.setValue (d, juce::dontSendNotification);
                }
            };
            depth.onTextFromValue = [this] (double v)
            {
                auto& a = owner.assignments.getReference (row);
                auto parameter = a.dst;

                auto val = parameter->getText (std::clamp (float (parameter->getValue() + v), 0.0f, 1.0f), 1000);

                return val + " " + parameter->getLabel();
            };

            enableButton.onClick = [this]
            {
                if (row >= 0 && row < owner.assignments.size())
                {
                    auto& a = owner.assignments.getReference (row);

                    auto e = owner.modMatrix.getModEnable (a.src, ModDstId (a.dst->getModIndex()));
                    owner.modMatrix.setModEnable (a.src, ModDstId (a.dst->getModIndex()), ! e);
                    enableButton.setToggleState (! e, juce::dontSendNotification);
                }
            };

            biPolarButton.onClick = [this]
            {
                if (row >= 0 && row < owner.assignments.size())
                {
                    auto& a = owner.assignments.getReference (row);

                    auto e = owner.modMatrix.getModBipolarMapping (a.src, ModDstId (a.dst->getModIndex()));
                    owner.modMatrix.setModBipolarMapping (a.src, ModDstId (a.dst->getModIndex()), ! e);
                    biPolarButton.setToggleState (! e, juce::dontSendNotification);
                }
            };

            deleteButton.onClick = [this]
            {
                if (row >= 0 && row < owner.assignments.size())
                {
                    auto& a = owner.assignments.getReference (row);
                    owner.modMatrix.clearModDepth (a.src, ModDstId (a.dst->getModIndex()));
                }
            };

            curveButton.onClick = [this]
            {
                if (row >= 0 && row < owner.assignments.size())
                {
                    auto& a = owner.assignments.getReference (row);
                    auto f = owner.modMatrix.getModFunction (a.src, ModDstId (a.dst->getModIndex()));

                    auto set = [this] (ModFunction func)
                    {
                        auto& aa = owner.assignments.getReference (row);
                        owner.modMatrix.setModFunction (aa.src, ModDstId (aa.dst->getModIndex()), func);
                    };

                    juce::PopupMenu m;
                    m.setLookAndFeel (&getLookAndFeel());

                    for (auto& c : curveNames)
                    {
                        if (c.first == ModFunction::invLinear)
                            m.addSeparator();

                        auto func = c.first;
                        m.addItem (c.second, true, f == func, [set, func] { set (func); });
                    }

                    m.showMenuAsync ({});
                }
            };
        }

        void sliderValueChanged (juce::Slider*) override
        {
            if (row >= 0 && row < owner.assignments.size())
            {
                auto& a = owner.assignments.getReference (row);
                owner.modMatrix.setModDepth (a.src, ModDstId (a.dst->getModIndex()), (float) depth.getValue());
            }
        }

        void update (int idx)
        {
            row = idx;

            if (idx >= 0 && idx < owner.assignments.size())
            {
                auto& a = owner.assignments.getReference (idx);
                src.setText (owner.modMatrix.getModSrcName (a.src), juce::dontSendNotification);
                dst.setText (a.dst->getName (100), juce::dontSendNotification);

                auto e = owner.modMatrix.getModEnable (a.src, ModDstId (a.dst->getModIndex()));
                enableButton.setToggleState (e, juce::dontSendNotification);

                auto b = owner.modMatrix.getModBipolarMapping (a.src, ModDstId (a.dst->getModIndex()));
                biPolarButton.setToggleState (b, juce::dontSendNotification);

                depth.setValue (owner.modMatrix.getModDepth (a.src, ModDstId (a.dst->getModIndex())), juce::dontSendNotification);
                curveButton.setCurve (owner.modMatrix.getModFunction (a.src, ModDstId (a.dst->getModIndex())));
                curveButton.setTitle ("Curve: " + getCurveName (owner.modMatrix.getModFunction (a.src, ModDstId (a.dst->getModIndex()))));
            }
            else
            {
                src.setText ("", juce::dontSendNotification);
                dst.setText ("", juce::dontSendNotification);
                curveButton.setCurve (ModFunction::linear);
                curveButton.setTitle ({});
            }
        }

        void resized() override
        {
            auto rc = getLocalBounds().reduced (2);

            int h = rc.getHeight();

            enableButton.setBounds (rc.removeFromLeft (h));
            rc.removeFromLeft (4);
            deleteButton.setBounds (rc.removeFromRight (h));
            rc.removeFromLeft (2);
            depth.setBounds (rc.removeFromLeft (owner.depthWidth));
            rc.removeFromLeft (4);
            biPolarButton.setBounds (rc.removeFromLeft (h));
            rc.removeFromLeft (2);
            curveButton.setBounds (rc.removeFromLeft (h));

            int w = rc.getWidth() / 2;
            src.setBounds (rc.removeFromLeft (w));
            dst.setBounds (rc.removeFromLeft (w));
        }

        ModMatrixBox& owner;
        int row = 0;

        class DepthSlider : public juce::Slider
        {
        public:
            DepthSlider() : juce::Slider (juce::Slider::LinearHorizontal, juce::Slider::NoTextBox)
            {
            }

            juce::String getTextFromValue (double value) override
            {
                if (onTextFromValue)
                    return onTextFromValue (value);

                return juce::Slider::getTextFromValue (value);
            }

            std::function<juce::String (double)> onTextFromValue;
        };

        DepthSlider depth;

        juce::Label src;
        juce::Label dst;

        ModCurveButton curveButton;
        SVGButton biPolarButton { "bi", Assets::bipolar };

        SVGButton enableButton { "enable", Assets::power, 1 };
        SVGButton deleteButton { "delete", Assets::del };
    };

    struct Assignment
    {
        ModSrcId src = {};
        Parameter* dst = nullptr;
    };

    gin::Processor& proc;
    gin::IModMatrix& modMatrix;
    juce::Array<Assignment> assignments;
    int depthWidth = 50;
    bool drawAlternateRows = false;
};
