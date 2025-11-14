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
    - Component lookup by ID with path support
    - Conditional rendering
    - Multiple components from single definition using ranges

    JSON Format:
    ============

    JSON Keys:
    - id: Component ID or comma-separated list (supports ranges like 'button[0..5]')
    - x, y: Position (numbers or expressions)
    - w, h: Width and height (numbers or expressions)
    - r, b: Right and bottom edges (numbers or expressions)
    - cx, cy: Center position (numbers or expressions)
    - bounds: Copy bounds - use 'parent' or 'prev'
    - constants: Object of name-value pairs for use in expressions
    - if: Conditional rendering (if expression == 0, component is skipped)
    - children: Array of child component layouts
    - properties: Object of properties to set on the component

    IMPORTANT: String parameters in expressions must use single quotes ' not double quotes "

    Available Functions (use in expressions):
    - getX('id'), getY('id'), getW('id'), getH('id'), getR('id'), getB('id')
    - getCX('id'), getCY('id') - Get center of component
    - prevX(), prevY(), prevW(), prevH(), prevR(), prevB() - Previous component bounds
    - parX(), parY(), parW(), parH(), parR(), parB() - Parent component bounds

    Component Paths:
    - Absolute: '/rootComponent/childComponent'
    - Relative: '../siblingComponent' or './childComponent'
    - Direct ID: 'componentName' (searches from current context)

    Example JSON layout:
    @code
    {
        "constants": {
            "margin": 10,
            "buttonHeight": 30,
            "showAdvanced": 1
        },
        "children": [
            {
                "id": "title",
                "x": "margin",
                "y": "margin",
                "w": "parW() - margin * 2",
                "h": 40
            },
            {
                "id": "button[0..4]",
                "x": "margin",
                "y": "prevB() + margin",
                "w": 100,
                "h": "buttonHeight"
            },
            {
                "id": "advanced",
                "if": "showAdvanced",
                "x": "getR('button0') + margin",
                "y": "getY('button0')",
                "bounds": "prev"
            }
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

        This method looks for the layout file in BinaryData first (for embedded resources).
        In debug builds, if a source file is provided, it will be used instead and watched
        for changes (hot-reloading).

        On desktop platforms, this also starts watching the file for changes and
        automatically reloads when it's modified, allowing for rapid UI iteration during
        development.

        Requirements:
        - Components must have names set via setName() to be found by the layout engine
        - The component hierarchy must already exist before applying the layout
        - Component IDs in JSON must match the component names exactly

        @param filename  Name of the layout file (must match a BinaryData resource name)
        @param source    Optional source file for hot-reloading in debug builds
    */
    void setLayout (const juce::String& filename, const juce::File& source = {});

    /** Parses and applies a layout from JSON content.

        This method directly parses JSON content and applies it to the component
        hierarchy. Unlike setLayout(), this doesn't set up hot-reloading.

        The JSON is parsed and all components with matching IDs have their bounds
        set according to the layout definition. Components are found by building
        a path from their name hierarchy (e.g., '/parent/child/button').

        @param content  JSON string containing the layout definition
        @returns        true if the layout was parsed successfully, false on parse error
    */
    bool parseLayout (const juce::String& content);

    /** Sets a constant value that can be used in layout expressions.

        Constants are global values that can be referenced by name in any expression
        within the layout. They're useful for values that are used in multiple places
        or that you want to change programmatically at runtime.

        Constants can also be defined in the JSON using the "constants" key, but
        this method allows you to set them from code.

        Example:
        @code
        layout.setConstant ("windowWidth", getWidth());
        layout.setConstant ("darkMode", isDarkMode() ? 1 : 0);
        // Then in JSON: "w": "windowWidth / 2"
        @endcode

        @param name  The name of the constant (case-sensitive)
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
