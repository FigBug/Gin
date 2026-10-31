/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 Graciously donated to Gin by reFX Audio Software Inc.

 ==============================================================================*/

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
    - `transitions` - Named transition presets, "<ms> <easing>" (see Transition)
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
    - `pW`, `pH` - Post-layout width and height (evaluated after child components are laid out)

    **Display:**
    - `visible` - Boolean or expression
    - `zorder` - Z-order index
    - `alpha` - Opacity 0..1, number or expression. Alpha 0 hides the component (setVisible (false)),
                above 0 shows it. With an "alpha" transition it fades: a fade out hides the component
                when it ends, a fade in shows it first and starts from 0 if it was hidden

    - `scale` - Zoom factor, number or expression, 1 = none (see Transitions below)
    - `origin` - The scale's pivot: "center" (default), "top left", "0.5 0", "25% 100%"...

    - `boxShadow` - CSS-style shadow layers "x y blur opacity [spread] [colour], ...", painted by the parent (see BoxShadow)
    - `borderRadius` - The corner radius of the outline the shadow follows
    - `states` - Values while hovered, pressed or disabled, e.g. { "hover": { "scale": 1.03 } } (see States)
    - `children` - { "states": ... } for each direct child, also ones added later (see States)

    **Animation:**
    - `transition` - Preset name, inline spec or "none" for all animated properties, or an object
                     per property, e.g. { "bounds": "glide", "alpha": "quick" }. Inherited by the
                     descendants (see Transition)

    **Accessibility:**
    - `title` - Accessibility title
    - `tip` - Tooltip text

    **Focus:**
    - `order` - Component focus order
    - `parentOrder` - Parent component focus order
    - `focus` - Boolean, whether the component takes keyboard focus (is a tab stop)
    - `clickFocus` - Boolean, whether a mouse click moves the keyboard focus to the component
    - `focusContainer` - "none", "focus" or "keyboard" (see juce::Component::FocusContainerType)

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
    - `parCX`, `parCY` - Parent center position
    - `parW2`, `parH2` - Half of parent width and height
    - `prevX`, `prevY`, `prevW`, `prevH`, `prevR`, `prevB`, `prevCX`, `prevCY` - Previous component
    - `i` - Current child index
    - `idIdx` - Current iteration index (when using multiple IDs or ranges)
    - `minX`, `minY`, `maxX`, `maxY` - Child bounds (only available in pW/pH expressions)

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

    @subsection transitions Transitions

    Top-level "transitions" names presets, a component's "transition" picks one per
    property; see Transition for the spec syntax ("<ms> <easing>"), the easing names, the
    string vs per-property object forms, "none" and how entries inherit down the tree.

    Three properties are animated by the layout itself:
    - "bounds": a component whose "bounds" transition isn't instant moves and resizes to its
      new bounds over the transition instead of jumping. The layout pass itself is
      unchanged, so prev / parent / maxY expressions see the final bounds; afterwards those
      components go back to their old bounds and animate. Re-running the layout mid-flight
      continues from where the component is.
    - "alpha": the "alpha" entry (0..1) fades with the "alpha" transition. A fade that ends
      at 0 hides the component (setVisible (false)) when it ends and takes no mouse clicks
      while it fades (setInterceptsMouseClicks (false, false), the previous flags come back
      when it is shown again); a fade from 0 or from hidden shows it first. Without a
      transition, 0 hides at once. No "visible" entry needed.
    - "scale": the "scale" entry (1 = none, number or expression) zooms the component with
      juce::Component::setTransform about its "origin" ("center" by default; CSS
      transform-origin style, e.g. "top left", "right", "0.5 0", "25% 100%", see
      parseTransformOrigin), following its bounds during a "bounds" transition. It replaces
      any transform of the component's own (see AnimatedScale).

    "states" and "children" switch "alpha", "scale" and the "imageFilter" of images drawn
    through AnimatedImageFilter while the mouse is over or presses a component, with the
    same transitions (see States).

    The first layout of a component, components that aren't showing and instant
    transitions never animate. Without a "default" preset and "transition" entries
    nothing animates. Hot reloaded json takes effect on the next pass. Animated and
    Crossfade values in code use the same presets and entries.

    Drive a state change through constants and re-run the layout:

    @code{.json}
    {
      "transitions": { "quick": "150 outQuad", "glide": "650 inOutCubic" },
      "components": [
        { "id": "drawer", "x": "open * 200 - 200", "y": 0, "w": 200, "h": "parH", "transition": "glide" },
        { "id": "hint", "x": 10, "y": 10, "w": 150, "h": 20, "alpha": "1 - open", "transition": { "alpha": "quick" } }
      ]
    }
    @endcode

    @code
    layout.setConstant ("open", 1);
    resized();      // calls layout.setLayout (...): the drawer glides in, the hint fades out and hides
    @endcode

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

    Auto-sizing container based on children (pW/pH):
    @code{.json}
    {
      "id": "container",
      "x": 10,
      "y": 10,
      "w": 500,
      "h": 500,
      "pW": "maxX + 20",
      "pH": "maxY + 20",
      "components": [
        { "id": "child1", "x": 10, "y": 10, "w": 100, "h": 50 },
        { "id": "child2", "x": 120, "y": 10, "w": 80, "h": 50 }
      ]
    }
    @endcode

    Center positioning using parCX/parCY and sizing with parW2/parH2:
    @code{.json}
    {
      "components": [
        { "id": "header", "x": 0, "y": 0, "w": "parW", "h": "parH2" },
        { "id": "footer", "x": 0, "y": "parH2", "w": "parW", "h": "parH2" },
        { "id": "centerButton", "cx": "parW2", "cy": "parH2", "w": 100, "h": 30 }
      ]
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
        Load layout from several raw JSON string.

        @param rawJson  JSON strings containing layout definition
    */
    void setLayouts (const juce::StringArray& rawJson);


    /**
        Set or update a constant value.

        @param name   Constant name
        @param value  Constant value
    */
    void setConstant (const juce::String& name, int value);

    /**
        Get a top-level constant from the loaded layout files, merged in load
        order. Holds the values of the most recent layout pass, so it is only
        meaningful after a layout has been set.

        @param name          Constant name
        @param defaultValue  Value returned when the constant doesn't exist
    */
    double getConstant (const juce::String& name, double defaultValue) const;

    /**
        Find a resource by name

     */
    juce::String findContent (const juce::String& name);

    /** A component property: true leaves the child out of the parent's maxX / maxY
        (pW / pH), like the shadow layer BoxShadow adds
    */
    inline static const juce::Identifier excludeFromLayout { "excludeFromLayout" };

    /** Callback invoked when layout is loaded or reloaded. Every component is at its final
        bounds here, before "bounds" transitions move them back and start animating, so it
        may size the parent from them; a layout pass it triggers (e.g. through resized())
        doesn't cancel the transitions.
    */
    std::function<void()> layoutChanged;

    /** Callback invoked on every step of a layout driven transition ("bounds", "alpha", "scale"),
        with the component that moved or faded. For anything painted outside that component
        that follows it, like a drop shadow or a glow the parent paints.
    */
    std::function<void (juce::Component&)> transitionStep;

protected:
    juce::Component* getComp (const juce::String& cid);

    juce::Component& parent;

    struct Bounds
    {
        int x;
        int y;
        int w;
        int h;

        std::optional<juce::String>  postWidth;
        std::optional<juce::String>  postHeight;
    };

    struct JsonFile
    {
        juce::String name;
        juce::String contents;
    };

    void setLayoutInternal (const juce::Array<JsonFile>& files);

    int parse (const juce::var& equation, int equationIndex);
    double parseDouble (const juce::var& equation, int equationIndex);

    void setComponentsLayout (const juce::String& currentPath, const juce::var& components);

    void loadConstants (juce::var& j);
    void loadMacros (juce::var& j);
    void loadTransitions (juce::var& j);

    using BoundsRecords = std::map<juce::Component*, std::pair<juce::Component::SafePointer<juce::Component>, juce::Rectangle<int>>>;

    AnimatedProperties& animatedProperties (juce::Component& c);

    void recordBounds (juce::Component& c);
    void animateLayoutChanges (const BoundsRecords& changes);
    void setAlpha (juce::Component& c, float alpha);
    void setScale (juce::Component& c, float scale, const juce::var& origin);
    void setShadow (juce::Component& c, const juce::var& shadow);

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
    std::map<juce::String, double> topLevelConstants;
    std::map<juce::String, juce::var> macros;

    juce::var transitionPresets;
    BoundsRecords boundsBeforeLayout;
    std::map<juce::Component*, std::unique_ptr<Animated<juce::Rectangle<int>>>> boundsAnimations;
    std::vector<juce::Component::SafePointer<juce::Component>> animatedComponents;

    juce::Array<juce::File> layoutFiles;
    bool layoutSet = false;

    juce::Component* curComponent = nullptr;

    juce::OwnedArray<juce::Component> createdComponents;

    ComponentMap compMap;
    std::function<std::pair<juce::Component*, bool> (const juce::String&)> componentFactory;
};
