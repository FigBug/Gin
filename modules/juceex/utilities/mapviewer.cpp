/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/


MapViewer::MapViewer()
  :	zoom(14),
	xoffset(0),
	yoffset(0),
	selectedTrack(0),
	userAdjusted(false),
	posMarker(Point<double>()),
    doubleBuffer(NULL)
{
	osm = OpenStreetMaps::retain();
    osm->setTileSource((TileSource)QSettings().value("tileSource", OpenCycleMapLandscape).toInt());

	mapsize = osm->getMapWidthPixels(zoom);

	centerOn (-123.41480970382690, 48.490145885498649);

	autoZoomAndCenter();
}

MapViewer::~MapViewer()
{
    clearDoubleBuffer();
}

void MapViewer::setZoom (int zoom_)
{
	zoom_ = jlimit (0, 18, zoom_);
	if (zoom != zoom_)
	{
		zoom = zoom_;
		mapsize = osm->getMapWidthPixels(zoom);		

		osm->clearQueue();

        mapUpdated();
	}
}

void MapViewer::tileFetched(int zoom_, int x, int y)
{
	if (zoom == zoom_)
    {
        mapUpdated();
		update(x * 256 - xoffset, y * 256 - yoffset, 256, 256);
    }
}

void MapViewer::resizeEvent(QResizeEvent* event)
{
    snapshot->move(width() - 18, 2);
    editStartFinish->move(width() - 18, snapshot->geometry().bottom());

	if (!userAdjusted && tracks.size() > 0)
		autoZoomAndCenter();
	else
		centerOn(centerPt.x(), centerPt.y());

    mapUpdated();
}

void MapViewer::paint (Graphics& g) 
{
	QPainter painter(this);
	painter.setClipRect(event->rect());

    updateDoubleBuffer();

    if (doubleBuffer)
        painter.drawImage(0, 0, *doubleBuffer);

    // draw the arrows
	for (int i = 0; i < tracks.size(); i++)
	{
		TrackInfo& ti = tracks[i];
		ti.color.setAlpha(190);
		DataSet* lat = ti.session->getDataSet(ti.latitude);
		DataSet* lon = ti.session->getDataSet(ti.longitude);
        if (lon && lat && lon->size() >= 2 && lat->size() >= 2)
		{            
			// create the marker            
            double t = ti.playhead->getPosition(TIME);
            ti.lastDrawnPosition = Point<double>(lon->yAtX(t), lat->yAtX(t));

			painter.setPen(ti.color);
			Point<double> now = osm->coordinateToDisplay(Point<double>(lon->yAtX(t), lat->yAtX(t)), zoom);
			Point<double> fut = osm->coordinateToDisplay(Point<double>(lon->yAtX(t + 5), lat->yAtX(t + 5)), zoom);

			QPolygon arrow(QVector<QPoint>() << QPoint(0, 8) << QPoint(-5, -8) << QPoint(0, -6) << QPoint(5, -8)); 
			QMatrix matrix;
			matrix.translate(now.x() - xoffset, now.y() - yoffset);

			float dx = fut.x() - now.x();
			float dy = fut.y() - now.y();
			float deg = qAtan2(dy, dx) * (180.0 / 3.14159);
			if(deg < 0)
				deg += 360; 
			matrix.rotate(deg - 90);
			matrix.scale(1.25, 1.25);

		    painter.setRenderHint(QPainter::Antialiasing);
		    painter.setPen(QPen(QBrush(Qt::SolidPattern), 2));
            painter.setBrush(QBrush(ti.color));

            painter.drawPolygon(matrix.map(arrow));

			posMarker = now;
		}
	}
}

void MapViewer::mousePressEvent(QMouseEvent* event)
{
	if (tracks.size() == 1 && (event->modifiers() & Qt::SHIFT))
	{
		if (event->modifiers() & Qt::CTRL)
			drawingFinish = true;
		else
			drawingStart  = true;

		startPt  = event->pos();
		finishPt = event->pos();

		QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
	}
	else
	{
		lastPos = event->globalPos();
	}
}

void MapViewer::mouseReleaseEvent(QMouseEvent* event)
{
	if (drawingStart)
	{
		startPt.rx()  += xoffset;
		startPt.ry()  += yoffset;
		finishPt.rx() += xoffset;
		finishPt.ry() += yoffset;

		tracks.first().session->setStartLine(QLineF(osm->displayToCoordinate(startPt, zoom), 
							                        osm->displayToCoordinate(finishPt, zoom)));

		drawingStart = false;

		QApplication::restoreOverrideCursor();
	}

	if (drawingFinish)
	{
		startPt.rx()  += xoffset;
		startPt.ry()  += yoffset;
		finishPt.rx() += xoffset;
		finishPt.ry() += yoffset;

		tracks.first().session->setFinishLine(QLineF(osm->displayToCoordinate(startPt, zoom), 
						                             osm->displayToCoordinate(finishPt, zoom)));

		drawingFinish = false;	

		QApplication::restoreOverrideCursor();
	}
}

