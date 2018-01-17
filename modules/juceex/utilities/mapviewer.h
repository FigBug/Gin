/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

class MapViewer : public Component
{
public:
    MapViewer();
	~MapViewer();

	void setZoom (int zoom);
	void centerOn (double longCenter, double latCenter);
	void centerUnderPt (Point<double> world, Point<int> view);

protected:
    void resized() override;
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
	void mouseUp (const MouseEvent& e) override;
	void mouseMove (const MouseEvent& e) override;
    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override;

	void tileFetched (int zoom, int x, int y);
	void updateMap();
    void mapUpdated();
    void preferencesChanged();
    void saveSnapshot();

private:
	void autoZoomAndCenter();
    void updateDoubleBuffer();
    void clearDoubleBuffer();

	int zoom;
	int mapsize;

	int xoffset;
	int yoffset;

	int selectedTrack;

	OpenStreetMaps* osm;

	Point<double> lastPos;

	Point<double> centerPt;

	bool userAdjusted;

	Point<double> posMarker;

    QImage* doubleBuffer;
};
