/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Adds an Array of components to a parent Component
 */
inline void addAndMakeVisible (juce::Component& parent, juce::Array<juce::Component*> children)
{
    for (auto child : children)
        parent.addAndMakeVisible (child);
}

/** Adds an std::initializer_list of components to a parent Component
*/
inline void addAndMakeVisible (juce::Component& parent, std::initializer_list<juce::Component*> children)
{
    for (auto child : children)
        parent.addAndMakeVisible ( child );
}
