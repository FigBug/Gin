/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

enum TileSource
{
	OpenStreetMap,
	OpenCycleMap,
	OpenCycleMapTransport,
	OpenCycleMapLandscape,
	StamenTerrain,
    MapQuestOSM,
    MapQuestOpenAerial,
    MapQuestOpenStreetMap,
};

class OpenStreetMaps
{
public:
    OpenStreetMaps();
	~OpenStreetMaps();

	Image fetchTile (int zoom, int x, int y);

	int getMapWidthPixels (int zoom);
	int getMapWidthTiles (int zoom);

    Point<double> coordinateToDisplay (Point<double> coordinate, int zoom);
    Point<double> displayToCoordinate (const Point<double> point, int zoom);
	Point<double> tileForCoordinate (double lat, double lng, int zoom);

	void clearQueue();

	TileSource getFileSource();
	void setTileSource(TileSource t);

private:
	void startRequest();
    int getNumServers();
    int getServer();

	class TileReq
	{
	public:
		TileReq (int z_, int x_, int y_)
		{
			zoom    = z_;
			x       = x_;
			y       = y_;
            server  = -1;
		}

		bool operator==(const TileReq& b)
		{
			return zoom == b.zoom && x == b.x && y == b.y;
		}

		int zoom;
		int x;
		int y;
        int server;
        
	};

	Array<TileReq> requests;

	File mapTileDir;

    //std::map<String,Image> cache;

	TileSource tileSource;

    Array<int> serversInUse;
};
