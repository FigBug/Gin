/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Duration and easing of an animated change, CSS transition style.

    Transitions are opt-in: a layout or a component without transition definitions behaves
    exactly as without this class, every change applies at once.

    @section spec Spec strings

    A spec string is "<ms> <easing>", e.g. "250 inOutQuad": the duration in milliseconds
    (digits and an optional decimal point), whitespace, the easing name. Also valid:
    - "250"     a duration alone is linear
    - "0 ..."   a duration of 0 is instant
    - "none"    instant

    Easing names (case sensitive, implemented by the functions in gin_easing.h):
    - linear
    - inQuad, outQuad, inOutQuad
    - inCubic, outCubic, inOutCubic
    - inQuart, outQuart, inOutQuart
    - inQuint, outQuint, inOutQuint
    - inSine, outSine, inOutSine
    - inCirc, outCirc, inOutCirc
    - inExpo, outExpo, inOutExpo
    - inBack, outBack, inOutBack
    - inElastic, outElastic, inOutElastic
    - inBounce, outBounce, inOutBounce

    @section presets Presets

    A layout json's top-level "transitions" object names specs. Several layout files of one
    LayoutSupport merge their presets in load order. Presets are looked up from the animated
    component up through its parents, so nested layouts can define their own and still use
    their ancestors'. The preset name "default" is special: it applies to every animated
    property under that layout that no "transition" entry covers.

    @code{.json}
    "transitions": { "quick": "100 outQuad", "glide": "650 inOutCubic", "default": "250 inOutQuad" }
    @endcode

    @section entries Per component "transition"

    A component entry's "transition" is either
    - a string, a preset name, an inline spec or "none": applies to every animated property
      of the component
    - an object, one value per property name, like CSS transition-property:
      { "bounds": "glide", "alpha": "quick", "colour": "none" }

    Property names: "bounds" (the layout moving and resizing the component), "alpha" and
    "scale" (the layout's entries of that name and States' values; "scale" is also
    AnimatedScale's default), "imageFilter" (AnimatedImageFilter's default), and any name
    an Animated or Crossfade value in code was given.

    @section inheritance Inheritance

    The transition for a property of a component is the first match walking from the
    component up through its parents:
    - a string "transition" matches every property (so "none" stops the walk, instant)
    - an object "transition" matches the properties it lists; others continue upwards
    If nothing matches, the "default" preset applies, else the change is instant. A
    container's transition therefore applies to all its descendants unless they override it.

    An object may set what the descendants inherit instead with the key "descendants" (a
    string or a per-property object, same rules). It doesn't apply to the component itself,
    e.g. a page that slides in while the rows inside it keep jumping when they relayout:
    { "bounds": "glide", "alpha": "glide", "descendants": "none" }

    @code{.json}
    {
      "transitions": { "quick": "100 outQuad", "glide": "650 inOutCubic" },
      "components": [
        { "id": "panel", "transition": "glide", "components": [
            { "id": "title" },                                              // glide
            { "id": "knob", "transition": { "bounds": "quick" } },          // bounds quick, rest glide
            { "id": "meter", "transition": "none" },                        // instant
            { "id": "list", "transition": { "alpha": "quick", "descendants": { "bounds": "quick" } } }
                                        // list: alpha quick, bounds glide; its children: bounds quick, alpha glide
        ] }
      ]
    }
    @endcode

    @see Animated, Crossfade, LayoutSupport
*/
class Transition
{
public:
    /** An easing function, mapping linear progress 0..1 to eased progress */
    using Easing = float (*) (float);

    /** Creates an instant transition */
    Transition() = default;

    /** Creates a transition of the given duration in milliseconds and easing */
    Transition (double durationMs, Easing easing);

    /** Parses a spec string, "<ms> <easing>", "<ms>" or "none".
        @returns  std::nullopt for a malformed spec or an unknown easing name, also for a
                  preset name (use resolve() for those)
    */
    static std::optional<Transition> fromSpec (const juce::String& spec);

    /** Returns the easing function for a name like "inOutQuad", nullptr for an unknown name */
    static Easing findEasing (const juce::String& name);