void MapViewer::mouseMoveEvent(QMouseEvent* event)
{
	if (drawingStart || drawingFinish)
	{
		finishPt = event->pos();
		update();
	}
	else
	{
		userAdjusted = true;

		QPoint curPos = event->globalPos();

		xoffset = bounded(0.0, double(mapsize - width()), xoffset - (curPos.x() - lastPos.x()));
		yoffset = bounded(0.0, double(mapsize - height()), yoffset - (curPos.y() - lastPos.y()));

		centerPt = osm->displayToCoordinate(QPoint(xoffset + width() / 2, yoffset + height() / 2), zoom);

		lastPos = curPos;

		mapUpdated();
	}
}
 
void MapViewer::wheelEvent(QWheelEvent* event)
{
	userAdjusted = true;

	Point<double> centerP = osm->displayToCoordinate(QPoint(xoffset + event->x(), yoffset + event->y()), zoom);

	if (event->delta() < 0)
		setZoom(zoom - 1);
	else
		setZoom(zoom + 1);

	centerUnderPt(centerP, event->pos());
}

void MapViewer::centerOn(double longCenter, double latCenter)
{
	Point<double> p = osm->coordinateToDisplay(Point<double>(longCenter, latCenter), zoom);

	xoffset = bounded(0.0, double(mapsize - width()),  p.x() - width() / 2);
	yoffset = bounded(0.0, double(mapsize - height()), p.y() - height() / 2);

	centerPt = Point<double>(longCenter, latCenter);

	mapUpdated();
}

void MapViewer::centerUnderPt(Point<double> world, QPoint view)
{
	Point<double> p = osm->coordinateToDisplay(world, zoom);

	xoffset = bounded(0.0, double(mapsize - width()),  p.x() - view.x());
	yoffset = bounded(0.0, double(mapsize - height()), p.y() - view.y());

	centerPt = osm->displayToCoordinate(QPoint(xoffset + width() / 2, yoffset + height() / 2), zoom);

	update();
}

void MapViewer::autoZoomAndCenter()
{
	double minLat = DBL_MAX;
	double maxLat = -DBL_MAX;

	double minLong = DBL_MAX;
	double maxLong = -DBL_MAX;

	bool pointFound = false;
	for (int j = 0; j < tracks.size(); j++)
	{
		TrackInfo ti = tracks[j];
		DataSet* latitude = ti.session->getDataSet(ti.latitude);
		DataSet* longitude = ti.session->getDataSet(ti.longitude);
		if (latitude && longitude)
		{
			for (int i = 0; i < min(longitude->size(), latitude->size()); i++)
			{
				double lng = (*longitude)[i].y();
				double lat = (*latitude)[i].y();

				if (lng > maxLong) maxLong = lng;
				if (lng < minLong) minLong = lng;

				if (lat > maxLat) maxLat = lat;
				if (lat < minLat) minLat = lat;		

				pointFound = true;
			}
		}
	}

	if (pointFound)
	{
		for (int zm = 18; zm >= 0; zm--)
		{
			Point<double> p1 = osm->coordinateToDisplay(Point<double>(minLong, maxLat), zm);
			Point<double> p2 = osm->coordinateToDisplay(Point<double>(maxLong, minLat), zm);

			QRectF rc(p1, p2);
			if (rc.width() < width() && rc.height() < height())
			{
				setZoom(zm);
				break;
			}
		}

		centerOn((maxLong + minLong) / 2, (maxLat + minLat) / 2);
	}
}

void MapViewer::dataSetDeleted(DataSet* dataset)
{
	for (int i = 0; i < tracks.size(); i++)
	{
		TrackInfo ti = tracks[i];
		DataSet* lat = ti.session->getDataSet(ti.latitude);
		DataSet* lon = ti.session->getDataSet(ti.longitude);
		if (lat == dataset)
			lat = NULL;
		if (lon == dataset)
			lon = NULL;
	}
}

void MapViewer::showEditStartFinish()
{
	EditStartFinishLines esfl(this, tracks.first().session);
	esfl.exec();
}

void MapViewer::setSelectedTrack(int selTrack)
{
	selectedTrack = selTrack;
}

void MapViewer::updateMap()
{
	TrackInfo ti = tracks[selectedTrack];
	DataSet* lat = ti.session->getDataSet(ti.latitude);
	DataSet* lon = ti.session->getDataSet(ti.longitude);

    bool anyMarkersMoved = false;

    for (int i = 0; i < tracks.size(); i++)
    {
        TrackInfo& ti = tracks[i];

        DataSet* lat = ti.session->getDataSet(ti.latitude);
		DataSet* lon = ti.session->getDataSet(ti.longitude);
		if (lon && lat)
        {
            double t = ti.playhead->getPosition(TIME);

            Point<double> pos = Point<double>(lon->yAtX(t), lat->yAtX(t));

            if (ti.lastDrawnPosition != pos)
                anyMarkersMoved = true;
        }
    }

	if (lat && lon)
	{
		Point<double> now = osm->coordinateToDisplay(Point<double>(lon->yAtX(ti.playhead->getPosition(TIME)), lat->yAtX(ti.playhead->getPosition(TIME))), zoom);
		int x = now.x() - xoffset;
		int y = now.y() - yoffset;
		if(x > (width() - 15)|| y > (height() - 15) || y < 15 || x < 15)
		{
			Point<double> centerPoint = osm->displayToCoordinate(now, zoom);
			centerOn(centerPoint.x(), centerPoint.y());
		}
	}

    if (anyMarkersMoved)
	    update();
}

