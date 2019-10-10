#ifndef OSMSCOUT_AREA_H
#define OSMSCOUT_AREA_H

/*
  This source is part of the libosmscout library
  Copyright (C) 2013  Tim Teulings

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

#include <memory>

#include <osmscout/GeoCoord.h>
#include <osmscout/Point.h>

#include <osmscout/TypeConfig.h>

#include <osmscout/util/FileScanner.h>
#include <osmscout/util/FileWriter.h>
#include <osmscout/util/GeoBox.h>
#include <osmscout/util/Progress.h>
#include <osmscout/util/Geometry.h>

#include <osmscout/system/Compiler.h>

namespace osmscout {
  /**
   * Representation of an (complex/multipolygon) area.
   *
   * It consists from hierarchy of rings:
   *  - optional master ring (ring number 0)
   *  - outer ring(s) (ring number 1)
   *  - inner ring(s) (ring number > 1)
   *
   *  When area consists just from simple line (usual building), Area will contains just one outer ring (ring number 0)
   *
   *  When area is multipolygon relation, type and features of such relation is stored in master ring.
   *
   */
  class OSMSCOUT_API Area CLASS_FINAL
  {
  public:
    static const uint8_t masterRingId;
    static const uint8_t outerRingId;

  public:
    class OSMSCOUT_API Ring
    {
    private:
      FeatureValueBuffer    featureValueBuffer; //!< List of features
      uint8_t               ring;               //!< The ring hierarchy number (0...n)

    public:
      /**
       * Note that ring nodes, bbox and segments fields are public for simple manipulation.
       * User that modify it is responsible to keep these values in sync!
       * You should not rely on segments and bbox, it is just a cache used some algorithms.
       * It may be empty/invalid!
       */
      std::vector<Point>          nodes;        //!< The array of coordinates
      std::vector<SegmentGeoBox>  segments;     //!< Precomputed (cache) segment bounding boxes for optimisation
      GeoBox                      bbox;         //!< Precomputed (cache) bounding box

    public:
      inline Ring()
      : ring(0)
      {
        // no code
      }

      inline TypeInfoRef GetType() const
      {
        return featureValueBuffer.GetType();
      }

      inline size_t GetFeatureCount() const
      {
        return featureValueBuffer.GetType()->GetFeatureCount();
      }

      inline bool HasFeature(size_t idx) const
      {
        return featureValueBuffer.HasFeature(idx);
      }

      bool HasAnyFeaturesSet() const;

      inline const FeatureInstance& GetFeature(size_t idx) const
      {
        return featureValueBuffer.GetType()->GetFeature(idx);
      }

      inline void UnsetFeature(size_t idx)
      {
        featureValueBuffer.FreeValue(idx);
      }

      inline const FeatureValueBuffer& GetFeatureValueBuffer() const
      {
        return featureValueBuffer;
      }

      inline bool IsMasterRing() const
      {
        return ring==masterRingId;
      }

      // top level outer ring
      inline bool IsOuterRing() const
      {
        return ring==outerRingId;
      }

      // ring level is odd, it is some outer ring
      inline bool IsSomeOuterRing() const
      {
        return (ring & outerRingId) == outerRingId;
      }

      inline uint8_t GetRing() const
      {
        return ring;
      }

      inline Id GetSerial(size_t index) const
      {
        return nodes[index].GetSerial();
      }

      inline Id GetId(size_t index) const
      {
        return nodes[index].GetId();
      }

      inline Id GetFrontId() const
      {
        return nodes.front().GetId();
      }

      inline Id GetBackId() const
      {
        return nodes.back().GetId();
      }

      bool GetNodeIndexByNodeId(Id id,
                                size_t& index) const;

      inline const GeoCoord& GetCoord(size_t index) const
      {
        return nodes[index].GetCoord();
      }

      bool GetCenter(GeoCoord& center) const;

      void GetBoundingBox(GeoBox& boundingBox) const;
      GeoBox GetBoundingBox() const;

      inline void SetType(const TypeInfoRef& type)
      {
        featureValueBuffer.SetType(type);
      }

      inline void SetFeatures(const FeatureValueBuffer& buffer)
      {
        featureValueBuffer.Set(buffer);
      }

      inline void CopyMissingValues(const FeatureValueBuffer& buffer)
      {
        featureValueBuffer.CopyMissingValues(buffer);
      }

      inline void MarkAsMasterRing()
      {
        ring=masterRingId;
      }

      inline void MarkAsOuterRing()
      {
        ring=outerRingId;
      }

      inline void SetRing(uint8_t ring)
      {
        this->ring=ring;
      }

      friend class Area;
    };

  private:
    FileOffset        fileOffset;
    FileOffset        nextFileOffset;

  public:
    std::vector<Ring> rings;

  public:
    inline Area()
    : fileOffset(0),nextFileOffset(0)
    {
      // no code
    }

    inline FileOffset GetFileOffset() const
    {
      return fileOffset;
    }

    inline FileOffset GetNextFileOffset() const
    {
      return nextFileOffset;
    }

    inline ObjectFileRef GetObjectFileRef() const
    {
      return {fileOffset,refArea};
    }

    inline TypeInfoRef GetType() const
    {
      return rings.front().GetType();
    }

    inline const FeatureValueBuffer& GetFeatureValueBuffer() const
    {
      return rings.front().GetFeatureValueBuffer();
    }

    inline bool IsSimple() const
    {
      return rings.size()==1;
    }

    bool GetCenter(GeoCoord& center) const;

    GeoBox GetBoundingBox() const;

    /**
     * Returns true if the bounding box of the object intersects the given
     * bounding box
     *
     * @param boundingBox
     *    bounding box to test for intersection
     * @return
     *    true on intersection, else false
     */
    inline bool Intersects(const GeoBox& boundingBox) const
    {
      return GetBoundingBox().Intersects(boundingBox);
    }

    /**
     * Read the area as written by Write().
     */
    void Read(const TypeConfig& typeConfig,
              FileScanner& scanner);

    /**
     * Read the area as written by WriteImport().
     */
    void ReadImport(const TypeConfig& typeConfig,
                    FileScanner& scanner);

    /**
     * Read the area as stored by WriteOptimized().
     */
    void ReadOptimized(const TypeConfig& typeConfig,
                       FileScanner& scanner);

    /**
     * Write the area with all data required in the
     * standard database.
     */
    void Write(const TypeConfig& typeConfig,
               FileWriter& writer) const;

    /**
     * Write the area with all data required during import,
     * certain optimizations done on the final data
     * are not done here to not loose information.
     */
    void WriteImport(const TypeConfig& typeConfig,
                     FileWriter& writer) const;

    /**
     * Write the area with all data required by the OptimizeLowZoom
     * index, dropping all ids.
     */
    void WriteOptimized(const TypeConfig& typeConfig,
                        FileWriter& writer) const;
  };

  typedef std::shared_ptr<Area> AreaRef;
}

#endif