    /** Resolves a preset name, an inline spec or "none". Presets are searched from the
        component up through its parents (the presets of each LayoutSupport live on the
        component it lays out). An unknown name asserts and is instant.
    */
    static Transition resolve (const juce::Component& c, const juce::String& nameOrSpec);

    /** Returns the transition a property of the component animates with, following the
        inheritance rules above: the nearest "transition" from the component up through its
        parents that covers the property, else the preset "default", else instant.
    */
    static Transition forProperty (const juce::Component& c, const juce::String& property);

    /** True for "none", a duration of 0 and a default constructed Transition */
    bool isInstant() const          { return durationMs <= 0.0 || easing == nullptr; }

    /** Returns the eased progress 0..1 at a time since the start; 1 from durationMs on and
        always for an instant transition
    */
    float getProgress (double elapsedMs) const;

    double durationMs = 0.0;
    Easing easing = nullptr;

    /** Sets a component's presets, exactly as a layout json's top-level "transitions" does
        for the component its LayoutSupport lays out: an object of name -> spec, or a JSON
        string of one. They apply to the component and everything inside it, also children
        added later. A void var clears them. Invalid specs assert. A LayoutSupport replaces the
        presets of the component it lays out on every pass, so set them on another component.

        For code-built UIs without a layout json above them:

        @code
        // The grid of game tiles, built in code: presets, timings and the tiles' hover state
        GameGrid()
        {
            gin::Transition::setPresets (*this, R"({ "hover": "60 inOutQuad", "fade": "300 outQuad" })");
            gin::Transition::set (*this, juce::JSON::parse (R"({ "descendants": { "scale": "hover", "imageFilter": "hover", "alpha": "fade" } })"));
            gin::States::setForChildren (*this, juce::JSON::parse (R"({ "hover": { "scale": 1.05, "imageFilter": "contrast 20" } })"));
        }

        // Each tile, added whenever; it inherits the grid's transitions and states
        class GameTile : public juce::Component
        {
        public:
            void paint (juce::Graphics& g) override             { cover.draw (g, getLocalBounds().toFloat()); }

        private:
            gin::AnimatedImageFilter cover { *this };   // its image set elsewhere
        };
        @endcode
    */
    static void setPresets (juce::Component& c, const juce::var& presets);

    /** The presets setPresets() or a layout set on exactly this component, void if none */
    static juce::var getPresets (const juce::Component& c);

    /** Sets a component's "transition", exactly as the layout entry of that name does: a
        string (preset name, inline spec or "none") or a per-property object, "descendants"
        included. Inherited by everything inside it as described above. A void var clears it.
        A layout pass that positions the component replaces it with the entry's own.
    */
    static void set (juce::Component& c, const juce::var& transition);

    /** The "transition" set on exactly this component, void if none */
    static juce::var get (const juce::Component& c);

    /** Switches every transition in the app on or off, e.g. off for software renderers or
        slow machines. Off, everything that would animate (Animated, Crossfade, AnimatedScale,
        AnimatedImageFilter, the layout's bounds, alpha and scale, States, box shadows) jumps
        straight to its target, its owner repainting once per change, and the vertical blank
        driver doesn't tick at all. Switching off makes running transitions jump to their
        targets at once. The initial value is GIN_TRANSITIONS_ENABLED_BY_DEFAULT: off on
        Linux (JUCE's software renderer only), on elsewhere, unless the app defines it.
    */
    static void setGlobalEnabled (bool enabled);
    static bool isGlobalEnabled();

    /** Component property holding a layout entry's "transition" */
    inline static const juce::Identifier componentProperty { "layoutTransition" };

    /** Component property holding the presets of the layout that positions its children */
    inline static const juce::Identifier presetsProperty { "layoutTransitions" };
};

/** How Animated blends and compares values of a type.

    The primary template works for any type with +, - and * float, like double or
    juce::Point<float>; specialise it for other types:

    @code
    template <>
    struct Interpolator<MyType>
    {
        static MyType blend (const MyType& a, const MyType& b, float p);
        static bool same (const MyType& a, const MyType& b);
    };
    @endcode
*/
template <typename T>
struct Interpolator
{
    static T blend (const T& a, const T& b, float p)    { return a + (b - a) * p; }
    static bool same (const T& a, const T& b)           { return a == b; }
};