void MapViewer::updateDoubleBuffer()
{
    if (doubleBuffer)
        return;

    QRect rc(0, 0, width(), height());
    doubleBuffer = new QImage(rc.size(), QImage::Format_ARGB32);

	QPainter painter(doubleBuffer);    

    // draw the map tiles
	for (int x = (rc.left() + xoffset) / 256 * 256; x <= rc.right() + xoffset; x += 256)
	{
		for (int y = (rc.top() + yoffset) / 256 * 256; y <= rc.bottom() + yoffset; y += 256)
		{
			QImage tile = osm->fetchTile(zoom, x / 256, y / 256);
			painter.drawImage(x - xoffset, y - yoffset, tile);
		}
	}

	// draw the track
    QList<QPair<QPolygon, QColor> > arrows;
	for (int i = 0; i < tracks.size(); i++)
	{
		TrackInfo& ti = tracks[i];
		ti.color.setAlpha(190);
		DataSet* lat = ti.session->getDataSet(ti.latitude);
		DataSet* lon = ti.session->getDataSet(ti.longitude);
        if (lon && lat && lon->size() >= 2 && lat->size() >= 2)
		{            
			// draw the track
            double in  = ti.playhead->getInMarker(TIME);
            double out = ti.playhead->getOutMarker(TIME);

            if (in > 0)
                drawTrack(painter, lat, lon, ti.color.lighter(155), 0, in);

            drawTrack(painter, lat, lon, ti.color, in, out);

            if (out < ti.session->getSessionLength(TIME))
                drawTrack(painter, lat, lon, ti.color.lighter(155), out, ti.session->getSessionLength(TIME));

            // draw markers
            for (int j = 0; j < ti.session->getNumMarkers(); j++)
            {
                Marker* m = ti.session->getMarker(j);
                double pos = m->getPosition(TIME);
                if (pos < in || pos > out) continue;

                Point<double> p0 = osm->coordinateToDisplay(Point<double>(lon->interpolatedY(pos), lat->interpolatedY(pos)), zoom);
                Point<double> p1 = osm->coordinateToDisplay(Point<double>(lon->interpolatedY(pos + 3), lat->interpolatedY(pos + 3)), zoom);

                QLineF l(p0, p1);
                l.setLength(4.5);
                l = l.normalVector();

                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setPen(QPen(ti.color, 2));
                painter.drawLine(l.p1() - Point<double>(xoffset, yoffset), l.p2() - Point<double>(xoffset, yoffset));
                l.setLength(-3.5);
                painter.drawLine(l.p1() - Point<double>(xoffset, yoffset), l.p2() - Point<double>(xoffset, yoffset));
                painter.setRenderHint(QPainter::Antialiasing, false);
            }

			// draw start line
			QLineF start = ti.session->getStartLine();
			painter.setPen(Qt::darkGreen);

			if (drawingStart)
			{
				painter.drawLine(startPt, finishPt);
			}
			else if (!start.isNull())
			{
				Point<double> p1 = osm->coordinateToDisplay(start.p1(), zoom);
				Point<double> p2 = osm->coordinateToDisplay(start.p2(), zoom);
				painter.drawLine(p1.x() - xoffset, p1.y() - yoffset, p2.x() - xoffset, p2.y() - yoffset);
			}

			// draw finish line
			QLineF finish = ti.session->getFinishLine();
			painter.setPen(Qt::darkRed);

			if (drawingFinish)
			{
				painter.drawLine(startPt, finishPt);
			}
			else if (!finish.isNull())
			{
				Point<double> p1 = osm->coordinateToDisplay(finish.p1(), zoom);
				Point<double> p2 = osm->coordinateToDisplay(finish.p2(), zoom);
				painter.drawLine(p1.x() - xoffset, p1.y() - yoffset, p2.x() - xoffset, p2.y() - yoffset);
            }
		}
	}
}

void MapViewer::clearDoubleBuffer()
{
    if (doubleBuffer)
    {
        delete doubleBuffer;
        doubleBuffer = NULL;
    }
}

void MapViewer::mapUpdated()
{
    clearDoubleBuffer();
    update();
}

void MapViewer::saveSnapshot()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Image",  QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation), "PNG Image (*.png)");

    if (!filename.isEmpty())
    {
        QPixmap px(rect().size());
        render(&px);
    
        px.save(filename, "PNG");
    }
}
