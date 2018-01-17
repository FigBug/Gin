/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

OpenStreetMaps::OpenStreetMaps()
  : tileSource (OpenCycleMapLandscape)
{
    mapTileDir = File::getSpecialLocation (File::tempDirectory).getChildFile ("mapTiles");
}

OpenStreetMaps::~OpenStreetMaps()
{
    for (int i = 0; i < requests.size(); i++)
    {
        if (requests[i].reply)
        {
            requests[i].reply->abort();
            requests[i].reply = NULL;
        }
    }
}

Image OpenStreetMaps::fetchTile (int zoom, int x, int y)
{
	int mw = getMapWidthTiles (zoom);
	x = x % mw;
	y = y % mw;

    String fname = String::formatted ("%d-%d-%d-%d.png", (int)tileSource, zoom, x, y);

	QString file = mapTileDir.getChildFile (fname);

	if (cache.contains (fname))
	{
		return cache.value (fname);
	}
	else if (File (file).existsAsFile())
	{
		Image img;
		img.load(file);

		cache.insert(fname, img);

		return img;
	}
	else
	{
		TileReq newReq (zoom, x, y);

		bool pending = false;
		for (int i = 0; i < requests.size(); i++)
		{
			if (requests[i] == newReq)
			{
				pending = true;
				break;
			}
		}

		if (!pending && manager->networkAccessible() != QNetworkAccessManager::NotAccessible)
		{
			requests.append(newReq);
			startRequest();
		}

		Image img(QSize(256,256), QImage::Format_RGB32);
		img.fill(0x808080);
		return img;
	}
}

int OpenStreetMaps::getNumServers()
{
	switch (tileSource)
	{
		case OpenStreetMap:         return 3;
		case OpenCycleMap:          return 3;
		case OpenCycleMapTransport: return 3;
		case OpenCycleMapLandscape: return 3;
		case StamenTerrain:         return 1;
        case MapQuestOSM:           return 4;
        case MapQuestOpenAerial:    return 4;
        case MapQuestOpenStreetMap: return 3;
		default:                    return 0;
	}
}

int OpenStreetMaps::getServer()
{
    for (int i = 0; i < getNumServers(); i++)
    {
        if (! serversInUse.contains(i))
            return i;
    }
    return -1;
}

void OpenStreetMaps::startRequest()
{
    int server = getServer();
    if (server == -1)
        return;

    for (int i = 0; i < requests.size(); i++)
    {
        if (requests[i].reply == NULL)
        {
            char buffer[1024];
            switch (tileSource)
            {
                case OpenStreetMap:
                    sprintf(buffer, "http://%c.tile.openstreetmap.org/%d/%d/%d.png", "abc"[server], requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                case OpenCycleMap:
                    sprintf(buffer, "http://%c.tile.opencyclemap.org/cycle/%d/%d/%d.png", "abc"[server], requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                case OpenCycleMapTransport:
                    sprintf(buffer, "http://%c.tile2.opencyclemap.org/transport/%d/%d/%d.png", "abc"[server], requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                case OpenCycleMapLandscape:
                    sprintf(buffer, "http://%c.tile3.opencyclemap.org/landscape/%d/%d/%d.png", "abc"[server], requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                case StamenTerrain:
                    sprintf(buffer, "http://tile.stamen.com/terrain/%d/%d/%d.png", requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                case MapQuestOSM:
                    sprintf(buffer, "http://otile%c.mqcdn.com/tiles/1.0.0/map/%d/%d/%d.jpg", "1234"[server], requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                case MapQuestOpenAerial:
                    sprintf(buffer, "http://otile%c.mqcdn.com/tiles/1.0.0/sat/%d/%d/%d.jpg", "1234"[server], requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                case MapQuestOpenStreetMap:
                    sprintf(buffer, "http://%c.tile.openstreetmap.org/%d/%d/%d.png", "abc"[server], requests[i].zoom, requests[i].x, requests[i].y);
                    break;
                default:
                    Q_ASSERT(false);
                    break;
            }

            requests[i].server     = server;

            serversInUse.append(server);

            QUrl url = QUrl(QString(buffer));
            QNetworkRequest nr(url);

            requests[i].reply = manager->get(nr);

            break;
        }
    }
}

void OpenStreetMaps::replyFinished(QNetworkReply* reply)
{
    for (int i = 0; i < requests.size(); i++)
    {
        if (requests[i].reply == reply)
        {
            requests[i].reply->deleteLater();
            requests[i].reply = NULL;

            serversInUse.removeOne(requests[i].server);
            requests[i].server = -1;

	        if (reply->error() == QNetworkReply::NoError)
	        {
		        QByteArray data = reply->readAll();

		        QString fname;
		        fname.sprintf("%d-%d-%d-%d.png", (int)tileSource, requests[i].zoom, requests[i].x, requests[i].y);

		        QImage img;
		        if (img.loadFromData(data))
		        {
			        cache.insert(fname, img);

                    img.save(mapTileDir.filePath(fname), "PNG");

			        emit tileFetched(requests[i].zoom, requests[i].x, requests[i].y);
		        }

                requests.removeAt(i);
	        }
            break;
        }
    }
	startRequest();
}

int OpenStreetMaps::getMapWidthPixels (int zoom)
{
	double numberOfTiles = pow(2.0, zoom);
	return doubleToInt(numberOfTiles * 256);
}

int OpenStreetMaps::getMapWidthTiles (int zoom)
{
    return roundToInt (std::pow (2.0, zoom));
}

Point<double> OpenStreetMaps::coordinateToDisplay (Point<double> coordinate, int zoom)
{    
    double numberOfTiles = std::pow (2.0, zoom);

    // LonToX
	double x = (coordinate.x() + 180) * (numberOfTiles * tilesize) / 360.0;
    // LatToY
    double projection = log(tan(M_PI / 4 + deg2rad(coordinate.y()) /2));

    double y = (projection / M_PI);
    y = 1 - y;
    y = y /2  * (numberOfTiles*tilesize);

    return Point<double> (x, y);
}

Point<double> OpenStreetMaps::displayToCoordinate (Point<double> point, int zoom)
{
    // longitude
    double longitude = (point.x() * (360 / (pow(2.0,zoom) * 256))) - 180;
    // latitude
    double latitude = point.y() * (2 / (pow(2.0,zoom) * 256));

    latitude = 1 - latitude;
    latitude = latitude * M_PI;
    latitude = rad2deg(atan(sinh(latitude)));
    
    QPointF coord = QPointF(longitude, latitude);
    return coord;
}

Point<double> OpenStreetMaps::tileForCoordinate (double lat, double lng, int zoom)
{ 
	double zn = static_cast<double>(1 << zoom);
	double tx = (lng + 180.0) / 360.0;
	double ty = (1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0; 
	return QPointF(tx * zn, ty * zn); 
} 

void OpenStreetMaps::clearQueue()
{
	while (requests.size() > 1)
    {
        TileReq req = requests.takeLast();

        if (req.server != -1)
            serversInUse.removeOne(req.server);

        if (req.reply)
            req.reply->abort();
    }
}

TileSource OpenStreetMaps::getFileSource()
{
	return tileSource;
}

void OpenStreetMaps::setTileSource(TileSource t)
{
	tileSource = t;
}