/** Floats cover any scalar, a font height or a corner radius alike */
template <>
struct Interpolator<float>
{
    static float blend (float a, float b, float p)      { return a + (b - a) * p; }
    static bool same (float a, float b)                 { return juce::exactlyEqual (a, b); }
};

template <>
struct Interpolator<juce::Colour>
{
    static juce::Colour blend (juce::Colour a, juce::Colour b, float p)     { return a.interpolatedWith (b, p); }
    static bool same (juce::Colour a, juce::Colour b)                       { return a == b; }
};

/** Blends the edges, so a rectangle moving and resizing keeps its edges steady */
template <>
struct Interpolator<juce::Rectangle<float>>
{
    static juce::Rectangle<float> blend (juce::Rectangle<float> a, juce::Rectangle<float> b, float p);
    static bool same (juce::Rectangle<float> a, juce::Rectangle<float> b)   { return a == b; }
};

template <>
struct Interpolator<juce::Rectangle<int>>
{
    static juce::Rectangle<int> blend (juce::Rectangle<int> a, juce::Rectangle<int> b, float p);
    static bool same (juce::Rectangle<int> a, juce::Rectangle<int> b)       { return a == b; }
};

/** Blends the height; the other attributes are the target's */
template <>
struct Interpolator<juce::Font>
{
    static juce::Font blend (const juce::Font& a, const juce::Font& b, float p);
    static bool same (const juce::Font& a, const juce::Font& b)             { return a == b; }
};

/** Resamples both gradients onto the union of their stop positions and blends the colours
    there, and the end points. Radial or not is the target's.
*/
template <>
struct Interpolator<juce::ColourGradient>
{
    static juce::ColourGradient blend (const juce::ColourGradient& a, const juce::ColourGradient& b, float p);
    static bool same (const juce::ColourGradient& a, const juce::ColourGradient& b)     { return a == b; }
};

class AnimatedBase;

/** Ticks all running Animated and Crossfade values on the vertical blank of the window
    they show in. Holds a juce::VBlankAttachment per window only while something in it
    animates, so it costs nothing when idle. Shared through a juce::SharedResourcePointer;
    used internally, there is no need to touch it.
*/
class AnimationDriver : private juce::AsyncUpdater,
                        private juce::ComponentListener
{
public:
    AnimationDriver() = default;
    ~AnimationDriver() override;

    /** Starts ticking a value that just started a transition */
    void add (AnimatedBase& a);

    /** Stops ticking a value; safe from inside a tick */
    void remove (AnimatedBase& a);

    /** Moves every running value to its target and stops it */
    void finishAll();

private:
    void tick (juce::Component* window);
    void detach (juce::Component* window);
    bool isRunningIn (juce::Component* window) const;

    void handleAsyncUpdate() override;
    void componentBeingDeleted (juce::Component& c) override;

    std::vector<AnimatedBase*> running;
    std::map<juce::Component*, std::unique_ptr<juce::VBlankAttachment>> windows;
    juce::Component* ticking = nullptr;

    JUCE_DECLARE_NON_COPYABLE (AnimationDriver)
};

/** The non-template part of Animated and Crossfade: the owner, the property name, the
    running transition and the registration with the shared AnimationDriver.
*/
class AnimatedBase
{
public:
    AnimatedBase (juce::Component& owner, const juce::String& property);
    virtual ~AnimatedBase();

    /** True while a transition runs */
    bool isAnimating() const                    { return animating; }

    /** The owning component, nullptr once it was deleted */
    juce::Component* getOwner() const           { return owner.getComponent(); }

    /** The name the layout's "transition" entries know this value by */
    const juce::String& getProperty() const     { return property; }

    /** The transition setTarget (value) uses: the owner's for the property, see Transition::forProperty */
    Transition getTransition() const;

