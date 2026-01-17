#pragma once

//==============================================================================
/** A list box of all assigned modulations
*/
class ModMatrixBox : public juce::ListBox,
                     private juce::ListBoxModel,
                     private ModMatrix::Listener
{
public:
    ModMatrixBox (gin::Processor& p, ModMatrix& m, int dw = 50)
        : proc (p), modMatrix (m), depthWidth (dw)
    {
        setName ("matrix");
        setModel (this);
        setRowHeight (16);
        refresh();

        modMatrix.addListener (this);
    }

    ~ModMatrixBox() override
    {
        modMatrix.removeListener (this);
    }

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

    void paintListBoxItem (int, juce::Graphics&, int, int, bool) override {}

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

                    auto set = [this] (ModMatrix::Function func)
                    {
                        auto& aa = owner.assignments.getReference (row);
                        owner.modMatrix.setModFunction (aa.src, ModDstId (aa.dst->getModIndex()), func);
                    };

                    juce::PopupMenu m;
                    m.setLookAndFeel (&getLookAndFeel());

                    m.addItem ("Linear", true, f == ModMatrix::Function::linear, [set] { set (ModMatrix::Function::linear); });
                    m.addItem ("Quadratic In", true, f == ModMatrix::Function::quadraticIn, [set] { set (ModMatrix::Function::quadraticIn); });
                    m.addItem ("Quadratic In/Out", true, f == ModMatrix::Function::quadraticInOut, [set] { set (ModMatrix::Function::quadraticInOut); });
                    m.addItem ("Quadratic Out", true, f == ModMatrix::Function::quadraticOut, [set] { set (ModMatrix::Function::quadraticOut); });
                    m.addItem ("Sine In", true, f == ModMatrix::Function::sineIn, [set] { set (ModMatrix::Function::sineIn); });
                    m.addItem ("Sine In Out", true, f == ModMatrix::Function::sineInOut, [set] { set (ModMatrix::Function::sineInOut); });
                    m.addItem ("Sine Out", true, f == ModMatrix::Function::sineOut, [set] { set (ModMatrix::Function::sineOut); });
                    m.addItem ("Exponential In", true, f == ModMatrix::Function::exponentialIn, [set] { set (ModMatrix::Function::exponentialIn); });
                    m.addItem ("Exponential In/Out", true, f == ModMatrix::Function::exponentialInOut, [set] { set (ModMatrix::Function::exponentialInOut); });
                    m.addItem ("Exponential Out", true, f == ModMatrix::Function::exponentialOut, [set] { set (ModMatrix::Function::exponentialOut); });
                    m.addSeparator();
                    m.addItem ("Inv Linear", true, f == ModMatrix::Function::invLinear, [set] { set (ModMatrix::Function::invLinear); });
                    m.addItem ("Inv Quadratic In", true, f == ModMatrix::Function::invQuadraticIn, [set] { set (ModMatrix::Function::invQuadraticIn); });
                    m.addItem ("Inv Quadratic In/Out", true, f == ModMatrix::Function::invQuadraticInOut, [set] { set (ModMatrix::Function::invQuadraticInOut); });
                    m.addItem ("Inv Quadratic Out", true, f == ModMatrix::Function::invQuadraticOut, [set] { set (ModMatrix::Function::invQuadraticOut); });
                    m.addItem ("Inv Sine In", true, f == ModMatrix::Function::invSineIn, [set] { set (ModMatrix::Function::invSineIn); });
                    m.addItem ("Inv Sine In/Out", true, f == ModMatrix::Function::invSineInOut, [set] { set (ModMatrix::Function::invSineInOut); });
                    m.addItem ("Inv Sine Out", true, f == ModMatrix::Function::invSineOut, [set] { set (ModMatrix::Function::invSineOut); });
                    m.addItem ("Inv Exponential In", true, f == ModMatrix::Function::invExponentialIn, [set] { set (ModMatrix::Function::invExponentialIn); });
                    m.addItem ("Inv Exponential In/Out", true, f == ModMatrix::Function::invExponentialInOut, [set] { set (ModMatrix::Function::invExponentialInOut); });
                    m.addItem ("Inv Exponential Out", true, f == ModMatrix::Function::invExponentialOut, [set] { set (ModMatrix::Function::invExponentialOut); });

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
            }
            else
            {
                src.setText ("", juce::dontSendNotification);
                dst.setText ("", juce::dontSendNotification);
                curveButton.setCurve (ModMatrix::Function::linear);
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
    gin::ModMatrix& modMatrix;
    juce::Array<Assignment> assignments;
    int depthWidth = 50;
};
