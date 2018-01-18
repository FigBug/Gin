/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

//==============================================================================*/
// Fetches map files from OSM servers
class OpenStreetMaps
{
public:
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
    
    OpenStreetMaps();
	~OpenStreetMaps();

    /** Set this or map tiles will be stored in the temp directory */
    void setMapTileDir (File f) { mapTileDir = f; }
    
	Image fetchTile (int zoom, int x, int y);

	int getMapWidthPixels (int zoom);
	int getMapWidthTiles (int zoom);

    Point<double> coordinateToDisplay (Point<double> coordinate, int zoom);
    Point<double> displayToCoordinate (const Point<double> point, int zoom);
	Point<double> tileForCoordinate (double lat, double lng, int zoom);

	void clearQueue();

	TileSource getTileSource();
	void setTileSource (TileSource t);
    
    class Listener
    {
    public:
        virtual ~Listener() {}
        
        virtual void tileFetched (int zoom, int x, int y) = 0;
    };

    void addListener (Listener* listener)       { listeners.add (listener);     }
    void removeListener (Listener* listener)    { listeners.remove (listener);  }
    
private:
    void finished (AsyncDownload* task, MemoryBlock data, bool success);
    
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

        ScopedPointer<AsyncDownload> reply;
		int zoom;
		int x;
		int y;
        int server;
	};

	OwnedArray<TileReq> requests;
    OwnedArray<TileReq> cancelledRequests;
    
	File mapTileDir;
    HashMap<String, Image> cache;
	TileSource tileSource;
    Array<int> serversInUse;
    ListenerList<Listener> listeners;
    
    const int tilesize = 256;
};
