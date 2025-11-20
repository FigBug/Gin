#pragma once

#include "gin_componentmap.h"

class ConstantsStack
{
public:
    void set (const juce::String& name, double value);
    std::optional<double> get (const juce::String& name);

    class ScopedSave
    {
    public:
        ScopedSave (ConstantsStack& o) : owner (o)
        {
        }

        ~ScopedSave()
        {
            owner.constants.resize (size);
        }

        ConstantsStack& owner;
        size_t size { owner.constants.size() };
    };

private:
    std::vector<std::pair<juce::String, double>> constants;
};

class Parser
{
public:
    Parser (const juce::String equation);

    double evaluate();

    // Need to be set before each evaluation
    ComponentMap* compMap = nullptr;
    juce::Component* curComponent = nullptr;
    ConstantsStack* constants = nullptr;

private:
    juce::Component* getComp (const juce::String& cid);

    std::vector<juce::String> usedConstants;
    size_t varStackPtr = 0;
    double varStack[10];

    gin::EquationParser parser;
};

/**
    JSON-based layout system for JUCE components. Allows declarative component positioning with
    expressions, constants, macros, and live reloading.

    @section usage Basic Usage
    @code
    class MyComponent : public juce::Component
    {
    public:
        MyComponent() : layoutSupport (*this)
        {
            addAndMakeVisible (button);
            button.setComponentID ("myButton");
            layoutSupport.setLayout ({ juce::File ("layout.json") });
        }
    private:
        LayoutSupport layoutSupport;
        juce::TextButton button;
    };
    @endcode

    @section json JSON Format

    Top-level properties:
    - `w`, `h` - Set parent component size
    - `constants` - Define reusable constant values
    - `macros` - Define reusable component templates
    - `components` - Array of component definitions

    @subsection component_props Component Properties

    **Identification:**
    - `id` - Component identifier (supports comma-separated list or range syntax like "button[0..9]")
    - `factory` - Factory name to create component on demand
    - `optional` - Boolean, won't assert if component not found
    - `if` - Conditional expression to include/exclude component

    **Position (absolute):**
    - `x`, `y` - Absolute position
    - `xy` - Comma-separated "x,y"
    - `r`, `b` - Right/bottom edges
    - `cx`, `cy` - Center position (requires size)

    **Position (relative):**
    - `xd` - Horizontal delta from previous (or "delta,refID" for specific component)
    - `yd` - Vertical delta from previous (or "delta,refID")
    - `cxd`, `cyd` - Center delta from previous

    **Position (special):**
    - `bounds` - "parent", "prev", "reduced,N", "reduced,H,V", "reduced,L,T,R,B", or "x,y,w,h"
    - `inset` - Component ID to inset from
    - `border` - Border amount for inset (default 0)

    **Size:**
    - `w`, `h` - Width and height
    - `size` - Reference to size constant (e.g., "buttonSize" references constants.buttonSize.w/h)

    **Display:**
    - `visible` - Boolean or expression
    - `zorder` - Z-order index

    **Accessibility:**
    - `title` - Accessibility title
    - `tip` - Tooltip text

    **Focus:**
    - `order` - Component focus order
    - `parentOrder` - Parent component focus order

    **Custom:**
    - `properties` - Object with custom properties to set on component
    - `constants` - Local constants scoped to this component
    - `macro` - Macro name with parameters
    - `components` - Nested child components

    **Grid Layout:**
    - `grid` - Component IDs for grid (supports range syntax)
    - `cols`, `rows` - Grid dimensions (default 1)
    - `colGap`, `rowGap` - Grid spacing (default 0)

    @subsection constants Constants

    Built-in constants:
    - `mac`, `win`, `linux` - Platform flags (1.0 or 0.0)
    - `parX`, `parY`, `parW`, `parH`, `parR`, `parB` - Parent bounds
    - `prevX`, `prevY`, `prevW`, `prevH`, `prevR`, `prevB`, `prevCX`, `prevCY` - Previous component
    - `i` - Current child index
    - `idIdx` - Current iteration index (when using multiple IDs or ranges)

    Custom constants can be defined at top-level or per-component:
    @code{.json}
    {
      "constants": [
        { "margin": 10 },
        { "buttonSize": { "w": 100, "h", 25 } }
      ],
      "components": [
        { "id": "button", "x": "margin", "w": 100, "h": 30 }
      ]
    }
    @endcode

    @subsection expressions Expressions

    All numeric properties support mathematical expressions:
    - Operators: `+`, `-`, `*`, `/`, `%`, `(`, `)`
    - Constants: Any defined constant name
    - Functions: `getX(id)`, `getY(id)`, `getW(id)`, `getH(id)`, `getR(id)`, `getB(id)`, `getCX(id)`, `getCY(id)`

    Example: `"x": "parW / 2 - getW('button') / 2"`

    @subsection macros Macros

    Reusable component templates with parameters:
    @code{.json}
    {
      "macros": [
        { "id": "standardButton", "w": 100, "h": 30, "tip": "P1" }
      ],
      "components": [
        { "id": "saveBtn", "macro": "standardButton('Save')", "x": 10, "y": 10 }
      ]
    }
    @endcode

    Parameters are referenced as P1, P2, P3, etc.

    @subsection examples Examples

    Simple layout:
    @code{.json}
    {
      "components": [
        { "id": "header", "bounds": "parent" },
        { "id": "button", "x": 10, "y": 10, "w": 100, "h": 30 }
      ]
    }
    @endcode

    Relative positioning:
    @code{.json}
    {
      "components": [
        { "id": "button1", "x": 10, "y": 10, "w": 100, "h": 30 },
        { "id": "button2", "xd": 10, "y": 10 },
        { "id": "button3", "xd": 10, "y": 10 }
      ]
    }
    @endcode

    Grid layout:
    @code{.json}
    {
      "grid": "cell[0..8]",
      "bounds": "10,10,300,300",
      "cols": 3,
      "rows": 3,
      "colGap": 5,
      "rowGap": 5
    }
    @endcode

    Range syntax and expressions:
    @code{.json}
    {
      "id": "knob[0..7]",
      "x": "10 + i * 50",
      "y": 10,
      "w": 40,
      "h": 40
    }
    @endcode
*/
class LayoutSupport : private gin::FileSystemWatcher::Listener
{
public:
    /**
        Construct a LayoutSupport instance for the given parent component.

        @param parent_   The parent component whose children will be positioned
        @param factory_  Optional factory function to create components on demand.
                        Function signature: (factoryName) -> (Component*, sendToBack)
                        Return nullptr if factory name not recognized.
    */
    LayoutSupport (juce::Component& parent_, std::function<std::pair<juce::Component*, bool> (const juce::String&)> factory_ = nullptr);

