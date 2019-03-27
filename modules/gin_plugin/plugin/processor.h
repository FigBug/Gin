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
    ~GinProcessor();
    
    PropertiesFile* getSettings();
        
    //==============================================================================
    using AudioProcessor::getParameter;
    
    void addPluginParameter (Parameter* parameter);
    Parameter* getParameter (const String& uid);
    float parameterValue (const String& uid);
    int parameterIntValue (const String& uid);
    bool parameterBoolValue (const String& uid);
    Array<Parameter*> getPluginParameters();
    
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
    
    ScopedPointer<PropertiesFile> properties;
    
    std::map<String, Parameter*> parameterMap;
    
    ValueTree state;
    
protected:
    virtual void stateUpdated() {};
    virtual void updateState()  {};
    
private:
    void updateParams();
    
    LookAndFeel_V3 lookAndFeel;
    
    int currentProgram {0};
    OwnedArray<GinProgram> programs;

    Time lastStateLoad;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GinProcessor)
};

