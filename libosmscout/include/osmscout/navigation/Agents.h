#ifndef OSMSCOUT_NAVIGATION_AGENTS_H
#define OSMSCOUT_NAVIGATION_AGENTS_H

/*
 This source is part of the libosmscout library
 Copyright (C) 2018  Tim Teulings

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

#include <osmscout/LocationDescriptionService.h>
#include <osmscout/routing/AbstractRoutingService.h>

namespace osmscout {
  /**
   * Message to pass to the NavigationEngine each time gps state has changed.
   *
   * TODO: Pass information about the accuracy of the signal
   */
  struct OSMSCOUT_API GPSUpdateMessage CLASS_FINAL : public NavigationMessage
  {
    const GeoCoord currentPosition;
    const double   currentSpeed;
    const Distance horizontalAccuracy;

    /**
     * @param timestamp
     * @param currentPosition
     * @param currentSpeed if negative, speed is not known
     * @param horizontalAccuracy if negative, accuracy is not known
     */
    GPSUpdateMessage(const Timestamp& timestamp,
                     const GeoCoord& currentPosition,
                     double currentSpeed,
                     const Distance &horizontalAccuracy);
  };

  struct OSMSCOUT_API PositionChangedMessage CLASS_FINAL : public NavigationMessage
  {
    const GeoCoord currentPosition;
    const double   currentSpeed;

    PositionChangedMessage(const Timestamp& timestamp,
                           const GeoCoord& currentPosition,
                           double currentSpeed);
  };

  struct OSMSCOUT_API BearingChangedMessage CLASS_FINAL : public NavigationMessage
  {
    const bool hasBearing;
    const double bearing;

    explicit BearingChangedMessage(const Timestamp& timestamp);
    BearingChangedMessage(const Timestamp& timestamp,
                          double bearing);
  };

  struct OSMSCOUT_API StreetChangedMessage CLASS_FINAL : public NavigationMessage
  {
    const std::string name;

    StreetChangedMessage(const Timestamp& timestamp,
                         const std::string& name);
  };

  class OSMSCOUT_API CurrentStreetAgent CLASS_FINAL : public NavigationAgent
  {
  private:
    LocationDescriptionServiceRef locationDescriptionService;

    GeoCoord                      currentPosition;
    Timestamp                     lastCheckTime;
    std::string                   lastStreetName;

  private:
    std::string GetStreetName() const;

  public:
    explicit CurrentStreetAgent(const LocationDescriptionServiceRef& locationDescriptionService);
    std::list<NavigationMessageRef> Process(const NavigationMessageRef& message) override;
  };

  /**
   * Message to pass to the NavigationEngine each time the calculated route changes.
   * If parts f the message attributes are empty, these information are not available anymore (
   * possibly because a route was not calculated, thrown away, or is currently recalculated).
 */
  struct OSMSCOUT_API RouteUpdateMessage CLASS_FINAL : public NavigationMessage
  {
    const RouteDescriptionRef routeDescription;
    osmscout::Vehicle vehicle;

    RouteUpdateMessage(const Timestamp& timestamp,
                       const RouteDescriptionRef &routeDescription,
                       const osmscout::Vehicle &vehicle);
  };

  /**
   * Message created if the state of the navigation in relation to the route changed.
 */
  struct OSMSCOUT_API RouteStateChangedMessage CLASS_FINAL : public NavigationMessage
  {
    enum class State {
      noRoute,
      onRoute,
      offRoute
    };

    const State state;

    RouteStateChangedMessage(const Timestamp& timestamp,
                             State state);
  };

  class OSMSCOUT_API RouteStateAgent CLASS_FINAL : public NavigationAgent
  {
  private:
    RouteStateChangedMessage::State state;

  public:
    explicit RouteStateAgent();
    std::list<NavigationMessageRef> Process(const NavigationMessageRef& message) override;
  };

}

#endif
