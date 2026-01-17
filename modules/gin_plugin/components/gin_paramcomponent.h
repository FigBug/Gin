#pragma once

//==============================================================================
/** Base class for a control showing a parameter with label
*/
class ParamComponent : public juce::Component,
                       public juce::SettableTooltipClient
{
public:
    using Ptr = ParamComponent*;

    ParamComponent (Parameter* parameter);
    ~ParamComponent() override;

    juce::String getUid() { return parameter->getUid(); }

    void setTooltip (const juce::String& newTooltip) override
    {
        juce::SettableTooltipClient::setTooltip (newTooltip);

        for (auto c : getChildren())
            if (auto stc = dynamic_cast<juce::SettableTooltipClient*> (c))
                stc->setTooltip (newTooltip);
    }

    virtual bool allowClicksOn (const juce::Component&) { return false; }

protected:
    Parameter* parameter = nullptr;

private:
    void mouseUp (const juce::MouseEvent& e) override;
    void showMidiLearnMenu (juce::Component* targetComponent);

    juce::Time lastMouseUpTime;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamComponent)
};