    /** Called on every change of the current value (each animation step, and jumps), after
        the owner was asked to repaint. Use it for anything beyond a repaint, e.g. moving
        child components. When a transition ends, isAnimating() is already false here.
    */
    std::function<void()> onChange;

protected:
    /** Returns false when the change has to apply at once: an instant transition or an owner not on screen */
    bool start (const Transition& t);
    void stop();
    void changed();

    /** Moves the current value to the eased progress between start and target */
    virtual void update (float progress) = 0;

    /** For subclasses that apply the value somewhere: called on every change, before onChange */
    virtual void valueChanged()                 {}

    /** The eased progress of the latest step of the running transition */
    float getLastProgress() const               { return lastProgress; }

private:
    friend class AnimationDriver;

    void tick (double nowMs);

    juce::Component::SafePointer<juce::Component> owner;
    juce::String property;

    Transition transition;
    double startMs = 0.0;
    float lastProgress = 0.0f;
    bool animating = false;

    juce::Component* window = nullptr;
    juce::SharedResourcePointer<AnimationDriver> driver;

    JUCE_DECLARE_NON_COPYABLE (AnimatedBase)
};

/** A value that eases to a new target over a Transition and repaints its owner on every step.

    - Setting a target starts from the current value, so retargeting mid-flight continues
      smoothly; setting the target it already heads for changes nothing.
    - Reading it (get() or the implicit conversion) returns the current value, so paint()
      just reads it.
    - A target set while the owner isn't showing, with an instant transition, or before a
      layout defined any transition, applies at once (setValue()).
    - Steps run on the vertical blank of the owner's window via a shared AnimationDriver,
      which goes idle when nothing animates.

    The transition comes from the layout json by the property name given to the
    constructor (see Transition for the json syntax and inheritance), or from a preset name
    or inline spec passed to setTarget().

    Any type with an Interpolator works: float (any scalar such as a font height or corner
    radius), double, juce::Colour, juce::Point<float>, juce::Rectangle<float>,
    juce::Rectangle<int>, juce::Font and juce::ColourGradient come with one.

    @code
    // layout json: "transitions": { "quick": "100 outQuad" },
    //              { "id": "panel", "transition": { "glow": "quick" } }
    class Panel : public juce::Component
    {
    public:
        void setLit (bool lit)
        {
            glow.setTarget (lit ? 1.0f : 0.0f);                 // the layout's "glow" transition
            tint.setTarget (lit ? juce::Colours::gold : juce::Colours::grey, "250 inOutQuad");
        }

        void paint (juce::Graphics& g) override
        {
            g.setColour (tint.get().withMultipliedAlpha (glow));
            g.fillEllipse (getLocalBounds().toFloat());
        }

    private:
        gin::Animated<float> glow { *this, "glow" };
        gin::Animated<juce::Colour> tint { *this, "tint", juce::Colours::grey };
    };
    @endcode

    @see Crossfade, Interpolator, Transition
*/
template <typename T>
class Animated : public AnimatedBase
{
public:
    /** @param owner     The component that repaints on every step and whose layout transitions apply
        @param property  The name the layout's "transition" object knows this value by
        @param initial   The starting value
    */
    Animated (juce::Component& owner, const juce::String& property = {}, T initial = T())
        : AnimatedBase (owner, property), value (initial), from (initial), target (initial)
    {
    }

    /** Returns the current, possibly mid-transition, value */
    const T& get() const            { return value; }

    /** Returns the current value, so an Animated<T> can be passed where a T is expected */
    operator const T&() const       { return value; }

    /** Returns the value the transition heads for, the current one when none runs */
    const T& getTarget() const      { return target; }

    /** Jumps to the value, stopping any transition. Calls onChange only if the value changed */
    void setValue (T newValue)
    {
        stop();

        const auto differs = ! same (value, newValue);
        value = from = target = newValue;

        if (differs)
            changed();
    }

    /** Eases to the value with the owner's transition for the property */
    void setTarget (T newTarget)
    {
        setTarget (newTarget, getTransition());
    }

    /** Eases to the value with a preset name (looked up from the owner up), an inline spec
        like "120 outQuad" or "none"
    */
    void setTarget (T newTarget, const juce::String& nameOrSpec)
    {
        if (auto c = getOwner())
            setTarget (newTarget, Transition::resolve (*c, nameOrSpec));
        else
            setValue (newTarget);
    }

