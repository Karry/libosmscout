/*
  OSMScout - a Qt backend for libosmscout and libosmscout-map
  Copyright (C) 2016 Lukas Karas

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

#ifndef MAPMANAGER_H
#define	MAPMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QTimer>

#include <osmscout/private/ClientQtImportExport.h>

#include <osmscout/MapProvider.h>
#include <osmscout/AvailableMapsModel.h>

#include <qconfig.h>
#if QT_VERSION_MAJOR>=5 && QT_VERSION_MINOR>=4
#define HAS_QSTORAGE
#include <QStorageInfo>
#endif

/**
 * Simple utility class for download single file over http.
 * It don't support downloading restart when connection 
 * is interrupted.
 * \ingroup QtAPI
 */
class OSMSCOUT_CLIENT_QT_API FileDownloadJob: public QObject
{
  Q_OBJECT
  
private:
  QUrl           url;
  QFile          file;
  bool           downloading;
  bool           downloaded;
  QNetworkReply  *reply;
  qint64         bytesDownloaded;
  QString        fileName;

signals:
  void finished();
  void failed();
  void downloadProgress();

public slots:
  void onFinished(QNetworkReply* reply);
  void onReadyRead();
  void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

public:
  FileDownloadJob(QUrl url, QFileInfo file);
  virtual inline ~FileDownloadJob(){};
  
  void start(QNetworkAccessManager *webCtrl);

  inline bool isDownloading() const
  {
    return downloading;
  }

  inline bool isDownloaded() const
  {
    return downloaded;
  };

  inline qint64 getBytesDownloaded() const
  {
    return bytesDownloaded;
  };

  inline QString getFileName() const
  {
    return fileName;
  };
};

/**
 * Utility class for downloading map database described by AvailableMapsModelMap
 * over http. When connection is interrupted, downloading will be retried
 * after some backoff period.
 * \ingroup QtAPI
 */
class OSMSCOUT_CLIENT_QT_API MapDownloadJob: public QObject
{
  Q_OBJECT
  
  QList<FileDownloadJob*> jobs;
  QNetworkAccessManager   *webCtrl;
  
  AvailableMapsModelMap   map;
  QDir                    target;
  
  QTimer                  backoffTimer;
  int                     backoffInterval;
  bool                    done;
  bool                    started;

signals:
  void finished();
  void downloadProgress();

public slots:
  void onJobFailed();
  void onJobFinished();
  void downloadNextFile();

public:
  MapDownloadJob(QNetworkAccessManager *webCtrl, AvailableMapsModelMap map, QDir target);
  virtual ~MapDownloadJob();
  
  void start();

  inline QString getMapName()
  {
    return map.getName();
  }

  inline size_t expectedSize()
  {
    return map.getSize();
  }

  inline QDir getDestinationDirectory()
  {
    return target;
  }

  inline bool isDone()
  {
    return done;
  }

  inline bool isDownloading()
  {
    return started && !done;
  }
  double getProgress();
  QString getDownloadingFile();
};

/**
 * Manager of map databases. It provide database lookup 
 * (in databaseDirectories) and simple scheduler for downloading maps.
 * \ingroup QtAPI
 */
class OSMSCOUT_CLIENT_QT_API MapManager: public QObject
{
  Q_OBJECT
  
private:
  QStringList databaseLookupDirs;
  QList<QDir> databaseDirectories;
  QList<MapDownloadJob*> downloadJobs;
  QNetworkAccessManager webCtrl;

public slots:
  void lookupDatabases();
  void onJobFinished();
  
signals:
  void databaseListChanged(QList<QDir> databaseDirectories);
  void downloadJobsChanged();

public:
  MapManager(QStringList databaseLookupDirs);
  
  virtual ~MapManager();

  void downloadMap(AvailableMapsModelMap map, QDir dir);
  void downloadNext();
  
  inline QList<MapDownloadJob*> getDownloadJobs(){
    return downloadJobs;
  }
  
  inline QStringList getLookupDirectories()
  {
    return databaseLookupDirs;
  }
};

#endif	/* MAPMANAGER_H */

