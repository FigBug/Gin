#pragma once

class Processor;
class ModMatrix;

//==============================================================================
/** A parameter with user values, real time safe callbacks, modulation, and
    all sorts of other fancy stuff
*/
class Parameter : public juce::AudioProcessorParameter,
                  protected juce::Timer,
                  protected RealtimeAsyncUpdater
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

    juce::String getUid()               { return uid;       }
    void setInternal (bool i)           { internal = i;     }
    bool isInternal()                   { return internal;  }
    void setModIndex (int i)            { modIndex = i;     }
    int getModIndex()                   { return modIndex;  }
    void setModMatrix (ModMatrix* m)    { modMatrix = m;    }
    ModMatrix* getModMatrix()           { return modMatrix; }

    virtual void prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)    {}
    virtual void reset()                                                           {}

    //==============================================================================
    float getProcValue();
    virtual float getProcValue (int stepSize);

    float getUserValue() const;
    int getUserValueInt() const;
    float getUserDefaultValue() const;
    virtual void setUserValue (float v);
    virtual void setUserValueNotifingHost (float f);
    void setUserValueAsUserAction (float f);
    juce::String getUserValueText() const;
    juce::String userValueToText (float val);

    bool isOnOff();
    bool isOn();

    std::function<float (float)> conversionFunction;

    virtual bool isSmoothingActive()            { return false;         }

    //==============================================================================
    void beginUserAction();
    void beginUserTimedAction();
    void endUserAction();

    //==============================================================================
    juce::NormalisableRange<float> getUserRange() const { return range;         }
    float getUserRangeStart() const                     { return range.start;   }
    float getUserRangeEnd() const                       { return range.end;     }

    //==============================================================================
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void parameterChanged (Parameter* param) = 0;
    };

    void addListener (Listener* listener);
    void removeListener (Listener* listener);

    //==============================================================================
    struct ParamState
    {
        juce::String uid;
        float value;
    };

    ParamState getState();
    void setState(const ParamState& state);

    //==============================================================================
    float getValue() const override;
    bool getBoolValue() const                   { return getValue() != 0.0f; }

    void setValue (float newValue) override;
    float getDefaultValue() const override;

    juce::String getName (int maximumStringLength) const override;
    juce::String getShortName() const;
    juce::String getLabel() const override;

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

    float value;
    float defaultValue;

    juce::String uid;
    juce::String name;
    juce::String shortName;
    juce::String label;

    std::function<juce::String (const Parameter&, float)> textFunction;

    int userActionCount {0};

    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)
};
