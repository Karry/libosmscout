#ifndef OSMSCOUT_CLIENT_QT_NAVIGATIONMODULE_H
#define OSMSCOUT_CLIENT_QT_NAVIGATIONMODULE_H

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

#include <osmscout/DBThread.h>
#include <osmscout/Settings.h>
#include <osmscout/Router.h>

#include <osmscout/navigation/Navigation.h>
#include <osmscout/navigation/Engine.h>
#include <osmscout/navigation/Agents.h>
#include <osmscout/navigation/DataAgent.h>
#include <osmscout/navigation/PositionAgent.h>
#include <osmscout/navigation/RouteStateAgent.h>
#include <osmscout/navigation/BearingAgent.h>
#include <osmscout/navigation/RouteInstructionAgent.h>
#include <osmscout/navigation/ArrivalEstimateAgent.h>
#include <osmscout/navigation/SpeedAgent.h>
#include <osmscout/navigation/VoiceInstructionAgent.h>

#include <osmscout/ClientQtImportExport.h>

#include <QObject>
#include <QTimer>
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace osmscout {

/**
 * \ingroup QtAPI
 */
class OSMSCOUT_CLIENT_QT_API NavigationModule: public QObject {
  Q_OBJECT

signals:
  void update(std::list<RouteStep> instructions);

  void updateNext(RouteStep nextRouteInstruction);

  void rerouteRequest(const osmscout::GeoCoord from,
                      const std::shared_ptr<osmscout::Bearing> initialBearing,
                      const osmscout::GeoCoord to);

  void positionEstimate(const osmscout::PositionAgent::PositionState state,
                        const osmscout::GeoCoord coord,
                        const std::shared_ptr<osmscout::Bearing> bearing);

  void arrivalEstimate(QDateTime arrivalEstimate, osmscout::Distance remainingDistance);

  void targetReached(const osmscout::Bearing targetBearing,
                     const osmscout::Distance targetDistance);

  void currentSpeed(double currentSpeedKmPH);
  void maxAllowedSpeed(double maxAllowedSpeedKmPh);

public slots:
  void setupRoute(QtRouteData route,
                  osmscout::Vehicle vehicle);

  /**
   * @param coord
   * @param horizontalAccuracyValid
   * @param horizontalAccuracy [meters]
   */
  void locationChanged(osmscout::GeoCoord coord,
                       bool horizontalAccuracyValid,
                       double horizontalAccuracy);

  void onTimeout();

  void onVoiceChanged(const QString);

  void playerStateChanged(QMediaPlayer::State state);

public:
  NavigationModule(QThread *thread,
                   SettingsRef settings,
                   DBThreadRef dbThread);

  bool loadRoutableObjects(const GeoBox &box,
                           const Vehicle &vehicle,
                           const std::map<std::string,DatabaseId> &databaseMapping,
                           RoutableObjectsRef &data);

  virtual ~NavigationModule();

private:
  void InitPlayer();
  void ProcessMessages(const std::list<osmscout::NavigationMessageRef>& messages);
  QString sampleFile(osmscout::VoiceInstructionMessage::VoiceSample sample) const;

private:
  QThread     *thread;
  SettingsRef settings;
  Units       units{Locale::ByEnvironment().GetDistanceUnits()}; // TODO: make possible to override
  DBThreadRef dbThread;
  QTimer      timer;
  std::shared_ptr<Bearing> lastBearing; // replace with optional with C++17

  // voice route instructions
  QString voiceDir;
  // player and playlist should be created in module thread, not in UI thread (constructor)
  // we setup QObject parents, objects are cleaned after Module destruction
  QMediaPlaylist *currentPlaylist{nullptr};
  QMediaPlayer *mediaPlayer{nullptr};
  std::vector<osmscout::VoiceInstructionMessage::VoiceSample> nextMessage;

  osmscout::RouteDescriptionRef routeDescription;

  using DataAgentInst=DataAgent<NavigationModule>;
  using DataAgentRef=std::shared_ptr<DataAgentInst>;

  osmscout::NavigationEngine engine{
      std::make_shared<DataAgent<NavigationModule>>(*this),
      std::make_shared<PositionAgent>(),
      std::make_shared<BearingAgent>(),
      std::make_shared<RouteInstructionAgent<RouteStep, RouteDescriptionBuilder>>(),
      std::make_shared<VoiceInstructionAgent>(units),
      std::make_shared<RouteStateAgent>(),
      std::make_shared<ArrivalEstimateAgent>(),
      std::make_shared<SpeedAgent>()
  };

};

}

#endif // OSMSCOUT_CLIENT_QT_NAVIGATIONMODULE_H