    /** Eases to the value with an explicit Transition */
    void setTarget (T newTarget, const Transition& t)
    {
        if (same (target, newTarget) && (isAnimating() || same (value, newTarget)))
            return;

        if (same (value, newTarget) || ! start (t))
        {
            setValue (newTarget);
            return;
        }

        from = value;
        target = newTarget;
    }

    /** Changes the target of a running transition without restarting it: start value,
        start time and easing stay, so it still ends on time. For a target that moves
        continuously while a transition runs, e.g. a level dragged during a colour blend.
        Without a running transition it jumps (setValue()).
    */
    void updateTarget (T newTarget)
    {
        if (! isAnimating())
        {
            setValue (newTarget);
        }
        else if (! same (target, newTarget))
        {
            target = newTarget;
            update (getLastProgress());
            changed();
        }
    }

    /** For a target that streams in, like live colours or a meter: starts a transition when
        none runs, otherwise moves the running one's target (updateTarget()). The value keeps
        following without restarting on every update, and without lagging further behind
        than one transition.
    */
    void followTarget (T newTarget)
    {
        followTarget (newTarget, getTransition());
    }

    /** followTarget() with a preset name, an inline spec or "none" */
    void followTarget (T newTarget, const juce::String& nameOrSpec)
    {
        if (auto c = getOwner())
            followTarget (newTarget, Transition::resolve (*c, nameOrSpec));
        else
            setValue (newTarget);
    }

    /** followTarget() with an explicit Transition */
    void followTarget (T newTarget, const Transition& t)
    {
        if (isAnimating())
            updateTarget (newTarget);
        else
            setTarget (newTarget, t);
    }

private:
    static bool same (const T& a, const T& b)   { return Interpolator<T>::same (a, b); }

    void update (float progress) override
    {
        value = progress >= 1.0f ? target : Interpolator<T>::blend (from, target, progress);
    }

    T value, from, target;
};

//==============================================================================
/** Parses a CSS transform-origin style string into a point relative to a component's
    bounds, (0, 0) its top left, (1, 1) its bottom right:
    - keywords "center" (or "centre"), "left", "right", "top", "bottom", one or two in any
      order: "top left", "right", "bottom" (the other axis centred)
    - numbers as fractions or percentages, x then y: "0.5 0", "25% 100%"; a single number
      applies to both axes
    @returns  std::nullopt for anything else
*/
std::optional<juce::Point<float>> parseTransformOrigin (const juce::String& origin);

/** The transform that scales a component by s about a point relative to its bounds (see
    parseTransformOrigin), in its parent's coordinates as juce::Component::setTransform
    takes it. Identity for s == 1.
*/
juce::AffineTransform scaleTransform (juce::Rectangle<int> bounds, float s, juce::Point<float> origin = { 0.5f, 0.5f });

/** An Animated<float> scale applied to its owner with juce::Component::setTransform, about
    an origin relative to the owner's bounds (its centre by default), so it zooms from the
    centre instead of towards the bottom right. The pivot follows the owner when it moves
    or resizes, also during a "bounds" transition. A scale of 1 clears the transform.

    It owns the owner's transform: a transform set elsewhere is replaced on the next change.
    A component that needs its own transform too should be scaled through a parent or child.
    Hit testing follows the transform; JUCE repaints the enlarged area. Decorations painted
    by an ancestor (shadows) can follow it through a juce::ComponentListener on the owner:
    setTransform sends componentMovedOrResized (c, false, false).

    The layout's "scale" entry uses it as well (with "origin"); give a component a scale from
    the layout or from code, not both.

    @code
    // layout json: { "id": "tile", "transition": { "scale": "60 inOutQuad" } }
    class Tile : public juce::Button
    {
    public:
        void buttonStateChanged() override  { zoom.setTarget (isOver() ? 1.03f : 1.0f); }

    private:
        gin::AnimatedScale zoom { *this };
    };
    @endcode
*/
class AnimatedScale : public Animated<float>,
                      private juce::ComponentListener
{
public:
    /** @param owner     The component that scales
        @param property  The name the layout's "transition" object knows this value by
        @param initial   The starting scale
    */
    AnimatedScale (juce::Component& owner, const juce::String& property = "scale", float initial = 1.0f);
    ~AnimatedScale() override;

    /** Sets the pivot relative to the owner's bounds, (0.5, 0.5) its centre */
    void setOrigin (juce::Point<float> relativeOrigin);
    juce::Point<float> getOrigin() const        { return origin; }

private:
    void apply();

    void valueChanged() override                { apply(); }
    void componentMovedOrResized (juce::Component&, bool wasMoved, bool wasResized) override;

    juce::Point<float> origin { 0.5f, 0.5f };
};

