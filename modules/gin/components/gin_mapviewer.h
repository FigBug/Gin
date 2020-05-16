/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================*/
/**
    Draws an map, etting tiles fom one of many OpenStreetMapsServers
*/
class MapViewer : public Component,
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
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override;

    void tileFetched (int zoom, int x, int y) override;
    void updateMap();
    void mapUpdated();
    void preferencesChanged();
    void saveSnapshot();

private:
    void updateDoubleBuffer();
    void clearDoubleBuffer();

    int zoom;
    int mapsize;

    int xoffset;
    int yoffset;

    double wheelDelta = 0;

    SharedResourcePointer<OpenStreetMaps> osm;

    juce::Point<int> lastPos;
    juce::Point<double> centerPt;

    bool userAdjusted;

    juce::Point<double> posMarker;

    std::unique_ptr<Image> doubleBuffer;


};
