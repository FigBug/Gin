#pragma once

//==============================================================================
/**
    SVG path data strings for common UI icons.

    Assets namespace contains SVG path strings for standard UI icons used
    throughout the Gin plugin framework. These can be used with SVGButton,
    Path::parsePathString(), or DrawablePath to create scalable icons without
    requiring external image files.

    Available Icons:
    - Navigation: next, prev, add, del, browse
    - UI Controls: menu, info, caretDown
    - System: voice, cpu, power, panic, retrigger
    - File Operations: disk, pencil, undo, redo
    - Modulation: bipolar

    All icons are defined as SVG path strings that can be parsed and rendered
    at any size without loss of quality.

    Usage:
    @code
    // Create a next button
    auto nextButton = new SVGButton("Next", Assets::next);

    // Or use with Path
    juce::Path iconPath;
    iconPath.parsePathString(Assets::undo);
    g.fillPath(iconPath);

    // With DrawablePath
    auto drawable = new juce::DrawablePath();
    drawable->setPath(juce::Drawable::parseSVGPath(Assets::menu));
    @endcode

    @see SVGButton, juce::Path, juce::DrawablePath
*/
namespace Assets
{
extern juce::String next;
extern juce::String prev;
extern juce::String add;
extern juce::String del;
extern juce::String browse;
extern juce::String menu;
extern juce::String info;
extern juce::String voice;
extern juce::String cpu;
extern juce::String power;
extern juce::String panic;
extern juce::String retrigger;
extern juce::String caretDown;
extern juce::String disk;
extern juce::String pencil;
extern juce::String undo;
extern juce::String redo;
extern juce::String bipolar;
}
