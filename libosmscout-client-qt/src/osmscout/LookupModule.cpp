/*
 OSMScout - a Qt backend for libosmscout and libosmscout-map
 Copyright (C) 2017 Lukas Karas

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

#include <osmscout/LookupModule.h>
#include <osmscout/OSMScoutQt.h>

LookupModule::LookupModule(QThread *thread,DBThreadRef dbThread):
  QObject(),
  thread(thread),
  dbThread(dbThread),
  loadJob(NULL)
{

  connect(dbThread.get(), SIGNAL(InitialisationFinished(const DatabaseLoadedResponse&)),
          this, SIGNAL(InitialisationFinished(const DatabaseLoadedResponse&)));
}

LookupModule::~LookupModule()
{
  if (thread!=QThread::currentThread()){
    qWarning() << "Destroy" << this << "from non incorrect thread;" << thread << "!=" << QThread::currentThread();
  }
  if (thread!=NULL){
    thread->quit();
  }
}

void LookupModule::requestObjectsOnView(const RenderMapRequest &view)
{
  double mapDpi=dbThread->GetMapDpi();

  // setup projection for data lookup
  osmscout::MercatorProjection lookupProjection;
  lookupProjection.Set(view.coord,  0, view.magnification, mapDpi, view.width*1.5, view.height*1.5);
  lookupProjection.SetLinearInterpolationUsage(view.magnification.GetLevel() >= 10);

  if (loadJob!=NULL){
    delete loadJob;
  }

  unsigned long maximumAreaLevel=4;
  if (view.magnification.GetLevel() >= 15) {
    maximumAreaLevel=6;
  }

  loadJob=new DBLoadJob(lookupProjection,maximumAreaLevel,/* lowZoomOptimization */ true);
  this->view=view;
  
  connect(loadJob, SIGNAL(databaseLoaded(QString,QList<osmscout::TileRef>)),
          this, SLOT(onDatabaseLoaded(QString,QList<osmscout::TileRef>)));
  connect(loadJob, SIGNAL(finished(QMap<QString,QMap<osmscout::TileId,osmscout::TileRef>>)),
          this, SLOT(onLoadJobFinished(QMap<QString,QMap<osmscout::TileId,osmscout::TileRef>>)));

  dbThread->RunJob(loadJob);
}

void LookupModule::onDatabaseLoaded(QString dbPath,QList<osmscout::TileRef> tiles)
{
  osmscout::MapData data;
  loadJob->AddTileDataToMapData(dbPath,tiles,data);
  emit viewObjectsLoaded(view, data);
}

void LookupModule::onLoadJobFinished(QMap<QString,QMap<osmscout::TileId,osmscout::TileRef>> /*tiles*/)
{
  emit viewObjectsLoaded(view, osmscout::MapData());
}

void LookupModule::requestLocationDescription(const osmscout::GeoCoord location)
{
  QMutexLocker locker(&mutex);
  OSMScoutQt::GetInstance().GetDBThread()->RunSynchronousJob(
    [this,location](const std::list<DBInstanceRef> &databases){
      int count = 0;
      for (auto db:databases){
        osmscout::LocationDescription description;
        osmscout::GeoBox dbBox;
        if (!db->database->GetBoundingBox(dbBox)){
          continue;
        }
        if (!dbBox.Includes(location)){
          continue;
        }

        std::map<osmscout::FileOffset,osmscout::AdminRegionRef> regionMap;
        if (!db->locationService->DescribeLocationByAddress(location, description)) {
          osmscout::log.Error() << "Error during generation of location description";
          continue;
        }

        if (description.GetAtAddressDescription()){
          count++;

          auto place = description.GetAtAddressDescription()->GetPlace();
          emit locationDescription(location, db->path, description,
                                   BuildAdminRegionList(db->locationService, place.GetAdminRegion(), regionMap));
        }

        if (!db->locationService->DescribeLocationByPOI(location, description)) {
          osmscout::log.Error() << "Error during generation of location description";
          continue;
        }

        if (description.GetAtPOIDescription()){
          count++;

          auto place = description.GetAtPOIDescription()->GetPlace();
          emit locationDescription(location, db->path, description,
                                   BuildAdminRegionList(db->locationService, place.GetAdminRegion(), regionMap));
        }
      }

      emit locationDescriptionFinished(location);
    }
  );
}

QStringList LookupModule::BuildAdminRegionList(const osmscout::AdminRegionRef& adminRegion,
                                               std::map<osmscout::FileOffset,osmscout::AdminRegionRef> regionMap)
{
  return BuildAdminRegionList(osmscout::LocationServiceRef(), adminRegion, regionMap);
}

QStringList LookupModule::BuildAdminRegionList(const osmscout::LocationServiceRef& locationService,
                                               const osmscout::AdminRegionRef& adminRegion,
                                           std::map<osmscout::FileOffset,osmscout::AdminRegionRef> regionMap)
{
  if (!adminRegion){
    return QStringList();
  }

  QStringList list;
  if (locationService){
    locationService->ResolveAdminRegionHierachie(adminRegion, regionMap);
  }
  QString name = QString::fromStdString(adminRegion->name);
  list << name;
  QString last = name;
  osmscout::FileOffset parentOffset = adminRegion->parentRegionOffset;
  while (parentOffset != 0){
    const auto &it = regionMap.find(parentOffset);
    if (it==regionMap.end())
      break;
    const osmscout::AdminRegionRef region=it->second;
    name = QString::fromStdString(region->name);
    if (last != name){ // skip duplicates in admin region names
      list << name;
    }
    last = name;
    parentOffset = region->parentRegionOffset;
  }
  return list;
}
