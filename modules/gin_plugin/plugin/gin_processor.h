#pragma once

#include "gin_parameter.h"
#include "gin_program.h"

#include "../components/gin_pluginlookandfeel.h"
#include "../components/gin_copperlookandfeel.h"

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

    virtual std::unique_ptr<PropertiesFile> getSettings();

   #if BUILD_INTERNAL_PLUGINS
    void fillInPluginDescription (PluginDescription&) const override {}
   #endif

    //==============================================================================
    using AudioProcessor::getParameter;

    void addPluginParameter (gin::Parameter* parameter);

    gin::Parameter* addExtParam (String uid, String name, String shortName, String label,
                                 NormalisableRange<float> range, float defaultValue,
                                 SmoothingType st,
                                 std::function<String (const gin::Parameter&, float)> textFunction = nullptr);

    gin::Parameter* addIntParam (String uid, String name, String shortName, String label,
                                 NormalisableRange<float> range, float defaultValue,
                                 SmoothingType st,
                                 std::function<String (const gin::Parameter&, float)> textFunction = nullptr);

    gin::Parameter* getParameter (const String& uid);
    float parameterValue (const String& uid);
    int parameterIntValue (const String& uid);
    bool parameterBoolValue (const String& uid);
    const Array<gin::Parameter*>& getPluginParameters();

    bool isSmoothing();

    virtual File getProgramDirectory();
    void loadAllPrograms();

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    const OwnedArray<Program>& getPrograms()    { return programs; }
    void setCurrentProgram (String name);
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    void saveProgram (String name, String author, String tags);
    void deleteProgram (int index);
    bool hasProgram (String name);

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

public:
    SharedResourcePointer<PluginLookAndFeel> lf;

    std::map<String, gin::Parameter*> parameterMap;
    OwnedArray<gin::Parameter> internalParameters;

    ValueTree state;

protected:
    virtual void stateUpdated() {}
    virtual void updateState()  {}

    void extractProgram (const String& name, const MemoryBlock& data);

private:
    gin::Parameter* createParam (String uid, String name, String shortName, String label,
                                 NormalisableRange<float> range, float defaultValue,
                                 SmoothingType st,
                                 std::function<String (const gin::Parameter&, float)> textFunction = nullptr);

    Array<gin::Parameter*> allParameters;

    void updateParams();

    int currentProgram = 0;
    OwnedArray<Program> programs;

    Time lastStateLoad;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