/** A discrete value that can't blend, like a text or an icon: on a change the previous
    value fades out while the new one fades in, over a Transition found exactly like
    Animated's (layout "transition" by property name, preset, inline spec).

    Painting draws getPrevious() at an opacity of 1 - getProgress() and getCurrent() at
    getProgress(), or uses the drawText / drawIcon helpers which do just that. T needs
    copy assignment and operator==; juce::String and juce::Path are the usual ones.

    A new target while a fade runs restarts it from 0, with the value that showed more so
    far as the one fading out. updateTarget() changes the incoming value without a restart.

    @code
    // layout json: { "id": "status", "transition": { "caption": "150 inOutQuad" } }
    class Status : public juce::Component
    {
    public:
        void setOnline (bool online)    { caption.setTarget (online ? "Online" : "Offline"); }

        void paint (juce::Graphics& g) override
        {
            gin::drawText (g, caption, getLocalBounds().toFloat(), juce::Font (juce::FontOptions (15.0f)),
                           juce::Colours::white, juce::Justification::centred);
        }

    private:
        gin::Crossfade<juce::String> caption { *this, "caption", "Offline" };
    };
    @endcode

    @see Animated, drawText, drawIcon
*/
template <typename T>
class Crossfade : public AnimatedBase
{
public:
    /** @param owner     The component that repaints on every step and whose layout transitions apply
        @param property  The name the layout's "transition" object knows this value by
        @param initial   The starting value, shown alone
    */
    Crossfade (juce::Component& owner, const juce::String& property = {}, T initial = T())
        : AnimatedBase (owner, property), current (initial), previous (initial)
    {
    }

    /** The incoming value, the only one once the fade ends */
    const T& getCurrent() const     { return current; }

    /** The outgoing value; equals getCurrent() when no fade runs */
    const T& getPrevious() const    { return previous; }

    /** 0 shows only the previous value, 1 only the current one */
    float getProgress() const       { return progress; }

    /** Paints both sides of the fade with your own drawing: calls paintOne (value, opacity)
        for the previous value at 1 - progress and the current one at progress, skipping a
        side at opacity 0. Multiply your colours' alpha by the opacity.

        @code
        weight.draw ([&] (int w, float opacity)
        {
            g.setColour (juce::Colours::white.withMultipliedAlpha (opacity));
            g.setFont (juce::FontOptions (14.0f).withStyle (w > 500 ? "Bold" : "Regular"));
            g.drawText (label, area, juce::Justification::centred);
        });
        @endcode
    */
    template <typename PaintFn>
    void draw (PaintFn&& paintOne) const
    {
        if (progress < 1.0f)
            paintOne (previous, 1.0f - progress);

        if (progress > 0.0f)
            paintOne (current, progress);
    }

    /** Jumps to the value, stopping any fade */
    void setValue (T newValue)
    {
        stop();

        const auto differs = ! same (current, newValue) || progress < 1.0f;

        current = previous = newValue;
        progress = 1.0f;

        if (differs)
            changed();
    }

    /** Changes the incoming value without restarting a running fade, e.g. a counter that
        keeps changing while it fades in. Without a fade it jumps.
    */
    void updateTarget (T newValue)
    {
        if (! isAnimating())
        {
            setValue (newValue);
        }
        else if (! same (current, newValue))
        {
            current = newValue;
            changed();
        }
    }

    /** Fades to the value with the owner's transition for the property */
    void setTarget (T newTarget)
    {
        setTarget (newTarget, getTransition());
    }

