#ifndef LIBOSMSCOUT_AREAYINDEX_H
#define LIBOSMSCOUT_AREAYINDEX_H
/*
  This source is part of the libosmscout library
  Copyright (C) 2011  Tim Teulings

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
#include <mutex>
#include <unordered_set>
#include <vector>

#include <osmscout/TypeConfig.h>
#include <osmscout/TypeInfoSet.h>

#include <osmscout/util/FileScanner.h>
#include <osmscout/util/TileId.h>

namespace osmscout {

  /**
    \ingroup Database
    Generic area index for lookup objects by area
    */
  class OSMSCOUT_API AreaIndex
  {
  protected:
    struct TypeData
    {
      TypeInfoRef         type;
      MagnificationLevel  indexLevel;

      uint8_t             dataOffsetBytes;
      FileOffset          bitmapOffset;

      TileIdBox           tileBox;

      GeoBox              boundingBox;

      TypeData();

      FileOffset GetDataOffset() const;
      FileOffset GetCellOffset(size_t x, size_t y) const;
    };

    std::string           indexFileName;
    std::string           fullIndexFileName;  //!< Full path and name of the data file
    mutable FileScanner   scanner;            //!< Scanner instance for reading this file

    std::vector<TypeData> typeData;

    mutable std::mutex    lookupMutex;

  protected:
    void GetOffsets(const TypeData& typeData,
                    const GeoBox& boundingBox,
                    std::unordered_set<FileOffset>& offsets) const;

    AreaIndex(const std::string &indexFileName);

    virtual void ReadTypeData(const TypeConfigRef& typeConfig,
                              TypeData &data) = 0;

  public:
    virtual ~AreaIndex();

    void Close();
    virtual bool Open(const TypeConfigRef& typeConfig,
                      const std::string& path,
                      bool memoryMappedData);

    inline bool IsOpen() const
    {
      return scanner.IsOpen();
    }

    inline std::string GetFilename() const
    {
      return fullIndexFileName;
    }

    virtual bool GetOffsets(const GeoBox& boundingBox,
                            const TypeInfoSet& types,
                            std::vector<FileOffset>& offsets,
                            TypeInfoSet& loadedTypes) const;
  };
}

#endif //LIBOSMSCOUT_AREAYINDEX_H
