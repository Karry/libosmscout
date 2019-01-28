#ifndef OSMSCOUT_NAVIGATION_ROUTE_POSITION_AGENT_H
#define OSMSCOUT_NAVIGATION_ROUTE_POSITION_AGENT_H

/*
 This source is part of the libosmscout library
 Copyright (C) 2018  Lukas Karas

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

#include <osmscout/navigation/Engine.h>
#include <osmscout/navigation/Agents.h>
#include <osmscout/navigation/DataAgent.h>

namespace osmscout {

  /**
   * This agent pin position of vehicle to most probable routable object.
   * TODO: compute probable position inside tunnel where is no GPS signal
   */
  class OSMSCOUT_API PositionAgent CLASS_FINAL : public NavigationAgent
  {
  public:
    enum GpsPositionState {
      Good,
      LowAccuracy,
      Outdated
    };

    struct GpsPosition {
      Timestamp lastUpdate;
      GeoCoord position;
      Distance horizontalAccuracy{Meters(2000)};

      GpsPositionState GetState(const Timestamp &now) const;
      std::string GetStateStr(const Timestamp &now) const;

      GeoBox GetGeoBox() const;

      void Update(const Timestamp &time,
                  const GeoCoord &position,
                  const Distance &horizontalAccuracy);
    };

    enum PositionState {
      NoGpsSignal,
      OnRoute,
      OffRoute,
      EstimateInTunnel
    };

    struct Position {
      PositionState state{PositionState::NoGpsSignal};
      GeoCoord coord;
      std::list<RouteDescription::Node>::const_iterator routeNode; // last passed node on the route

      // resolved object
      DatabaseId databaseId;
      TypeConfigRef typeConfig;
      WayRef way;
      AreaRef area;

      std::string StateStr() const;
    };

    /**
     * Message with estimated possition
     */
    struct OSMSCOUT_API PositionMessage CLASS_FINAL : public NavigationMessage
    {
      RouteDescriptionRef route;
      Position position;

      PositionMessage(const Timestamp& timestamp, const RouteDescriptionRef &route, const Position position);
    };

    using PositionMessageRef=std::shared_ptr<PositionMessage>;

  private:
    GpsPosition gps;
    Timestamp lastUpdate; // last update of agent state
    RoutableObjectsRef routableObjects; // routable objects around current position
    RouteDescriptionRef route; // current route description
    osmscout::Vehicle vehicle; // current vehicle
    Position position;
    Distance snapDistanceInMeters{Meters(20)}; // max distance from the route path to consider being on route

  public:
    PositionAgent();

    std::list<NavigationMessageRef> Process(const NavigationMessageRef& message) override;

  private:
    bool SearchClosestSegment(const GeoCoord& location,
                              const std::list<RouteDescription::Node>::const_iterator locationOnRoute,
                              GeoCoord &closestPosition,
                              std::list<RouteDescription::Node>::const_iterator& foundNode,
                              double& foundAbscissa,
                              double& minDistance);
  };

}
#endif 
