#pragma once

//==============================================================================
class MultiParamComponent : public Component,
                            public Parameter::Listener,
                            private AsyncUpdater
{
public:
    MultiParamComponent ()
    {
        triggerAsyncUpdate ();
    }

    ~MultiParamComponent () override
    {
        unwatchParams();
    }

protected:
    void showBubble (const String& text, const Rectangle<int>& rc)
    {
        if (auto p = findParentComponentOfClass<AudioProcessorEditor>())
        {
            if (bubble == nullptr)
            {
                bubble = std::make_unique<BubbleMessageComponent> (50000);
                p->addAndMakeVisible (*bubble);
            }

            AttributedString attString;
            attString.append (text, Font (13.0f));
            attString.setColour (Colours::white);

            auto rcg = p->getLocalArea (this, rc);

            bubble->showAt (rcg, attString, 50000, false, false);
        }
    }

    void hideBubble()
    {
        bubble = nullptr;
    }

    void watchParam (Parameter* p)
    {
        p->addListener (this);
        params.add (p);
    }

    void unwatchParams()
    {
        for (auto p : params)
            p->removeListener (this);
    }

    virtual void paramChanged ()                { repaint ();               }

    void enablementChanged() override           { repaint();                }
    void parameterChanged (Parameter*) override { triggerAsyncUpdate ();    }
    void handleAsyncUpdate () override          { paramChanged ();          }

    Array<Parameter*> params;

    std::unique_ptr<BubbleMessageComponent> bubble;
};
