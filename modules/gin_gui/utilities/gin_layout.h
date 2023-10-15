#pragma once

//==============================================================================
/** Layout components with json, supports hot reloading
 */
class Layout
#if ! JUCE_IOS
    : public FileSystemWatcher::Listener
#endif
{
public:
    Layout (juce::Component&);
   #if JUCE_IOS
    ~Layout();
   #else
    ~Layout() override;
   #endif

    void setLayout (const juce::String& filename, const juce::File& source = {});

private:
    void setupParser();

    bool parseLayout (const juce::String& content);
    int parse (const juce::var& equation, int equationIndex);

    void doComponent (const juce::String& currentPath, const juce::var& components);
    juce::Component* setBounds (const juce::String& currentPath, const juce::String& id, int idIdx, const juce::var& component);

   #if ! JUCE_IOS
    void fileChanged (const juce::File, gin::FileSystemWatcher::FileSystemEvent) override;
   #endif

    std::map<juce::String, juce::Component*> findAllComponents();

   #if ! JUCE_IOS
    gin::FileSystemWatcher watcher;
   #endif

    juce::Component& parent;
    juce::File layoutFile;

    EquationParser parser;
    juce::Component* prevComponent = nullptr;
    juce::Component* curComponent = nullptr;

    std::map<juce::String, juce::Component*> componentMap;
};
