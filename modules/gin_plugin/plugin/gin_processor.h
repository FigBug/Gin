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
/**
    Parameter smoothing configuration for audio-rate interpolation.

    SmoothingType specifies how parameter changes should be smoothed when applied
    to audio processing. This prevents zipper noise and clicks when parameters
    change, particularly important for frequency, gain, and other audio-critical
    parameters.

    Smoothing Types:
    - linear: Simple linear interpolation (faster, good for most uses)
    - eased: Exponential/eased interpolation (smoother, more natural)

    The time value specifies how long the parameter takes to reach its new
    value from the current value. Typical values are 0.02-0.2 seconds.

    Usage:
    @code
    // Fast linear smoothing for non-critical params
    SmoothingType fast(0.02f, SmoothingType::linear);

    // Slower eased smoothing for filter cutoff
    SmoothingType smooth(0.1f, SmoothingType::eased);

    // Used when creating parameters
    auto cutoff = processor.addExtParam("cutoff", "Cutoff", "", "",
                                       {20.0f, 20000.0f, 1.0f, 0.3f},
                                       1000.0f, 0.0f,
                                       SmoothingType(0.05f, SmoothingType::eased));
    @endcode

    @see Parameter, Processor
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
/**
    Plugin metadata and configuration options for Processor.

    ProcessorOptions provides configuration and metadata for a Gin-based plugin,
    including plugin identity (name, version, developer), capabilities (MIDI I/O),
    and optional features (update checker, news checker). When using a standard
    JUCE plugin project, the default constructor automatically fills these from
    JucePlugin_* preprocessor defines. For custom applications, you can manually
    configure all options.

    Configuration Categories:
    - Identity: pluginName, developer, devId, pluginVersion
    - URLs: updatesURL, url, urlTitle (for help/website links)
    - MIDI: wantsMidi, makesMidi flags
    - Features: useUpdateChecker, useNewsChecker toggles
    - Credits: programmingCredits array

    The class provides builder-style methods for common customizations:
    - withAdditionalCredits(): Add more credits while keeping defaults
    - withoutUpdateChecker(): Disable automatic update checking
    - withoutNewsChecker(): Disable news feed checking

    Usage:
    @code
    // Standard plugin (auto-configured from JucePlugin_* defines)
    ProcessorOptions opts;

    // Custom configuration
    ProcessorOptions customOpts;
    customOpts.pluginName = "My Synth";
    customOpts.developer = "My Company";
    customOpts.devId = "MYCO";
    customOpts.pluginVersion = "1.0.0";
    customOpts.wantsMidi = true;
    customOpts.makesMidi = false;

    // Builder pattern
    auto opts = ProcessorOptions()
        .withAdditionalCredits({"Jane Smith", "Additional DSP"})
        .withoutUpdateChecker()
        .withoutNewsChecker();

    // Pass to Processor
    MyProcessor processor(opts);
    @endcode

    @see Processor
*/
class ProcessorOptions
{
public:
    ProcessorOptions();

    juce::String    pluginName;
    juce::String    devId;
    juce::String    developer;
    juce::String    pluginVersion;
    juce::String    updatesURL;
    juce::String    url;
    juce::String    urlTitle;

    bool wantsMidi          = false;
    bool makesMidi          = false;
    bool useUpdateChecker   = true;
    bool useNewsChecker     = true;
    bool hasMidiLearn       = false;

    juce::StringArray programmingCredits =
    {
        "Roland Rabien",
        "RAW Material Software JUCE Framework"
    };

    ProcessorOptions withAdditionalCredits (juce::StringArray names) &&
    {
        programmingCredits.addArray (names);
        return std::move (*this);
    }

    ProcessorOptions withoutUpdateChecker() &&
    {
        useUpdateChecker = false;
        return std::move (*this);
    }

    ProcessorOptions withoutNewsChecker() &&
    {
        useNewsChecker = false;
        return std::move (*this);
    }

    std::unique_ptr<juce::LookAndFeel> lookAndFeel;
};

