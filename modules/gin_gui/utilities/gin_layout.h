#pragma once

class Layout : public FileSystemWatcher::Listener
{
public:
    Layout (juce::Component&);
    ~Layout() override;

    void setLayout (const juce::String& filename, const juce::File& source);

private:
    void setupParser();

    void parseLayout (const juce::String& content);
    int parse (const juce::var& equation, int equationIndex);

    void doComponent (const juce::String& currentPath, const juce::var& components);
    juce::Component* setBounds (const juce::String& currentPath, const juce::String& id, int idIdx, const juce::var& component);

    void fileChanged (const juce::File, gin::FileSystemWatcher::FileSystemEvent) override;

    std::map<juce::String, juce::Component*> findAllComponents();

    gin::FileSystemWatcher watcher;

    juce::Component& parent;
    juce::File layoutFile;

    EquationParser parser;
    juce::Component* prevComponent = nullptr;
    juce::Component* curComponent = nullptr;

    std::map<juce::String, juce::Component*> componentMap;
};
