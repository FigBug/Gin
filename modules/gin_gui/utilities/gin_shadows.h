/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** One layer of a CSS-style box shadow: an offset, a blur radius, a spread and a colour
    (its alpha the shadow's opacity). How the blur looks is the renderer's (see BoxShadow).
*/
struct ShadowSpec
{
    juce::Point<float> offset;
    float blur = 0.0f;
    float spread = 0.0f;
    juce::Colour colour { juce::Colours::black };

    // an exact comparison of the float members is what is wanted here
    bool operator== (const ShadowSpec& other) const
    {
        return offset == other.offset
            && juce::exactlyEqual (blur, other.blur)
            && juce::exactlyEqual (spread, other.spread)
            && colour == other.colour;
    }

    bool operator!= (const ShadowSpec& other) const  { return ! operator== (other); }
};

/** A box shadow's layers, painted in order */
using ShadowList = std::vector<ShadowSpec>;

/** Blends layer by layer; a missing layer blends from or to a clear copy of the other one */
template <>
struct Interpolator<ShadowList>
{
    static ShadowList blend (const ShadowList& a, const ShadowList& b, float p);
    static bool same (const ShadowList& a, const ShadowList& b)      { return a == b; }
};

//==============================================================================
/** CSS-style box shadows, painted by the PARENT under its children, because a shadow lies
    outside the component and would be clipped by it.

    @section json Layout json

    @code{.json}
    { "id": "card", "boxShadow": "0 4 8 0.45, 0 12 30 0.6", "borderRadius": 14,
      "states": { "hover": { "boxShadow": "0 6 10 0.5, 0 16 36 0.65" } },
      "transition": { "boxShadow": "60 inOutQuad" } }
    @endcode

    - "boxShadow": one or more layers separated by commas (or a json array of strings),
      each "<x> <y> <blur> <opacity> [<spread>] [<colour>]": offset and blur in pixels,
      opacity 0..1 multiplying the colour's alpha, the colour black by default ("#ff3366",
      "ff00ff00" or a JUCE colour name like "red"). "none" or "" for no shadow.
    - "borderRadius": the corner radius of the outline the shadow follows, number or
      expression (e.g. "pillH / 2").
    - Animated with the "boxShadow" transition, usable in "states" like "alpha" and "scale".
    - The outline is the component's bounds with that radius, following its scale
      transform, its bounds transitions and its alpha.

    @section painting Painting

    The first shadow in a container adds an invisible layer as its first child (at the
    back, taking no clicks, ignored by the layout's maxX/maxY), which paints the shadows of
    all its siblings; so "zorder" indices count it. A container that clips (most do) needs
    room around shadowed children for the blur.

    @section renderer Renderer

    Gin draws no blur itself: install a renderer once, e.g. at app start, otherwise no
    shadows are drawn at all:

    @code
    gin::BoxShadow::setRenderer ([] (juce::Graphics& g, const juce::Path& outline, const gin::ShadowSpec& s)
    {
        // a blur library's drop shadow for outline with s.offset, s.blur, s.spread, s.colour
    });
    @endcode

    @section code Code

    @code
    gin::BoxShadow::set (tile, "0 3 6 0.45, 0 8 24 0.6");
    gin::BoxShadow::setRadius (tile, 14.0f);
    @endcode
*/
class BoxShadow
{
public:
    using Renderer = std::function<void (juce::Graphics&, const juce::Path& outline, const ShadowSpec&)>;

    /** Parses a box shadow: a string of comma separated layers or an array of layer strings.
        @returns  std::nullopt when malformed; an empty list for "none", "" or void
    */
    static std::optional<ShadowList> parse (const juce::var& shadow);

    /** The renderer for every container that has none of its own; nullptr = no shadows */
    static void setRenderer (Renderer renderer);

    /** A renderer for the shadows of this container's children; nullptr = the global one */
    static void setRenderer (juce::Component& container, Renderer renderer);

    /** The component's shadow, as the layout's "boxShadow" entry sets it; void clears */
    static void set (juce::Component& c, const juce::var& shadow);

    /** The corner radius of the outline the shadow follows, as "borderRadius" sets it */
    static void setRadius (juce::Component& c, float radius);

    /** Paints the shadows of the container's children in its coordinates, as its layer does */
    static void paintChildShadows (juce::Graphics& g, juce::Component& container);
};
