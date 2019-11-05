#ifndef OSMSCOUT_NAVIGATION_BEARING_AGENT_H
#define OSMSCOUT_NAVIGATION_BEARING_AGENT_H

/*
 This source is part of the libosmscout library
 Copyright (C) 2018  Tim Teulings
 Copyright (C) 2019  Lukas Karas

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
#include <osmscout/navigation/PositionAgent.h>

namespace osmscout {

  struct OSMSCOUT_API BearingChangedMessage CLASS_FINAL : public NavigationMessage
  {
    const Bearing bearing;

    BearingChangedMessage(const Timestamp& timestamp,
                          const Bearing &bearing);
  };

  class OSMSCOUT_API BearingAgent CLASS_FINAL : public NavigationAgent
  {
  private:
    GeoCoord previousPoint;
    Bearing previousBearing;
    bool previousPointValid{false};
    Timestamp lastUpdate;

  public:
    explicit BearingAgent();
    std::list<NavigationMessageRef> Process(const NavigationMessageRef& message) override;
  };
}

#endif //OSMSCOUT_NAVIGATION_BEARING_AGENT_H
