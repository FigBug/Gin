#pragma once

#include "parameter.h"
#include "program.h"

#include "../components/pluginlookandfeel.h"

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
    
    void reset() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    std::unique_ptr<PropertiesFile> getSettings();

    //==============================================================================
    using AudioProcessor::getParameter;

    void addPluginParameter (Parameter* parameter);
    
    Parameter* addExtParam (String uid, String name, String shortName, String label,
                            NormalisableRange<float> range, float defaultValue,
                            float smoothingTime,
                            std::function<String (const Parameter&, float)> textFunction = nullptr);
    
    Parameter* addIntParam (String uid, String name, String shortName, String label,
                            NormalisableRange<float> range, float defaultValue,
                            float smoothingTime,
                            std::function<String (const Parameter&, float)> textFunction = nullptr);
    
    Parameter* getParameter (const String& uid);
    float parameterValue (const String& uid);
    int parameterIntValue (const String& uid);
    bool parameterBoolValue (const String& uid);
    const Array<Parameter*>& getPluginParameters();
    
    bool isSmoothing();

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
    SharedResourcePointer<PluginLookAndFeel> lf;
    
    std::unique_ptr<PropertiesFile> properties;

    std::map<String, Parameter*> parameterMap;
    OwnedArray<Parameter> internalParameters;

    ValueTree state;

protected:
    virtual void stateUpdated() {}
    virtual void updateState()  {}

private:
    Array<Parameter*> allParameters;
    
    void updateParams();

    int currentProgram = 0;
    OwnedArray<GinProgram> programs;

    Time lastStateLoad;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GinProcessor)
};
