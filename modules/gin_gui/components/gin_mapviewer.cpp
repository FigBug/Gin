/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/


MapViewer::MapViewer()
  : zoom (14),
    xoffset (0),
    yoffset (0),
    userAdjusted (false),
    posMarker (juce::Point<double>())
{
    mapsize = osm->getMapWidthPixels(zoom);

    osm->addListener (this);

    centerOn (-123.41480970382690, 48.490145885498649);
}

MapViewer::~MapViewer()
{
    osm->removeListener (this);

    clearDoubleBuffer();
}

void MapViewer::setZoom (int zoom_)
{
    zoom_ = juce::jlimit (0, 18, zoom_);
    if (zoom != zoom_)
    {
        zoom = zoom_;
        mapsize = osm->getMapWidthPixels(zoom);

        osm->clearQueue();

        mapUpdated();
    }
}

void MapViewer::tileFetched (int zoom_, int x, int y)
{
    if (zoom == zoom_)
    {
        mapUpdated();

        juce::MessageManager::callAsync ([x, y, self = juce::Component::SafePointer<MapViewer> (this)]
        {
            if (self != nullptr)
                self->repaint (x * 256 - self->xoffset, y * 256 - self->yoffset, 256, 256);
        });
    }
}

void MapViewer::resized()
{
    centerOn (centerPt.getX(), centerPt.getY());

    mapUpdated();
}

void MapViewer::paint (juce::Graphics& g)
{
    updateDoubleBuffer();

    if (doubleBuffer != nullptr)
        g.drawImageAt (*doubleBuffer, 0, 0);
}

void MapViewer::mouseDown (const juce::MouseEvent& e)
{
    lastPos = e.getPosition();
}

void MapViewer::mouseDrag (const juce::MouseEvent& e)
{
    userAdjusted = true;

    auto curPos = e.getPosition();

    xoffset = juce::jlimit (0, mapsize - getWidth(), xoffset - (curPos.getX() - lastPos.getX()));
    yoffset = juce::jlimit (0, mapsize - getHeight(), yoffset - (curPos.getY() - lastPos.getY()));

    centerPt = osm->displayToCoordinate (juce::Point<double>(xoffset + getWidth() / 2, yoffset + getHeight() / 2), zoom);

    lastPos = curPos;

    mapUpdated();
}

void MapViewer::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    userAdjusted = true;

    wheelDelta += wheel.deltaY;

    if (std::abs (wheelDelta) > 0.1 || ! wheel.isSmooth)
    {
        wheelDelta = 0;

        juce::Point<double> centerP = osm->displayToCoordinate (juce::Point<double> (xoffset + e.x, yoffset + e.y), zoom);

        if (wheel.deltaY < 0)
            setZoom (zoom - 1);
        else
            setZoom (zoom + 1);

        centerUnderPt (centerP, e.getPosition());
    }
}

void MapViewer::centerOn (double longCenter, double latCenter)
{
    juce::Point<double> p = osm->coordinateToDisplay(juce::Point<double>(longCenter, latCenter), zoom);

    xoffset = juce::jlimit (0, (mapsize - getWidth()),  int (p.getX()) - getWidth() / 2);
    yoffset = juce::jlimit (0, (mapsize - getHeight()), int (p.getY()) - getHeight() / 2);

    centerPt = juce::Point<double>(longCenter, latCenter);

    mapUpdated();
}

void MapViewer::centerUnderPt (juce::Point<double> world, juce::Point<int> view)
{
    juce::Point<double> p = osm->coordinateToDisplay (world, zoom);

    xoffset = juce::jlimit (0, juce::jmax (1, (mapsize - getWidth())),  int (p.getX()) - view.getX());
    yoffset = juce::jlimit (0, juce::jmax (1, (mapsize - getHeight())), int (p.getY()) - view.getY());

    centerPt = osm->displayToCoordinate (juce::Point<double>(xoffset + getWidth() / 2, yoffset + getHeight() / 2), zoom);

    repaint();
}

void MapViewer::updateDoubleBuffer()
{
    if (doubleBuffer)
        return;

    juce::Rectangle<int> rc (0, 0, getWidth(), getHeight());
    doubleBuffer = std::make_unique<juce::Image> (juce::Image::ARGB, rc.getWidth(), rc.getHeight(), true);

    juce::Graphics g (*doubleBuffer);

    // draw the map tiles
    for (int x = (rc.getX() + xoffset) / 256 * 256; x <= rc.getRight() + xoffset; x += 256)
    {
        for (int y = (rc.getY() + yoffset) / 256 * 256; y <= rc.getBottom() + yoffset; y += 256)
        {
            juce::Image tile = osm->fetchTile (zoom, x / 256, y / 256);
            g.drawImageAt (tile, x - xoffset, y - yoffset);
        }
    }
}

void MapViewer::clearDoubleBuffer()
{
    if (doubleBuffer)
        doubleBuffer = nullptr;
}

void MapViewer::mapUpdated()
{
    clearDoubleBuffer();

    juce::MessageManager::callAsync ([self = juce::Component::SafePointer<MapViewer> (this)]
    {
        if (self != nullptr)
            self->repaint();
    });
}