    /** Fades to the value with a preset name (looked up from the owner up), an inline spec
        like "150 inOutQuad" or "none"
    */
    void setTarget (T newTarget, const juce::String& nameOrSpec)
    {
        if (auto c = getOwner())
            setTarget (newTarget, Transition::resolve (*c, nameOrSpec));
        else
            setValue (newTarget);
    }

    /** Fades to the value with an explicit Transition */
    void setTarget (T newTarget, const Transition& t)
    {
        if (same (current, newTarget))
            return;

        // Mid fade, the value that shows more stays as the one fading out
        auto outgoing = progress >= 0.5f ? current : previous;

        if (! start (t))
        {
            setValue (newTarget);
            return;
        }

        previous = std::move (outgoing);
        current = newTarget;
        progress = 0.0f;
        changed();
    }

private:
    static bool same (const T& a, const T& b)   { return a == b; }

    void update (float p) override
    {
        progress = p;
    }

    T current, previous;
    float progress = 1.0f;
};

//==============================================================================
/** @name Paint helpers

    They take plain values, so Animated<T> members pass straight in through their implicit
    conversion; the Crossfade overloads draw both sides of a fade.

    @code
    class Card : public juce::Component
    {
    public:
        void paint (juce::Graphics& g) override
        {
            auto rc = getLocalBounds().toFloat();

            gin::fillGradient (g, rc, 12.0f, fill);
            gin::drawIcon (g, icon, rc.removeFromLeft (rc.getHeight()).reduced (8.0f), ink);
            gin::drawText (g, title, rc, juce::Font (juce::FontOptions (20.0f)), ink, juce::Justification::centredLeft);
        }

        gin::Animated<juce::ColourGradient> fill { *this, "fill" };     // relative points, see fillGradient
        gin::Animated<juce::Colour> ink { *this, "ink", juce::Colours::white };
        gin::Crossfade<juce::Path> icon { *this, "icon" };              // relative paths, see drawIcon
        gin::Crossfade<juce::String> title { *this, "title" };
    };
    @endcode
*/
///@{

/** Fills a rectangle, rounded when cornerRadius > 0, with a gradient whose points are
    relative to it: (0, 0) is its top left, (1, 1) its bottom right. Relative points keep an
    Animated<juce::ColourGradient> independent of the component's size. A gradient without
    colours draws nothing.
*/
void fillGradient (juce::Graphics& g, juce::Rectangle<float> area, float cornerRadius, const juce::ColourGradient& relativeGradient);

/** Draws a Crossfade<juce::String> with juce::Graphics::drawText (ellipsis on overflow): the
    previous text at the colour's alpha times 1 - progress, the current one times progress.
    Empty texts are skipped.
*/
void drawText (juce::Graphics& g, const Crossfade<juce::String>& text, juce::Rectangle<float> area,
               const juce::Font& font, juce::Colour colour, juce::Justification justification);

/** Fills a path whose coordinates are relative: (0, 0)..(1, 1) maps onto the largest square
    centred in the area. Make a relative path from an SVG or a Drawable by fitting it into
    the rectangle 0, 0, 1, 1 once.
*/
void drawIcon (juce::Graphics& g, const juce::Path& relativePath, juce::Rectangle<float> area, juce::Colour colour);

/** Draws a Crossfade<juce::Path> of relative paths (see the other drawIcon), the previous
    icon fading out as the current one fades in
*/
void drawIcon (juce::Graphics& g, const Crossfade<juce::Path>& icon, juce::Rectangle<float> area, juce::Colour colour);

/** Draws a Crossfade<MipMap> with drawBlend: the previous image at full opacity, the
    current one on top at the fade's progress, so an opaque image never shows through
    to the background mid-fade
*/
void drawImage (juce::Graphics& g, const Crossfade<MipMap>& image, juce::Rectangle<float> area,
                juce::RectanglePlacement placement = 0);

///@}