    ~LayoutSupport() override;

    /**
        Generate JSON layout from existing component hierarchy.
        Outputs JSON to debug console and copies to clipboard.
        Useful for migrating existing layouts to JSON format.

        @param c  Root component to dump
    */
    static void dumpComponents (juce::Component& c);

    /** Returns true if a layout has been set */
    bool isLayoutSet();

    /**
        Load layout from JSON files. Watches files for changes and auto-reloads.

        @param files  Array of JSON files to load
    */
    void setLayout (const juce::Array<juce::File>& files);

    /**
        Load layout from binary resources (BinaryData).

        @param resourceNames  Array of resource names to load
    */
    void setLayout (const juce::StringArray& resourceNames);

    /**
        Load layout from raw JSON string.

        @param rawJson  JSON string containing layout definition
    */
    void setLayout (const juce::String& rawJson);

    /**
        Set or update a constant value.

        @param name   Constant name
        @param value  Constant value
    */
    void setConstant (const juce::String& name, int value);

    /** Callback invoked when layout is loaded or reloaded */
    std::function<void()> layoutChanged;

private:
    juce::String findContent (const juce::String& name);
    juce::Component* getComp (const juce::String& cid);

    juce::Component& parent;

    struct Bounds
    {
        int x;
        int y;
        int w;
        int h;
        bool hasPosition = false;
        bool hasSize = false;
    };

    struct JsonFile
    {
        juce::String name;
        juce::String contents;
    };

    void setLayoutInternal (const juce::Array<JsonFile>& files);

    int parse (const juce::var& equation, int equationIndex);

    void setComponentsLayout (const juce::String& currentPath, const juce::var& components);

    void loadConstants (juce::var& j);
    void loadMacros (juce::var& j);

    Bounds getBounds (int idIdx, const juce::var& component);

    juce::Component* setPosition (const juce::String& currentPath,
                                  const juce::String& id,
                                  int idIdx,
                                  const juce::var& component);

    juce::var expandMacro (const juce::var& component);

    void setGridPositions (const juce::String& currentPath, const juce::var& component);

    inline static int refCount = 0;
    inline static std::unique_ptr<FileSystemWatcher> watcher;

    // gin::FileSystemWatcher::Listener
    void fileChanged (const juce::File&, gin::FileSystemWatcher::FileSystemEvent) override;

    ConstantsStack constants;
    std::map<juce::String, juce::var> macros;

    juce::Array<juce::File> layoutFiles;
    bool layoutSet = false;

    juce::Component* curComponent = nullptr;

    juce::OwnedArray<juce::Component> createdComponents;

    ComponentMap compMap;
    std::function<std::pair<juce::Component*, bool> (const juce::String&)> componentFactory;
};
