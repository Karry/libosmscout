/*
  This source is part of the libosmscout library
  Copyright (C) 2016  Tim Teulings

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

#include <osmscout/util/TileId.h>

#include <osmscout/util/Geometry.h>

namespace osmscout {

  /**
   * Ceate a new tile by passing magnification and tile coordinates
   */
  TileId::TileId(uint32_t x,
                 uint32_t y)
  : x(x),
    y(y)
  {
    // no code
  }

  /**
   * Return a short human readable description of the tile id
   */
  std::string TileId::GetDisplayText() const
  {
    return std::to_string(y) + "." + std::to_string(x);
  }

  /**
   * Compare tile ids for equality
   */
  bool TileId::operator==(const TileId& other) const
  {
    return y==other.y &&
           x==other.x;
  }

  /**
   * Compare tile ids for inequality
   */
  bool TileId::operator!=(const TileId& other) const
  {
    return y!=other.y ||
           x!=other.x;
  }

  /**
   * Compare tile ids by their order. Needed for sorting tile ids and placing them into (some)
   * containers.
   */
  bool TileId::operator<(const TileId& other) const
  {
    if (y!=other.y) {
      return y<other.y;
    }

    return x<other.x;
  }

  /**
   * Return the libosmscout-specific tile id for the given magnification that contains the given
   * coordinate.
   *
   * @param magnification
   *    Magnification to use
   * @param coord
   *    Coordinate that should be covered by the tile
   * @return
   *    A tile id
   */
  TileId TileId::GetTile(const Magnification& magnification,
                         const GeoCoord& coord)
  {
    return {uint32_t((coord.GetLon()+180.0)/cellDimension[magnification.GetLevel()].width),
            uint32_t((coord.GetLat()+90.0)/cellDimension[magnification.GetLevel()].height)};
  }

  /**
   * Return the libosmscout-specific tile id for the given magnification that contains the given
   * coordinate.
   *
   * @param level
   *    Level to use (magnification.GetLevel())
   * @param coord
   *    Coordinate that should be covered by the tile
   * @return
   *    A tile id
   */
  TileId TileId::GetTile(MagnificationLevel level,
                         const GeoCoord& coord)
  {
    return {uint32_t((coord.GetLon()+180.0)/cellDimension[level.Get()].width),
            uint32_t((coord.GetLat()+90.0)/cellDimension[level.Get()].height)};
  }

  TileKey::TileKey(const Magnification& magnification,
                   const TileId& id)
  : level(magnification.GetLevel()),
    id(id)
  {
  }

  GeoBox TileKey::GetBoundingBox() const
  {
    return GeoBox(GeoCoord(id.GetY()*cellDimension[level].height-90.0,
                           id.GetX()*cellDimension[level].width-180.0),
                  GeoCoord((id.GetY()+1)*cellDimension[level].height-90.0,
                           (id.GetX()+1)*cellDimension[level].width-180.0));
  }


  /**
   * Return a short human readable description of the tile id
   */
  std::string TileKey::GetDisplayText() const
  {
    return std::to_string(level)+ "." + std::to_string(id.GetY()) + "." + std::to_string(id.GetX());
  }

  /**
   * Return the parent tile.
   *
   * Note that the parent tile will cover a 4 times bigger region than the current tile.
   *
   * Note that for tiles with level 0 no parent tile will exist. The method will assert in this case!
   */
  TileKey TileKey::GetParent() const
  {
    Magnification zoomedOutMagnification;

    assert(level>0);

    zoomedOutMagnification.SetLevel(MagnificationLevel(level-1));

    return TileKey(zoomedOutMagnification,
                   TileId(id.GetX()/2,id.GetY()/2));
  }

  /**
   * Compare tile ids for equality
   */
  bool TileKey::operator==(const TileKey& other) const
  {
    return level==other.level &&
           id==other.id;
  }

  /**
   * Compare tile ids for inequality
   */
  bool TileKey::operator!=(const TileKey& other) const
  {
    return level!=other.level ||
           id!=other.id;
  }

  /**
   * Compare tile ids by their order. Needed for sorting tile ids and placing them into (some)
   * containers.
   */
  bool TileKey::operator<(const TileKey& other) const
  {
    if (level!=other.level) {
      return level<other.level;
    }

    return id<other.id;
  }

  TileIdBox::TileIdBox(const TileId& a,
                       const TileId& b)
    : minTile(std::min(a.GetX(),b.GetX()),
              std::min(a.GetY(),b.GetY())),
      maxTile(std::max(a.GetX(),b.GetX()),
              std::max(a.GetY(),b.GetY()))
  {
  }

  /**
   * Return the bounding box of the region defined by the box
   *
   * @return
   *    The GeoBox defining the resulting area
   */
  GeoBox TileIdBox::GetBoundingBox(const Magnification& magnification) const
  {
    TileKey minKey(magnification,minTile);
    TileKey maxPlusKey(magnification,TileId(maxTile.GetX()+1,maxTile.GetY()+1));

    return GeoBox(minKey.GetBoundingBox().GetTopLeft(),
                  maxPlusKey.GetBoundingBox().GetTopLeft());
  }
}
