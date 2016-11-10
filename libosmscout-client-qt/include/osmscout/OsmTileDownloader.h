/*
 OSMScout - a Qt backend for libosmscout and libosmscout-map
 Copyright (C) 2016  Lukas Karas

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifndef OSMTILEDOWNLOADER_H
#define	OSMTILEDOWNLOADER_H

 
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImage>
#include <QNetworkDiskCache>

#include "TileCache.h"
#include "OnlineTileProvider.h"

// these variables should be defined by build system
#ifndef OSMSCOUT_VERSION_STRING
#define OSMSCOUT_VERSION_STRING "v?"
#endif

#ifndef OSMSCOUT_USER_AGENT
#define OSMSCOUT_USER_AGENT "OSMScout demo app %1"
#endif

class OsmTileDownloader : public QObject
{
 Q_OBJECT
 
public:
  OsmTileDownloader(QString diskCacheDir);
  virtual ~OsmTileDownloader();
  
public slots:
  void download(uint32_t zoomLevel, uint32_t x, uint32_t y);
  void onlineTileProviderChanged();
  
signals:
  void downloaded(uint32_t zoomLevel, uint32_t x, uint32_t y, QImage image, QByteArray downloadedData);
  void failed(uint32_t zoomLevel, uint32_t x, uint32_t y, bool zoomLevelOutOfRange);

private slots:
  void fileDownloaded(QNetworkReply* pReply);
 
private:
  int                       serverNumber;
  QNetworkAccessManager     webCtrl;
  QHash<QUrl,TileCacheKey>  requests;
  QNetworkDiskCache         diskCache;
  OnlineTileProvider        tileProvider;

};

#endif	/* OSMTILEDOWNLOADER_H */