//==============================================================================
/** A chain of image filters (as CSS filter), applied in order with Gin's applyX functions,
    parsed from a spec string of names, each followed by its value where it takes one.
    It applies only to images drawn through the image helpers (AnimatedImageFilter,
    drawBlend), not to arbitrary components:

    | Name          | Value                                  | Function                                     |
    |---------------|----------------------------------------|----------------------------------------------|
    | brightness    | -100..100, 0 = none                    | applyBrightnessContrast (img, v, 0)          |
    | contrast      | -100..100, 0 = none                    | applyBrightnessContrast (img, 0, v)          |
    | hue           | -180..180 degrees                      | applyHueSaturationLightness (img, v, 100, 0) |
    | saturation    | 0..200, 100 = none                     | applyHueSaturationLightness (img, 0, v, 0)   |
    | lightness     | -100..100, 0 = none                    | applyHueSaturationLightness (img, 0, 100, v) |
    | gamma         | above 0, 1 = none                      | applyGamma                                   |
    | sharpen       | (none)                                 | applySharpen                                 |
    | soften        | (none)                                 | applySoften                                  |
    | greyscale     | (none)                                 | applyGreyScale                               |
    | sepia         | (none)                                 | applySepia                                   |
    | invert        | (none)                                 | applyInvert                                  |

    e.g. "contrast 30 saturation 130" or "greyscale brightness -20".

    @see AnimatedImageFilter
*/
class ImageFilter
{
public:
    ImageFilter() = default;

    /** @returns  std::nullopt for an unknown name or a missing or malformed value */
    static std::optional<ImageFilter> fromSpec (const juce::String& spec);

    /** Applies the chain to the image in place */
    void apply (juce::Image& img) const;

    bool isEmpty() const                                    { return steps.empty(); }
    bool operator== (const ImageFilter& other) const        { return steps == other.steps; }

private:
    std::vector<std::pair<juce::String, float>> steps;
};

/** An image and an ImageFilter version of it, blended by an Animated<float> amount: 0
    shows the image, 1 the filtered one, e.g. a contrast boost while hovered. Only what is
    drawn through draw() is filtered, never the owner's other painting or its children.

    The filter is applied once to the full size image and a second MipMap built from the
    result; that happens again only when the image or the filter changes, so painting at
    any size or scale (HiDPI) costs two MipMap::draw calls at most (see drawBlend).

    The amount's transition comes from the layout like any Animated's ("imageFilter" by
    default). The owner's "states" (see States) drive it: a state with an "imageFilter"
    spec sets the filter and eases the amount to 1 while it is active, back to 0 after:

    @code
    // layout json: { "id": "cover", "states": { "hover": { "imageFilter": "contrast 30 saturation 120" } },
    //                "transition": { "imageFilter": "150 outQuad" } }
    class Cover : public juce::Component
    {
    public:
        Cover()                                     { image.setImage (gin::MipMap (loadCover())); }
        void paint (juce::Graphics& g) override     { image.draw (g, getLocalBounds().toFloat()); }

    private:
        gin::AnimatedImageFilter image { *this };
    };
    @endcode

    Without states, set the filter and drive amount yourself.
*/
class AnimatedImageFilter
{
public:
    /** @param owner     The component that repaints on every step, whose layout transitions and states apply
        @param property  The name the layout's "transition" object knows the amount by
    */
    AnimatedImageFilter (juce::Component& owner, const juce::String& property = "imageFilter");
    ~AnimatedImageFilter();

    /** Sets the image; the filtered version is built again if it differs */
    void setImage (const MipMap& image);

    /** Sets the filter; the filtered version is built again if it differs. An invalid spec
        asserts and counts as no filter.
    */
    void setFilter (const ImageFilter& filter);
    void setFilter (const juce::String& spec);

    /** Draws the blend at the amount's current value */
    void draw (juce::Graphics& g, juce::Rectangle<float> area, juce::RectanglePlacement placement = 0) const;

    const MipMap& getImage() const                  { return image; }
    const MipMap& getFilteredImage() const          { return filtered; }

    /** 0 = the image, 1 = the filtered one */
    Animated<float> amount;

private:
    void rebuild();

    juce::Component::SafePointer<juce::Component> owner;
    MipMap image, filtered;
    ImageFilter filter;

    JUCE_DECLARE_NON_COPYABLE (AnimatedImageFilter)
};
