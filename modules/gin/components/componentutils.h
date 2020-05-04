/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

inline void addAndMakeVisible (Component& parent, Array<Component*> children)
{
    for (auto child : children)
        parent.addAndMakeVisible (child);
}

inline void addAndMakeVisible (Component& parent, std::initializer_list<Component*> children)
{
    for (auto child : children)
        parent.addAndMakeVisible ( child );
}
