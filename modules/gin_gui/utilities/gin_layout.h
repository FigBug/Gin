#pragma once

//==============================================================================
/** Layout components with json, supports hot reloading
 */
class Layout
#if ! JUCE_IOS && ! JUCE_ANDROID
    : public FileSystemWatcher::Listener
#endif
{
public:
    Layout (juce::Component&);
   #if JUCE_IOS || JUCE_ANDROID
    ~Layout();
   #else
    ~Layout() override;
   #endif

    void setLayout (const juce::String& filename, const juce::File& source = {});
    bool parseLayout (const juce::String& content);
    void setConstant (const juce::String& name, double val);

private:
    void setupParser();

    int parse (const std::map<juce::String, double>& constants, const juce::var& equation, int equationIndex);

    void doComponent (const std::map<juce::String, double>& constants, const juce::String& currentPath, const juce::var& components);
    juce::Component* setBounds (const std::map<juce::String, double>& constants, const juce::String& currentPath, const juce::String& id, int idIdx, const juce::var& component);

   #if ! JUCE_IOS && ! JUCE_ANDROID
    void fileChanged (const juce::File&, gin::FileSystemWatcher::FileSystemEvent) override;
   #endif

    std::map<juce::String, juce::Component*> findAllComponents() const;

   #if ! JUCE_IOS && ! JUCE_ANDROID
    gin::FileSystemWatcher watcher;
   #endif

    juce::Component& parent;
    juce::File layoutFile;

    EquationParser parser;
    juce::Component* prevComponent = nullptr;
    juce::Component* curComponent = nullptr;

    std::map<juce::String, double> constants;
    std::map<juce::String, juce::Component*> componentMap;
};
