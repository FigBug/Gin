#pragma once

class Processor;
class ModMatrix;

//==============================================================================
/**
    Advanced parameter class with modulation, real-time callbacks, and host automation.

    Parameter provides a comprehensive plugin parameter implementation with features
    needed for professional audio plugins. It extends JUCE's parameter system with
    modulation support, user/processing value separation, and real-time safe callbacks.

    Value Formats:
    Parameter values are represented in three formats:
    - **value**: Normalized 0..1 range (for host automation)
    - **user value**: Range as displayed to user (e.g., -100 to +10 dB)
    - **proc value**: Range used for DSP processing (e.g., 0 to 3.1623 for gain)

    By default, user value and proc value are identical. To provide custom processing
    values, set the conversionFunction (e.g., converting dB to linear gain).

    Parameter Types:
    - **External**: Exposed to host, can be automated and modulated
    - **Internal**: Hidden from host, cannot be modulated (e.g., UI-only settings)

    Key Features:
    - Host automation support
    - Modulation matrix integration
    - Real-time safe value callbacks
    - User action tracking (begin/end gestures)
    - Custom text formatting
    - Skewed parameter ranges
    - Optional parameter smoothing
    - Listener callbacks for UI updates

    Usage:
    @code
    // Create a gain parameter with dB display but linear processing
    auto* gain = new Parameter(processor,
        "gain", "Gain", "Gain", "dB",
        -100.0f, 10.0f,   // User range: -100 to +10 dB
        0.1f,             // Step size
        0.0f,             // Default value
        1.0f);            // No skew

    // Convert dB to linear for processing
    gain->conversionFunction = [](float db) {
        return juce::Decibels::decibelsToGain(db);
    };

    // In DSP code
    float linearGain = gain->getProcValue();
    // In UI
    String displayText = gain->getUserValueText(); // "0.0 dB"
    @endcode

    @see Processor, ModMatrix, SmoothedParameter
*/
class Parameter : public juce::AudioPluginInstance::HostedParameter,
                  public juce::AsyncUpdater,
                  protected juce::Timer
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

    float getUserValue() const                          { return juce::jlimit (range.start, range.end, value.load (std::memory_order_relaxed)); }
    int getUserValueInt() const                         { return int (juce::jlimit (range.start, range.end, value.load (std::memory_order_relaxed))); }
    bool getUserValueBool() const                       { return juce::jlimit (range.start, range.end, value.load (std::memory_order_relaxed)) != 0.0f; }
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
    
    float getValue() const override                 { return juce::jlimit (0.0f, 1.0f, range.convertTo0to1 (value.load (std::memory_order_relaxed))); }
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

    std::atomic<float> value {0.0f};
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
