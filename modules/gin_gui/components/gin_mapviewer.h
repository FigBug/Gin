/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================*/
/**
    Interactive map viewer component using OpenStreetMap tiles.

    MapViewer displays an interactive map using tiles from OpenStreetMap servers.
    It supports panning, zooming (levels 0-18), and can be centered on any
    latitude/longitude coordinate. The component handles tile fetching, caching,
    and rendering automatically.

    Key Features:
    - Interactive panning via mouse drag
    - Zoom levels 0-18 (world to street level)
    - Mouse wheel zoom
    - Automatic tile fetching and caching
    - Smooth rendering with double buffering
    - Center on any lat/long coordinate

    Zoom Levels:
    - 0-2: World view
    - 3-6: Continent/country view
    - 7-10: State/region view
    - 11-14: City view
    - 15-18: Street/building view

    Usage:
    @code
    MapViewer mapViewer;
    mapViewer.setSize(800, 600);

    // Center on London
    mapViewer.centerOn(-0.1278, 51.5074); // longitude, latitude
    mapViewer.setZoom(12); // City level zoom

    // Or center on San Francisco
    mapViewer.centerOn(-122.4194, 37.7749);
    mapViewer.setZoom(13);
    @endcode

    @see OpenStreetMaps
*/
class MapViewer : public juce::Component,
                  private OpenStreetMaps::Listener
{
public:
    MapViewer();
    ~MapViewer() override;

    /** Zoom level between 0 and 18
    */
    void setZoom (int zoom);

    /** Centre map on long/lat
    */
    void centerOn (double longCenter, double latCenter);

    /** Set world coordinate (longlat) under view coordinate (x/y)
    */
    void centerUnderPt (juce::Point<double> world, juce::Point<int> view);

    OpenStreetMaps* getOpenStreetMaps() { return osm; }

protected:
    void resized() override;
    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    void tileFetched (int zoom, int x, int y) override;
    void updateMap();
    void mapUpdated();

private:
    void updateDoubleBuffer();
    void clearDoubleBuffer();

    int zoom;
    int mapsize;

    int xoffset;
    int yoffset;

    double wheelDelta = 0;

    juce::SharedResourcePointer<OpenStreetMaps> osm;

    juce::Point<int> lastPos;
    juce::Point<double> centerPt;

    bool userAdjusted;

    juce::Point<double> posMarker;

    std::unique_ptr<juce::Image> doubleBuffer;


};
