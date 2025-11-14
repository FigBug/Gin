/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** JSON-based component layout engine with hot-reloading support.

    This class allows you to define component layouts using JSON instead of hard-coding
    positions and sizes. On desktop platforms (not iOS/Android), it automatically watches
    the layout file for changes and reloads the layout when the file is modified, enabling
    rapid iteration during development.

    Features:
    - Define component bounds using JSON with mathematical expressions
    - Support for constants that can be referenced in expressions
    - Automatic hot-reloading on desktop platforms
    - Component lookup by ID

    Example JSON layout:
    @code
    {
        "constants": {
            "margin": 10,
            "buttonHeight": 30
        },
        "components": [
            {"id": "myButton", "x": "margin", "y": "margin", "w": 100, "h": "buttonHeight"}
        ]
    }
    @endcode

    @see setLayout, parseLayout, setConstant
*/
class Layout
#if ! JUCE_IOS && ! JUCE_ANDROID
    : public FileSystemWatcher::Listener
#endif
{
public:
    /** Creates a Layout manager for the specified parent component.

        @param parent  The component whose children will be laid out
    */
    Layout (juce::Component& parent);

   #if JUCE_IOS || JUCE_ANDROID
    /** Destructor. */
    ~Layout();
   #else
    /** Destructor. Stops watching the layout file. */
    ~Layout() override;
   #endif

    /** Loads and applies a layout from a JSON file.

        On desktop platforms, this also starts watching the file for changes
        and automatically reloads when it's modified.

        @param filename  Name of the layout file (can be relative or absolute path)
        @param source    Optional source directory for the file
    */
    void setLayout (const juce::String& filename, const juce::File& source = {});

    /** Parses and applies a layout from JSON content.

        @param content  JSON string containing the layout definition
        @returns        true if the layout was parsed successfully
    */
    bool parseLayout (const juce::String& content);

    /** Sets a constant value that can be used in layout expressions.

        Constants can be referenced by name in the JSON layout's mathematical
        expressions. For example, if you set a constant "margin" = 10, you can
        use "margin" in your layout JSON.

        @param name  The name of the constant
        @param val   The value of the constant
    */
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
