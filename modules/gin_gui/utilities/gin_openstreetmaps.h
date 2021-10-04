/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================*/
/** Fetches map files from OSM servers
 */
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
    void setMapTileDir (juce::File f) { mapTileDir = f; }

    juce::Image fetchTile (int zoom, int x, int y);

    int getMapWidthPixels (int zoom);
    int getMapWidthTiles (int zoom);

    juce::Point<double> coordinateToDisplay (juce::Point<double> coordinate, int zoom);
    juce::Point<double> displayToCoordinate (const juce::Point<double> point, int zoom);
    juce::Point<double> tileForCoordinate (double lat, double lng, int zoom);

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
    void finished (AsyncDownload* task, juce::MemoryBlock data, bool success);

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

        bool operator==(const TileReq& b) const
        {
            return zoom == b.zoom && x == b.x && y == b.y;
        }

        std::unique_ptr<AsyncDownload> reply;
        int zoom;
        int x;
        int y;
        int server;
    };

    juce::OwnedArray<TileReq> requests;
    juce::OwnedArray<TileReq> cancelledRequests;

    juce::File mapTileDir;
    juce::HashMap<juce::String, juce::Image> cache;
    TileSource tileSource;
    juce::Array<int> serversInUse;
    juce::ListenerList<Listener> listeners;

    const int tilesize = 256;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenStreetMaps)
};
