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

class OpenStreetMaps : private URL::DownloadTask::Listener
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
    void finished (URL::DownloadTask* task, bool success) override;
    
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

        ScopedPointer<URL::DownloadTask> reply;
        File temp;
		int zoom;
		int x;
		int y;
        int server;
	};

	OwnedArray<TileReq> requests;
	File mapTileDir;
    HashMap<String, Image> cache;
	TileSource tileSource;
    Array<int> serversInUse;
    ListenerList<Listener> listeners;
    
    const int tilesize = 256;
};
