/*
  This source is part of the libosmscout library
  Copyright (C) 2009  Tim Teulings

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

#include <osmscout/routing/Route.h>

#include <sstream>

#include <osmscout/system/Assert.h>
#include <osmscout/system/Math.h>
#include "../../../../Demos/src/Navigation.h"

namespace osmscout {

  /** Constant for a description of the start node (StartDescription) */
  const char* const RouteDescription::NODE_START_DESC        = "NodeStart";
  /** Constant for a description of the target node (TargetDescription) */
  const char* const RouteDescription::NODE_TARGET_DESC       = "NodeTarget";
  /** Constant for a description of name of the way (NameDescription) */
  const char* const RouteDescription::WAY_NAME_DESC          = "WayName";
  /** Constant for a description of a change of way name (NameChangedDescription) */
  const char* const RouteDescription::WAY_NAME_CHANGED_DESC  = "WayChangedName";
  /** Constant for a description of list of way name crossing a node (CrossingWaysDescription) */
  const char* const RouteDescription::CROSSING_WAYS_DESC     = "CrossingWays";
  /** Constant for a description of a turn (TurnDescription) */
  const char* const RouteDescription::DIRECTION_DESC         = "Direction";
  /** Constant for doing description of an explicit turn (TurnDescription) */
  const char* const RouteDescription::TURN_DESC              = "Turn";
  /** Constant for a description of entering a roundabout (RoundaboutEnterDescription) */
  const char* const RouteDescription::ROUNDABOUT_ENTER_DESC  = "RountaboutEnter";
  /** Constant for a description of entering a roundabout (RoundaboutLeaveDescription) */
  const char* const RouteDescription::ROUNDABOUT_LEAVE_DESC  = "RountaboutLeave";
  /** Constant for a description of entering a motorway (MotorwayEnterDescription) */
  const char* const RouteDescription::MOTORWAY_ENTER_DESC    = "MotorwayEnter";
  /** Constant for a description of changing a motorway (MotorwayChangeDescription) */
  const char* const RouteDescription::MOTORWAY_CHANGE_DESC   = "MotorwayChange";
  /** Constant for a description of leaving a motorway (MotorwayLeaveDescription) */
  const char* const RouteDescription::MOTORWAY_LEAVE_DESC    = "MotorwayLeave";
  /** Constant for a description of motorway junction (MotorwayJunctionDescription) */
  const char* const RouteDescription::MOTORWAY_JUNCTION_DESC = "MotorwayJunction";
  /** Constant for a description of a destination to choose at a junction */
  const char* const RouteDescription::CROSSING_DESTINATION_DESC = "CrossingDestination";
  /** Constant for a description of the maximum speed for the given way */
  const char* const RouteDescription::WAY_MAXSPEED_DESC      = "MaxSpeed";
  /** Constant for a description of type name of the way (TypeNameDescription) */
  const char* const RouteDescription::WAY_TYPE_NAME_DESC     = "TypeName";
  /** Constant for a description of type name of the way (TypeNameDescription) */
  const char* const RouteDescription::POI_AT_ROUTE_DESC      = "POIAtRoute";

  RouteDescription::Description::~Description()
  {
    // no code
  }

  RouteDescription::StartDescription::StartDescription(const std::string& description)
  : description(description)
  {
    // no code
  }

  std::string RouteDescription::StartDescription::GetDebugString() const
  {
    return "Start: '"+description+"'";
  }

  std::string RouteDescription::StartDescription::GetDescription() const
  {
    return description;
  }

  RouteDescription::TargetDescription::TargetDescription(const std::string& description)
  : description(description)
  {
    // no code
  }

  std::string RouteDescription::TargetDescription::GetDebugString() const
  {
    return "Target: '"+description+"'";
  }

  std::string RouteDescription::TargetDescription::GetDescription() const
  {
    return description;
  }

  RouteDescription::NameDescription::NameDescription(const std::string& name)
  : name(name)
  {
    // no code
  }

  RouteDescription::NameDescription::NameDescription(const std::string& name,
                                                     const std::string& ref)
  : name(name),
    ref(ref)
  {
    // no code
  }

  std::string RouteDescription::NameDescription::GetDebugString() const
  {
    return "Name: '"+GetDescription()+"'";
  }

  bool RouteDescription::NameDescription::HasName() const
  {
    return !name.empty() || !ref.empty();
  }

  std::string RouteDescription::NameDescription::GetName() const
  {
    return name;
  }

  std::string RouteDescription::NameDescription::GetRef() const
  {
    return ref;
  }

  std::string RouteDescription::NameDescription::GetDescription() const
  {
    std::ostringstream stream;

    if (name.empty() &&
        ref.empty()) {
      return "unnamed road";
    }

    if (!name.empty()) {
      stream << name;
    }

    if (!name.empty() &&
        !ref.empty()) {
      stream << " (";
    }

    if (!ref.empty()) {
      stream << ref;
    }

    if (!name.empty() &&
        !ref.empty()) {
      stream << ")";
    }

    return stream.str();
  }

  RouteDescription::NameChangedDescription::NameChangedDescription(const NameDescriptionRef& originDescription,
                                                                   const NameDescriptionRef& targetDescription)
  : originDescription(originDescription),
    targetDescription(targetDescription)
  {
    // no code
  }

  std::string RouteDescription::NameChangedDescription::GetDebugString() const
  {
    std::string result="Name Change: ";

    if (originDescription) {
      result+="'"+originDescription->GetDescription()+"'";
    }

    result+=" => ";

    if (targetDescription) {
      result+="'"+targetDescription->GetDescription()+"'";
    }

    return result;
  }

  RouteDescription::CrossingWaysDescription::CrossingWaysDescription(size_t exitCount,
                                                                     const NameDescriptionRef& originDescription,
                                                                     const NameDescriptionRef& targetDescription)
  : exitCount(exitCount),
    originDescription(originDescription),
    targetDescription(targetDescription)
  {
    // no code
  }

  void RouteDescription::CrossingWaysDescription::AddDescription(const NameDescriptionRef& description)
  {
    descriptions.push_back(description);
  }

  std::string RouteDescription::CrossingWaysDescription::GetDebugString() const
  {
    std::string result;

    result+="Crossing";

    if (originDescription) {
      if (!result.empty()) {
        result+=" ";
      }

      result+="from '"+originDescription->GetDescription()+"'";
    }

    if (targetDescription) {
      if (!result.empty()) {
        result+=" ";
      }

      result+="to '"+targetDescription->GetDescription()+"'";
    }

    if (!descriptions.empty()) {
      if (!result.empty()) {
        result+=" ";
      }

      result+="with";

      for (std::list<NameDescriptionRef>::const_iterator desc=descriptions.begin();
          desc!=descriptions.end();
          ++desc) {
        result+=" '"+(*desc)->GetDescription()+"'";
      }
    }

    if (!descriptions.empty()) {
      if (!result.empty()) {
        result+=" ";
      }
      result+=std::to_string(exitCount)+ " exits";
    }

    return "Crossing: "+result;
  }

  RouteDescription::DirectionDescription::Move RouteDescription::DirectionDescription::ConvertAngleToMove(double angle) const
  {
    if (fabs(angle)<=10.0) {
      return straightOn;
    }
    else if (fabs(angle)<=45.0) {
      return angle<0 ? slightlyLeft : slightlyRight;
    }
    else if (fabs(angle)<=120.0) {
      return angle<0 ? left : right;
    }
    else {
      return angle<0 ? sharpLeft : sharpRight;
    }
  }

  std::string RouteDescription::DirectionDescription::ConvertMoveToString(Move move) const
  {
    switch (move) {
    case osmscout::RouteDescription::DirectionDescription::sharpLeft:
      return "Turn sharp left";
    case osmscout::RouteDescription::DirectionDescription::left:
      return "Turn left";
    case osmscout::RouteDescription::DirectionDescription::slightlyLeft:
      return "Turn slightly left";
    case osmscout::RouteDescription::DirectionDescription::straightOn:
      return "Straight on";
    case osmscout::RouteDescription::DirectionDescription::slightlyRight:
      return "Turn slightly right";
    case osmscout::RouteDescription::DirectionDescription::right:
      return "Turn right";
    case osmscout::RouteDescription::DirectionDescription::sharpRight:
      return "Turn sharp right";
    }

    assert(false);

    return "???";
  }

  RouteDescription::DirectionDescription::DirectionDescription(double turnAngle,
                                                               double curveAngle)
  : turnAngle(turnAngle),
    curveAngle(curveAngle)
  {
    turn=ConvertAngleToMove(turnAngle);
    curve=ConvertAngleToMove(curveAngle);
  }

  std::string RouteDescription::DirectionDescription::GetDebugString() const
  {
    std::ostringstream stream;

    stream << "Direction: ";
    stream << "Turn: " << ConvertMoveToString(turn) << ", " << turnAngle << " degrees ";
    stream << "Curve: " << ConvertMoveToString(curve) << ", " << curveAngle << " degrees";

    return stream.str();
  }

  RouteDescription::TurnDescription::TurnDescription()
  {
    // no code
  }

  std::string RouteDescription::TurnDescription::GetDebugString() const
  {
    return "Turn";
  }

  RouteDescription::RoundaboutEnterDescription::RoundaboutEnterDescription()
  {
    // no code
  }

  std::string RouteDescription::RoundaboutEnterDescription::GetDebugString() const
  {
    return "Enter roundabout";
  }

  RouteDescription::RoundaboutLeaveDescription::RoundaboutLeaveDescription(size_t exitCount)
  : exitCount(exitCount)
  {
    // no code
  }

  std::string RouteDescription::RoundaboutLeaveDescription::GetDebugString() const
  {
    return "Leave roundabout";
  }

  RouteDescription::MotorwayEnterDescription::MotorwayEnterDescription(const NameDescriptionRef& targetDescription)
  : toDescription(targetDescription)
  {
    // no code
  }

  std::string RouteDescription::MotorwayEnterDescription::GetDebugString() const
  {
    std::string result="Enter motorway";

    if (toDescription &&
        toDescription->HasName()) {
      result+=" '"+toDescription->GetDescription()+"'";
    }

    return result;
  }

  RouteDescription::MotorwayChangeDescription::MotorwayChangeDescription(const NameDescriptionRef& fromDescription,
                                                                         const NameDescriptionRef& toDescription)
  : fromDescription(fromDescription),
    toDescription(toDescription)
  {
    // no code
  }

  std::string RouteDescription::MotorwayChangeDescription::GetDebugString() const
  {
    return "Change motorway";
  }

  RouteDescription::MotorwayLeaveDescription::MotorwayLeaveDescription(const NameDescriptionRef& fromDescription)
  : fromDescription(fromDescription)
  {
    // no code
  }

  std::string RouteDescription::MotorwayLeaveDescription::GetDebugString() const
  {
    return "Leave motorway";
  }

  RouteDescription::MotorwayJunctionDescription::MotorwayJunctionDescription(const NameDescriptionRef& junctionDescription)
  : junctionDescription(junctionDescription)
  {
    // no code
  }

  std::string RouteDescription::MotorwayJunctionDescription::GetDebugString() const
  {
    return "motorway junction";
  }

  RouteDescription::DestinationDescription::DestinationDescription(const std::string& description)
    : description(description)
  {
    // no code
  }

  std::string RouteDescription::DestinationDescription::GetDebugString() const
  {
    return "Start: '"+description+"'";
  }

  std::string RouteDescription::DestinationDescription::GetDescription() const
  {
    return description;
  }

  RouteDescription::MaxSpeedDescription::MaxSpeedDescription(uint8_t speed)
  : maxSpeed(speed)
  {

  }

  std::string RouteDescription::MaxSpeedDescription::GetDebugString() const
  {
    return std::string("Max. speed ")+std::to_string(maxSpeed)+"km/h";
  }

  RouteDescription::Node::Node(DatabaseId database,
                               size_t currentNodeIndex,
                               const std::vector<ObjectFileRef>& objects,
                               const ObjectFileRef& pathObject,
                               size_t targetNodeIndex)
  : database(database),
    currentNodeIndex(currentNodeIndex),
    objects(objects),
    pathObject(pathObject),
    targetNodeIndex(targetNodeIndex),
    time(0.0),
    location(GeoCoord(NAN, NAN))
  {
    // no code
  }

  RouteDescription::TypeNameDescription::TypeNameDescription(const std::string& name)
    : name(name)
  {
    // no code
  }

  std::string RouteDescription::TypeNameDescription::GetDebugString() const
  {
    return "Name: '"+GetDescription()+"'";
  }

  bool RouteDescription::TypeNameDescription::HasName() const
  {
    return !name.empty();
  }

  std::string RouteDescription::TypeNameDescription::GetName() const
  {
    return name;
  }

  std::string RouteDescription::TypeNameDescription::GetDescription() const
  {
    std::ostringstream stream;

    stream << name;

    return stream.str();
  }

  RouteDescription::POIAtRouteDescription::POIAtRouteDescription(DatabaseId databaseId,
                                                                 const ObjectFileRef& object,
                                                                 const NameDescriptionRef& name,
                                                                 const Distance& distance)
  : databaseId(databaseId),
    object(object),
    name(name),
    distance(distance)
  {
  }

  std::string RouteDescription::POIAtRouteDescription::GetDebugString() const
  {
    return object.GetName() +" "+std::to_string(distance.AsMeter());
  }

  bool RouteDescription::Node::HasDescription(const char* name) const
  {
    std::unordered_map<std::string,DescriptionRef>::const_iterator entry;

    entry=descriptionMap.find(name);

    return entry!=descriptionMap.end() && entry->second;
  }

  RouteDescription::DescriptionRef RouteDescription::Node::GetDescription(const char* name) const
  {
    std::unordered_map<std::string,DescriptionRef>::const_iterator entry;

    entry=descriptionMap.find(name);

    if (entry!=descriptionMap.end()) {
      return entry->second;
    }
    else {
      return nullptr;
    }
  }

  void RouteDescription::Node::SetDistance(Distance distance)
  {
    this->distance=distance;
  }

  void RouteDescription::Node::SetLocation(const GeoCoord &coord)
  {
    this->location=coord;
  }


  void RouteDescription::Node::SetTime(double time)
  {
    this->time=time;
  }

  void RouteDescription::Node::AddDescription(const char* name,
                                              const DescriptionRef& description)
  {
    descriptions.push_back(description);
    descriptionMap[name]=description;
  }

  RouteDescription::RouteDescription()
  {
    // no code
  }

  RouteDescription::~RouteDescription()
  {
    // no code
  }

  void RouteDescription::Clear()
  {
    nodes.clear();
  }

  void RouteDescription::AddNode(DatabaseId database,
                                 size_t currentNodeIndex,
                                 const std::vector<ObjectFileRef>& objects,
                                 const ObjectFileRef& pathObject,
                                 size_t targetNodeIndex)
  {
    nodes.emplace_back(database,
                       currentNodeIndex,
                       objects,
                       pathObject,
                       targetNodeIndex);
  }

  RouteDescriptionGenerator::Callback::~Callback()
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::BeforeRoute()
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnStart(const RouteDescription::StartDescriptionRef& /*startDescription*/,
                                                    const RouteDescription::TypeNameDescriptionRef& /*typeNameDescription*/,
                                                    const RouteDescription::NameDescriptionRef& /*nameDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnTargetReached(const RouteDescription::TargetDescriptionRef& /*targetDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnTurn(const RouteDescription::TurnDescriptionRef& /*turnDescription*/,
                                                   const RouteDescription::CrossingWaysDescriptionRef& /*crossingWaysDescription*/,
                                                   const RouteDescription::DirectionDescriptionRef& /*directionDescription*/,
                                                   const RouteDescription::TypeNameDescriptionRef& /*typeNameDescription*/,
                                                   const RouteDescription::NameDescriptionRef& /*nameDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnRoundaboutEnter(const RouteDescription::RoundaboutEnterDescriptionRef& /*roundaboutEnterDescription*/,
                                                              const RouteDescription::CrossingWaysDescriptionRef& /*crossingWaysDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnRoundaboutLeave(const RouteDescription::RoundaboutLeaveDescriptionRef& /*roundaboutLeaveDescription*/,
                                                              const RouteDescription::NameDescriptionRef& /*nameDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnMotorwayEnter(const RouteDescription::MotorwayEnterDescriptionRef& /*motorwayEnterDescription*/,
                                                            const RouteDescription::CrossingWaysDescriptionRef& /*crossingWaysDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnMotorwayChange(const RouteDescription::MotorwayChangeDescriptionRef& /*motorwayChangeDescription*/,
                                                             const RouteDescription::MotorwayJunctionDescriptionRef& /*motorwayJunctionDescription*/,
                                                             const RouteDescription::DestinationDescriptionRef& /*crossingDestinationDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnMotorwayLeave(const RouteDescription::MotorwayLeaveDescriptionRef& /*motorwayLeaveDescription*/,
                                                            const RouteDescription::MotorwayJunctionDescriptionRef& /*motorwayJunctionDescription*/,
                                                            const RouteDescription::DirectionDescriptionRef& /*directionDescription*/,
                                                            const RouteDescription::NameDescriptionRef& /*nameDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnPathNameChange(const RouteDescription::NameChangedDescriptionRef& /*nameChangedDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnMaxSpeed(const RouteDescription::MaxSpeedDescriptionRef& /*maxSpeedDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::OnPOIAtRoute(const RouteDescription::POIAtRouteDescriptionRef& /*poiAtRouteDescription*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::BeforeNode(const RouteDescription::Node& /*node*/)
  {
    // no code
  }

  void RouteDescriptionGenerator::Callback::AfterNode(const RouteDescription::Node& /*node*/)
  {
    // no code
  }


  /**
   * Evaluate the already postprocessed RouteDescription and call the given callback for
   * node segments where something important happens or changes.
   *
   * @param description
   * @param callback
   */
  void RouteDescriptionGenerator::GenerateDescription(const RouteDescription& description,
                                                      RouteDescriptionGenerator::Callback& callback)
  {
    callback.BeforeRoute();

    auto prevNode=description.Nodes().cend();
    for (auto node=description.Nodes().cbegin();
         node!=description.Nodes().cend();
         ++node) {
      osmscout::RouteDescription::DescriptionRef                 desc;
      osmscout::RouteDescription::NameDescriptionRef             nameDescription;
      osmscout::RouteDescription::DirectionDescriptionRef        directionDescription;
      osmscout::RouteDescription::NameChangedDescriptionRef      nameChangedDescription;
      osmscout::RouteDescription::CrossingWaysDescriptionRef     crossingWaysDescription;

      osmscout::RouteDescription::StartDescriptionRef            startDescription;
      osmscout::RouteDescription::TargetDescriptionRef           targetDescription;
      osmscout::RouteDescription::TurnDescriptionRef             turnDescription;
      osmscout::RouteDescription::RoundaboutEnterDescriptionRef  roundaboutEnterDescription;
      osmscout::RouteDescription::RoundaboutLeaveDescriptionRef  roundaboutLeaveDescription;
      osmscout::RouteDescription::MotorwayEnterDescriptionRef    motorwayEnterDescription;
      osmscout::RouteDescription::MotorwayChangeDescriptionRef   motorwayChangeDescription;
      osmscout::RouteDescription::MotorwayLeaveDescriptionRef    motorwayLeaveDescription;
      osmscout::RouteDescription::MotorwayJunctionDescriptionRef motorwayJunctionDescription;
      osmscout::RouteDescription::DestinationDescriptionRef      crossingDestinationDescription;
      osmscout::RouteDescription::MaxSpeedDescriptionRef         maxSpeedDescription;
      osmscout::RouteDescription::TypeNameDescriptionRef         typeNameDescription;
      osmscout::RouteDescription::POIAtRouteDescriptionRef       poiAtRouteDescription;

      desc=node->GetDescription(osmscout::RouteDescription::WAY_NAME_DESC);
      if (desc) {
        nameDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::NameDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::DIRECTION_DESC);
      if (desc) {
        directionDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::DirectionDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::WAY_NAME_CHANGED_DESC);
      if (desc) {
        nameChangedDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::NameChangedDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::CROSSING_WAYS_DESC);
      if (desc) {
        crossingWaysDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::CrossingWaysDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::NODE_START_DESC);
      if (desc) {
        startDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::StartDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::NODE_TARGET_DESC);
      if (desc) {
        targetDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::TargetDescription>(desc);
      }


      desc=node->GetDescription(osmscout::RouteDescription::TURN_DESC);
      if (desc) {
        turnDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::TurnDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::ROUNDABOUT_ENTER_DESC);
      if (desc) {
        roundaboutEnterDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::RoundaboutEnterDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::ROUNDABOUT_LEAVE_DESC);
      if (desc) {
        roundaboutLeaveDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::RoundaboutLeaveDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::MOTORWAY_ENTER_DESC);
      if (desc) {
        motorwayEnterDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::MotorwayEnterDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::MOTORWAY_CHANGE_DESC);
      if (desc) {
        motorwayChangeDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::MotorwayChangeDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::MOTORWAY_LEAVE_DESC);
      if (desc) {
        motorwayLeaveDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::MotorwayLeaveDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::MOTORWAY_JUNCTION_DESC);
      if (desc) {
        motorwayJunctionDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::MotorwayJunctionDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::CROSSING_DESTINATION_DESC);
      if (desc) {
        crossingDestinationDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::DestinationDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::WAY_MAXSPEED_DESC);
      if (desc) {
        maxSpeedDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::MaxSpeedDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::WAY_TYPE_NAME_DESC);
      if (desc) {
        typeNameDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::TypeNameDescription>(desc);
      }

      desc=node->GetDescription(osmscout::RouteDescription::POI_AT_ROUTE_DESC);
      if (desc) {
        poiAtRouteDescription=std::dynamic_pointer_cast<osmscout::RouteDescription::POIAtRouteDescription>(desc);
      }

      callback.BeforeNode(*node);

      if (startDescription) {
        callback.OnStart(startDescription,
                         typeNameDescription,
                         nameDescription);
      }

      if (targetDescription) {
        callback.OnTargetReached(targetDescription);
      }

      if (turnDescription) {
        callback.OnTurn(turnDescription,
                        crossingWaysDescription,
                        directionDescription,
                        typeNameDescription,
                        nameDescription);
      }
      else if (roundaboutEnterDescription) {
        callback.OnRoundaboutEnter(roundaboutEnterDescription,
                                   crossingWaysDescription);
      }
      else if (roundaboutLeaveDescription) {
        callback.OnRoundaboutLeave(roundaboutLeaveDescription,
                                   nameDescription);
      }

      if (motorwayEnterDescription) {
        callback.OnMotorwayEnter(motorwayEnterDescription,
                                 crossingWaysDescription);
      }
      else if (motorwayChangeDescription) {
        callback.OnMotorwayChange(motorwayChangeDescription,
                                  motorwayJunctionDescription,
                                  crossingDestinationDescription);
      }
      else if (motorwayLeaveDescription) {
        callback.OnMotorwayLeave(motorwayLeaveDescription,
                                 motorwayJunctionDescription,
                                 directionDescription,
                                 nameDescription);
      }
      else if (nameChangedDescription) {
        callback.OnPathNameChange(nameChangedDescription);
      }

      if (maxSpeedDescription) {
        callback.OnMaxSpeed(maxSpeedDescription);
      }

      if (poiAtRouteDescription) {
        callback.OnPOIAtRoute(poiAtRouteDescription);
      }

      callback.AfterNode(*node);

      prevNode=node;
    }
  }
}

