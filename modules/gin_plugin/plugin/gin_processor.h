#pragma once

#include "gin_parameter.h"
#include "gin_program.h"

#include "../lookandfeel/gin_pluginlookandfeel.h"
#include "../lookandfeel/gin_copperlookandfeel.h"

#if BUILD_INTERNAL_PLUGINS
 #define ProcessorBaseClass juce::AudioPluginInstance
#else
 #define ProcessorBaseClass juce::AudioProcessor
#endif

//==============================================================================
/** How do you want you params smoothed?
*/
class SmoothingType
{
public:
    enum Type
    {
        linear,
        eased
    };

    SmoothingType (float time_ = 0.1f, Type type_ = linear)
      : time (time_), type (type_)
    {
    }

    float time;
    Type type;
};
//==============================================================================
/** A process with internal and external params
*/
class Processor : public ProcessorBaseClass,
                  public juce::ChangeBroadcaster
{
public:
    //==============================================================================
    /**
     If you override any virtual functions, they won't get called by the constructor.
     So pass false and then call init manually from your constructor. If you don't
     override any functions, then it's safe to pass true to the constructor and
     you don't need to call init.
     */
    Processor (bool init = true);
    ~Processor() override;

    void init();

    void reset() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    virtual std::unique_ptr<juce::PropertiesFile> getSettings();

   #if BUILD_INTERNAL_PLUGINS
    void fillInPluginDescription (juce::PluginDescription&) const override {}
   #endif

    //==============================================================================
    using AudioProcessor::getParameter;

    void addPluginParameter (gin::Parameter* parameter);

    gin::Parameter* addExtParam (juce::String uid, juce::String name, juce::String shortName, juce::String label,
                                 juce::NormalisableRange<float> range, float defaultValue,
                                 SmoothingType st,
                                 std::function<juce::String (const gin::Parameter&, float)> textFunction = nullptr);

    gin::Parameter* addIntParam (juce::String uid, juce::String name, juce::String shortName, juce::String label,
                                 juce::NormalisableRange<float> range, float defaultValue,
                                 SmoothingType st,
                                 std::function<juce::String (const gin::Parameter&, float)> textFunction = nullptr);

    gin::Parameter* getParameter (const juce::String& uid);
    float parameterValue (const juce::String& uid);
    int parameterIntValue (const juce::String& uid);
    bool parameterBoolValue (const juce::String& uid);
    const juce::Array<gin::Parameter*>& getPluginParameters();

    bool isSmoothing();

    virtual juce::File getProgramDirectory();
    void loadAllPrograms();

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    const juce::OwnedArray<Program>& getPrograms()    { return programs; }
    void setCurrentProgram (juce::String name);
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void saveProgram (juce::String name, juce::String author, juce::String tags);
    void deleteProgram (int index);
    bool hasProgram (juce::String name);

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

public:
    std::unique_ptr<PluginLookAndFeel> lf;

    std::map<juce::String, gin::Parameter*> parameterMap;
    juce::OwnedArray<gin::Parameter> internalParameters;

    juce::ValueTree state;

protected:
    virtual void stateUpdated() {}
    virtual void updateState()  {}

    void extractProgram (const juce::String& name, const juce::MemoryBlock& data);

private:
    gin::Parameter* createParam (juce::String uid, juce::String name, juce::String shortName, juce::String label,
                                 juce::NormalisableRange<float> range, float defaultValue,
                                 SmoothingType st,
                                 std::function<juce::String (const gin::Parameter&, float)> textFunction = nullptr);

    juce::Array<gin::Parameter*> allParameters;

    void updateParams();

    int currentProgram = 0;
    juce::OwnedArray<Program> programs;

    juce::Time lastStateLoad;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
