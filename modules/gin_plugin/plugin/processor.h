#pragma once

#include "parameter.h"
#include "program.h"

//==============================================================================
/**
*/
class GinProcessor : public AudioProcessor,
                     public ChangeBroadcaster
{
public:
    //==============================================================================
    GinProcessor();
    ~GinProcessor() override;
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    std::unique_ptr<PropertiesFile> getSettings();

    //==============================================================================
    using AudioProcessor::getParameter;

    void addPluginParameter (Parameter* parameter);
    Parameter* addParam (String uid, String name, String shortName, String label, float minValue, float maxValue,
                         float intervalValue, float defaultValue, float skewFactor = 1.0f,
                         std::function<String (const Parameter&, float)> textFunction = nullptr);
    
    Parameter* getParameter (const String& uid);
    float parameterValue (const String& uid);
    int parameterIntValue (const String& uid);
    bool parameterBoolValue (const String& uid);
    Array<Parameter*> getPluginParameters();
    
    bool isSmoothing()                              { return smoothingCount.get() > 0; }

    File getProgramDirectory();
    File getSettingsFile();

    void loadAllPrograms();

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    void saveProgram (String name);
    void deleteProgram (int index);

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

public:
    OwnedArray<LevelTracker> inputLevels;
    OwnedArray<LevelTracker> outputLevels;

    std::unique_ptr<PropertiesFile> properties;

    std::map<String, Parameter*> parameterMap;

    ValueTree state;

protected:
    virtual void stateUpdated() {}
    virtual void updateState()  {}

private:
    Atomic<int> smoothingCount;
    
    void updateParams();

    int currentProgram = 0;
    OwnedArray<GinProgram> programs;

    Time lastStateLoad;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GinProcessor)
};
