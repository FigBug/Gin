/*
  ==============================================================================

    MapDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct MapDemo : public DemoComponent
{
    MapDemo()
    {
        setName ("OpenStreetMaps");
        addAndMakeVisible (map);
    }

    void resized() override
    {
        map.setBounds (getLocalBounds());
    }

    gin::MapViewer map;
};
