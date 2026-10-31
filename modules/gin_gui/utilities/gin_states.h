/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class AnimatedImageFilter;

/** The animated values of one component that the layout and its States share, so a base
    value (the layout's "alpha" and "scale" entries) and a state's value (e.g. a hover
    scale) drive the same animation instead of fighting over it. A state's value wins
    while the state is active; leaving it eases back to the base value. Values:
    - "alpha" (Component::setAlpha, 0 hides, see LayoutSupport), base 1 without an entry
    - "scale" (an AnimatedScale about the "origin"), base 1 without an entry
    - "imageFilter" (a spec, see ImageFilter): sets the filter of every AnimatedImageFilter
      the component owns and eases their amount to 1, to 0 when no state sets one
    - "boxShadow" (see BoxShadow), base none without an entry

    Timings come from Transition::forProperty for "alpha", "scale" and the filters' own
    property names ("imageFilter" by default). Kept in the component's properties, it
    lives as long as the component. Created on demand; LayoutSupport, States and
    AnimatedImageFilter use it, code rarely needs to.
*/
class AnimatedProperties : public juce::ReferenceCountedObject
{
public:
    /** The component's, created when it has none */
    static AnimatedProperties& get (juce::Component& c);

    /** The component's, nullptr when it has none */
    static AnimatedProperties* find (const juce::Component& c);

    ~AnimatedProperties() override;

    /** The base values; animate false applies at once (a component's first layout) */
    void setBaseAlpha (float alpha, bool animate);
    void setBaseScale (float scale, juce::Point<float> origin, bool animate);

    /** The active states' values merged into one object ("alpha", "scale", "imageFilter"),
        void when no state is active
    */
    void setStateValues (const juce::var& values);
    const juce::var& getStateValues() const             { return stateValues; }

    void addImageFilter (AnimatedImageFilter& f);
    void removeImageFilter (AnimatedImageFilter& f);

    /** The box shadow's base value and outline radius (see BoxShadow) */
    void setBaseShadow (const ShadowList& shadow, bool animate);
    void setShadowRadius (float radius);

    /** The shadow as it shows now, mid-transition included */
    const ShadowList& getShadow() const;
    float getShadowRadius() const                       { return shadowRadius; }

    /** Called with the component on every step of its alpha or scale */
    std::function<void (juce::Component&)> onStep;

private:
    explicit AnimatedProperties (juce::Component& c);

    void applyAlpha (bool animate);
    void applyScale (bool animate);
    void applyImageFilters();
    void applyShadow (bool animate);
    void repaintShadow();

    // The parent paints the shadow: a new parent gets the layer for it
    struct ParentWatch : public juce::ComponentListener
    {
        void componentParentHierarchyChanged (juce::Component&) override;
        AnimatedProperties* owner = nullptr;
    };

    juce::Component::SafePointer<juce::Component> owner;

    std::optional<float> baseAlpha, baseScale;
    juce::Point<float> origin { 0.5f, 0.5f };
    juce::var stateValues;

    std::unique_ptr<Animated<float>> alpha;
    std::unique_ptr<AnimatedScale> scale;
    bool clicksSuppressed = false, selfClicks = true, childClicks = true;

    std::vector<AnimatedImageFilter*> imageFilters;

    std::optional<ShadowList> baseShadow;
    float shadowRadius = 0.0f;
    std::unique_ptr<Animated<ShadowList>> shadow;
    ParentWatch parentWatch;

    JUCE_DECLARE_NON_COPYABLE (AnimatedProperties)
};

//==============================================================================
/** Declarative states, like CSS pseudo-classes: values a component takes while the mouse
    is over it or presses it, eased there and back with its transitions.

    @section json Layout json

    @code{.json}
    { "id": "card", "states": { "hover": { "scale": 1.03 }, "pressed": { "scale": 0.98, "alpha": 0.9 } } },
    { "id": "grid", "children": { "states": { "hover": { "scale": 1.05, "imageFilter": "contrast 20 brightness 10" } } },
      "transition": { "descendants": { "scale": "60 inOutQuad", "imageFilter": "150 outQuad" } } }
    @endcode

    - "states" on an entry applies to the component itself.
    - "children": { "states": ... } applies to each DIRECT child (CSS .grid > *), also
      children added later, e.g. tiles built at runtime. A child with its own "states"
      keeps those instead.
    - States never inherit any further down: a tile's inner image or label doesn't get a
      zoom or a filter of its own on top of the tile's.
    - State names: "hover" (the mouse over the component or anything inside it, so moving
      across the tile's content doesn't flicker), "pressed" (a mouse button down on it),
      "disabled" (the component disabled; while disabled, hover and pressed don't apply).
      Pressed values win over hover values, key by key.
    - Keys: "alpha", "scale", "imageFilter", "boxShadow" (see AnimatedProperties). A key no active
      state sets returns to its base value: the entry's own "alpha" or "scale", else 1;
      the image filters' amount to 0.
    - Timing per key comes from "transition" as everywhere else, "descendants" included.

    A layout pass that positions a component replaces its "states" and "children" with
    the entry's own (none when the entry has none).

    @section code Code

    The same from code, for UIs without a layout json; the layout uses these setters too:

    @code
    gin::States::setForChildren (grid, juce::JSON::parse (R"({ "hover": { "scale": 1.05 }, "pressed": { "scale": 0.98 } })"));
    gin::States::set (closeButton, juce::JSON::parse (R"({ "hover": { "alpha": 1 } })"));
    @endcode

    @see Transition, AnimatedProperties, AnimatedImageFilter
*/
class States
{
public:
    /** Sets the component's own states: an object of state name -> values, void clears */
    static void set (juce::Component& c, const juce::var& states);

    /** The component's own states, void if none */
    static juce::var get (const juce::Component& c);

    /** Sets the states of each direct child of the container, current and later ones;
        void clears
    */
    static void setForChildren (juce::Component& container, const juce::var& states);

    /** The states set for the container's children, void if none */
    static juce::var getForChildren (const juce::Component& container);

    /** The values of the states that apply to a component that is hovered, pressed or
        disabled, merged as AnimatedProperties takes them; void when none applies
    */
    static juce::var merge (const juce::var& states, bool hovered, bool pressed, bool disabled);
};
