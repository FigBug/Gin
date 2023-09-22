#pragma once

class Processor;
class ModMatrix;

//==============================================================================
/** A parameter with user values, real time safe callbacks, modulation, and
    all sorts of other fancy stuff.
 
    A paramter return it's value in 3 formats:
      value:        always 0..1
      user value:   range as displayed to user
      proc value:   range used for processing
 
      For example, a gain parameter may have a user range of -100 to +10 dB. The coresponding
      proc range would be 0 to 3.1623. By default user value and proc value are the same.
      To provide an alternate proc value, set conversionFunction
 
    Parameters can be either internal or external. External parameters are exposed to the host
    and can be modulated. Internal parameters are for things that should not be modulated.
 
    Parameters can optionally be added to a modmatrix 
*/
class Parameter : public juce::AudioPluginInstance::HostedParameter,
                  protected juce::Timer,
                  protected juce::AsyncUpdater
{
public:
    using Ptr = Parameter*;

    Parameter (Processor&, juce::String uid, juce::String name, juce::String shortName, juce::String label,
               float minValue, float maxValue,
               float intervalValue, float defaultValue, float skewFactor = 1.0f,
               std::function<juce::String (const Parameter&, float)> textFunction = nullptr);

    Parameter (Processor&, juce::String uid, juce::String name, juce::String shortName, juce::String label,
               juce::NormalisableRange<float> range, float defaultValue,
               std::function<juce::String (const Parameter&, float)> textFunction = nullptr);

    juce::String getUid() const         { return uid;       }
    void setInternal (bool i)           { internal = i;     }
    bool isInternal() const             { return internal;  }
    void setModIndex (int i)            { modIndex = i;     }
    int getModIndex() const             { return modIndex;  }
    void setModMatrix (ModMatrix* m)    { modMatrix = m;    }
    ModMatrix* getModMatrix() const     { return modMatrix; }

    virtual void prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)    {}
    virtual void reset()                                                           {}

    //==============================================================================
    float getProcValue()
    {
        if (conversionFunction != nullptr)
            return conversionFunction (getUserValue());

        return getUserValue();
    }
    
    virtual float getProcValue (int /*stepSize*/)
    {
        if (conversionFunction != nullptr)
            return conversionFunction (getUserValue());

        return getUserValue();
    }

    float getUserValue() const                          { return juce::jlimit (range.start, range.end, value); }
    int getUserValueInt() const                         { return int (juce::jlimit (range.start, range.end, value)); }
    bool getUserValueBool() const                       { return juce::jlimit (range.start, range.end, value) != 0.0f; }
    float getUserDefaultValue() const                   { return defaultValue; }
    virtual void setUserValue (float v);
    virtual void setUserValueNotifingHost (float f);
    void setUserValueAsUserAction (float f);
    juce::String getUserValueText() const               { return getText (getValue(), 1000) + label; }
    juce::String userValueToText (float val)            { return getText (range.convertTo0to1 (val), 1000) + label; }

    bool isOnOff()                                      { return juce::exactlyEqual (range.start, 0.0f) && juce::exactlyEqual (range.end, range.interval); }
    bool isOn()                                         { return ! juce::exactlyEqual (range.start, getUserValue()); }

    std::function<float (float)> conversionFunction;

    virtual bool isSmoothingActive()                    { return false;         }

    //==============================================================================
    void beginUserAction();
    void beginUserTimedAction();
    void endUserAction();

    //==============================================================================
    juce::NormalisableRange<float> getUserRange() const { return range;         }
    float getUserRangeStart() const                     { return range.start;   }
    float getUserRangeEnd() const                       { return range.end;     }

    //==============================================================================
    class ParameterListener
    {
    public:
        virtual ~ParameterListener() {}
        virtual void valueUpdated (Parameter* param) = 0;
    };

    void addListener (ParameterListener* listener);
    void removeListener (ParameterListener* listener);

    //==============================================================================
    struct ParamState
    {
        juce::String uid;
        float value;
    };

    ParamState getState();
    void setState(const ParamState& state);

    //==============================================================================
    juce::String getParameterID() const override    { return uid; }
    
    float getValue() const override                 { return juce::jlimit (0.0f, 1.0f, range.convertTo0to1 (value)); }
    bool getBoolValue() const                       { return getValue() != 0.0f; }

    void setValue (float newValue) override;
    float getDefaultValue() const override          { return range.convertTo0to1 (defaultValue); }

    juce::String getName (int maximumStringLength) const override;
    juce::String getShortName() const               { return shortName; }
    juce::String getLabel() const override          { return label; }

    int getNumSteps() const override;
    juce::String getText (float value, int /*maximumStringLength*/) const override;
    float getValueForText (const juce::String& text) const override;

    bool isOrientationInverted() const override;
    bool isAutomatable() const override;
    bool isMetaParameter() const override;

    float getSkew()         { return range.skew;            }
    bool isSkewSymmetric()  { return range.symmetricSkew;   }

protected:
    Processor& processor;

    //==============================================================================
    void handleAsyncUpdate() override;
    void timerCallback() override;
    virtual void changed() {}

    //==============================================================================
    juce::NormalisableRange<float> range;

    bool internal = false;
    ModMatrix* modMatrix = nullptr;
    int modIndex = -1;

    float value = 0.0f;
    float defaultValue = 0.0f;

    juce::String uid;
    juce::String name;
    juce::String shortName;
    juce::String label;

    std::function<juce::String (const Parameter&, float)> textFunction;

    int userActionCount = 0;

    juce::ListenerList<ParameterListener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)
};