//==============================================================================
/**
    Advanced audio processor base class with parameter management and preset system.

    Processor extends JUCE's AudioProcessor with a complete parameter management system,
    preset/program handling, state management, and various conveniences for building
    professional audio plugins.

    Key Features:
    - **Parameter Management**: Internal and external parameters with automatic host exposure
    - **Preset System**: Full program/preset management with file watching
    - **State Management**: ValueTree-based state with automatic parameter sync
    - **Parameter Smoothing**: Built-in smoothing for audio-rate parameters
    - **Settings**: Optional persistent settings via PropertiesFile
    - **Modulation Support**: Integration with ModMatrix system
    - **Update Checking**: Optional update and news checking

    Parameter Types:
    - **External Parameters**: Exposed to host, automatable, saved in presets
    - **Internal Parameters**: Not exposed to host, for UI-only settings

    Preset System:
    Programs are automatically loaded from a directory and file-watched for changes.
    Each program stores parameters and can include custom state data.

    Usage:
    @code
    class MyProcessor : public gin::Processor
    {
    public:
        MyProcessor() : Processor(false) // false = don't auto-init
        {
            // Add parameters
            cutoff = addExtParam("cutoff", "Cutoff", "Cutoff", "Hz",
                                 {20.0f, 20000.0f}, 1000.0f,
                                 SmoothingType(0.05f));

            resonance = addExtParam("resonance", "Resonance", "Res", "",
                                    {0.0f, 1.0f}, 0.5f,
                                    SmoothingType(0.05f));

            init(); // Call after adding all parameters
        }

        void prepareToPlay(double sr, int block) override
        {
            Processor::prepareToPlay(sr, block);
            filter.setSampleRate(sr);
        }

        void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi) override
        {
            // Parameters are automatically smoothed
            filter.setCutoff(cutoff->getProcValue());
            filter.setResonance(resonance->getProcValue());
            filter.process(buffer);
        }

    private:
        Parameter* cutoff;
        Parameter* resonance;
        MyFilter filter;
    };
    @endcode

    @see Parameter, Program, ModMatrix
*/
class Processor : public ProcessorBaseClass,
                  public juce::ChangeBroadcaster,
#if ! JUCE_IOS && ! JUCE_ANDROID
                  private FileSystemWatcher::Listener,
#endif
                  private juce::Timer
{
public:
    //==============================================================================
    /**
     If you override any virtual functions, they won't get called by the constructor.
     So pass false and then call init manually from your constructor. If you don't
     override any functions, then it's safe to pass true to the constructor and
     you don't need to call init.
     */
    Processor (bool init = true, ProcessorOptions&& = {});
    Processor (const BusesProperties& ioLayouts, bool init = true, ProcessorOptions&& = {});
    ~Processor() override;

    virtual void init();
    virtual Program* createProgram() { return new Program(); }

    void reset() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    virtual juce::PropertiesFile* getSettings();

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

    /* Are any parameters currrently smoothing? */
    bool isSmoothing();
    
    virtual bool isParamLocked (gin::Parameter*) { return false; }

    virtual juce::File getProgramDirectory();
    virtual void loadAllPrograms();

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    const juce::OwnedArray<Program>& getPrograms()    { return programs; }
    Program* getProgram (const juce::String& name);
    void setCurrentProgram (juce::String name);
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void saveProgram (juce::String name, juce::String author, juce::String tags);
    void deleteProgram (int index);
    bool hasProgram (juce::String name);

    //==============================================================================
    juce::String getStateXml();
    void setStateXml (const juce::String& s);
    
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

public:
    ProcessorOptions processorOptions;
    bool loadingState = false;

    std::map<juce::String, gin::Parameter*> parameterMap;
    juce::OwnedArray<gin::Parameter> internalParameters;

    juce::ValueTree state;
    int versionHint = 1;

    std::unique_ptr<MidiLearn> midiLearn;

protected:
    /* If you plugin has more state than just parameters you need to implement these two functions
       to copy your any data you need to save/restore to/from the ValueTree 'state'.
     */
    virtual void stateUpdated() {}
    virtual void updateState()  {}

    void extractProgram (const juce::String& name, const juce::MemoryBlock& data);
    void extractProgram (const juce::String& name, const void* data, int sz);

#if ! JUCE_IOS && ! JUCE_ANDROID
    void folderChanged (const juce::File&) override;
#endif
    void timerCallback() override;

    std::unique_ptr<juce::PropertiesFile> settings;

    std::unique_ptr<gin::Parameter> createParam (juce::String uid, juce::String name, juce::String shortName, juce::String label,
                                                 juce::NormalisableRange<float> range, float defaultValue,
                                                 SmoothingType st,
                                                 std::function<juce::String (const gin::Parameter&, float)> textFunction = nullptr);

    juce::Array<gin::Parameter*> allParameters;

    void updateParams();

#if ! JUCE_IOS && ! JUCE_ANDROID
    FileSystemWatcher watcher;
#endif

    juce::String currentProgramName;
    int maxPrograms = 0;
    juce::OwnedArray<Program> programs;

    juce::Time lastStateLoad;
    juce::Time lastProgramsUpdated;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
