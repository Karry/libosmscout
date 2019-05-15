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

#include <osmscout/AreaNodeIndex.h>

#include <algorithm>

#include <osmscout/util/File.h>
#include <osmscout/util/Geometry.h>
#include <osmscout/util/Logger.h>
#include <osmscout/util/StopClock.h>
#include <osmscout/util/TileId.h>

#include <osmscout/system/Math.h>

#include <iostream>
namespace osmscout {

  FileOffset AreaNodeIndex::TypeData18::GetDataOffset() const
  {
    return indexOffset+cellXCount*cellYCount*(FileOffset)dataOffsetBytes;
  }

  FileOffset AreaNodeIndex::TypeData18::GetCellOffset(size_t x, size_t y) const
  {
    return indexOffset+((y-cellYStart)*cellXCount+x-cellXStart)*dataOffsetBytes;
  }

  AreaNodeIndex::TypeData18::TypeData18()
      : indexLevel(0),
        indexOffset(0),
        dataOffsetBytes(0),
        cellXStart(0),
        cellXEnd(0),
        cellYStart(0),
        cellYEnd(0),
        cellXCount(0),
        cellYCount(0),
        cellWidth(0.0),
        cellHeight(0.0),
        minLon(0.0),
        maxLon(0.0),
        minLat(0.0),
        maxLat(0.0)
  {}

  bool AreaNodeIndex::Open18(const std::string& path,
                             bool memoryMappedData,
                             uint32_t fileFormatVersion)
  {
    std::string datafilename=AppendFileToDir(path,AREA_NODE_IDX);

    try {
      scanner.Open(datafilename,
                   FileScanner::FastRandom,
                   memoryMappedData,
                   fileFormatVersion);

      uint32_t indexEntries;

      scanner.Read(indexEntries);

      for (size_t i=0; i<indexEntries; i++) {
        TypeId type;

        scanner.ReadNumber(type);

        if (type>=nodeTypeData18.size()) {
          nodeTypeData18.resize(type+1);
        }

        scanner.ReadFileOffset(nodeTypeData18[type].indexOffset);
        scanner.Read(nodeTypeData18[type].dataOffsetBytes);

        scanner.ReadNumber(nodeTypeData18[type].indexLevel);

        scanner.ReadNumber(nodeTypeData18[type].cellXStart);
        scanner.ReadNumber(nodeTypeData18[type].cellXEnd);
        scanner.ReadNumber(nodeTypeData18[type].cellYStart);
        scanner.ReadNumber(nodeTypeData18[type].cellYEnd);

        nodeTypeData18[type].cellXCount=nodeTypeData18[type].cellXEnd-nodeTypeData18[type].cellXStart+1;
        nodeTypeData18[type].cellYCount=nodeTypeData18[type].cellYEnd-nodeTypeData18[type].cellYStart+1;

        nodeTypeData18[type].cellWidth=cellDimension[nodeTypeData18[type].indexLevel].width;
        nodeTypeData18[type].cellHeight=cellDimension[nodeTypeData18[type].indexLevel].height;

        nodeTypeData18[type].minLon=nodeTypeData18[type].cellXStart*nodeTypeData18[type].cellWidth-180.0;
        nodeTypeData18[type].maxLon=(nodeTypeData18[type].cellXEnd+1)*nodeTypeData18[type].cellWidth-180.0;
        nodeTypeData18[type].minLat=nodeTypeData18[type].cellYStart*nodeTypeData18[type].cellHeight-90.0;
        nodeTypeData18[type].maxLat=(nodeTypeData18[type].cellYEnd+1)*nodeTypeData18[type].cellHeight-90.0;
      }

      return !scanner.HasError();
    }
    catch (IOException& e) {
      log.Error() << e.GetDescription();

      return false;
    }
  }

  bool AreaNodeIndex::GetOffsets18(const TypeData18& typeData,
                                   const GeoBox& boundingBox,
                                   std::vector<FileOffset>& offsets) const
  {
    if (typeData.indexOffset==0) {

      // No data for this type available
      return true;
    }

    if (boundingBox.GetMaxLon()<typeData.minLon ||
        boundingBox.GetMinLon()>=typeData.maxLon ||
        boundingBox.GetMaxLat()<typeData.minLat ||
        boundingBox.GetMinLat()>=typeData.maxLat) {

      // No data available in given bounding box
      return true;
    }

    uint32_t minxc=(uint32_t)floor((boundingBox.GetMinLon()+180.0)/typeData.cellWidth);
    uint32_t maxxc=(uint32_t)floor((boundingBox.GetMaxLon()+180.0)/typeData.cellWidth);

    uint32_t minyc=(uint32_t)floor((boundingBox.GetMinLat()+90.0)/typeData.cellHeight);
    uint32_t maxyc=(uint32_t)floor((boundingBox.GetMaxLat()+90.0)/typeData.cellHeight);

    minxc=std::max(minxc,typeData.cellXStart);
    maxxc=std::min(maxxc,typeData.cellXEnd);

    minyc=std::max(minyc,typeData.cellYStart);
    maxyc=std::min(maxyc,typeData.cellYEnd);

    FileOffset dataOffset=typeData.GetDataOffset();

    // For each row
    for (size_t y=minyc; y<=maxyc; y++) {
      std::lock_guard<std::mutex> guard(lookupMutex);
      FileOffset                  initialCellDataOffset=0;
      size_t                      cellDataOffsetCount=0;
      FileOffset                  cellIndexOffset=typeData.GetCellOffset(minxc,y);

      scanner.SetPos(cellIndexOffset);

      // For each column in row
      for (size_t x=minxc; x<=maxxc; x++) {
        FileOffset cellDataOffset;

        scanner.ReadFileOffset(cellDataOffset,
                               typeData.dataOffsetBytes);

        if (cellDataOffset==0) {
          continue;
        }

        // We added +1 during import and now substract it again
        cellDataOffset--;

        if (initialCellDataOffset==0) {
          initialCellDataOffset=dataOffset+cellDataOffset;
        }

        cellDataOffsetCount++;
      }

      if (cellDataOffsetCount==0) {
        continue;
      }

      assert(initialCellDataOffset>=cellIndexOffset);

      scanner.SetPos(initialCellDataOffset);

      // For each data cell in row found
      for (size_t i=0; i<cellDataOffsetCount; i++) {
        uint32_t   dataCount;
        FileOffset lastOffset=0;


        scanner.ReadNumber(dataCount);

        for (size_t d=0; d<dataCount; d++) {
          FileOffset objectOffset;

          scanner.ReadNumber(objectOffset);

          objectOffset+=lastOffset;

          offsets.push_back(objectOffset);

          lastOffset=objectOffset;
        }
      }
    }

    return true;
  }

  bool AreaNodeIndex::GetOffsets18(const GeoBox& boundingBox,
                                   const TypeInfoSet& requestedTypes,
                                   std::vector<FileOffset>& offsets,
                                   TypeInfoSet& loadedTypes) const
  {

    StopClock time;

    loadedTypes.Clear();

    offsets.reserve(std::min((size_t)10000,offsets.capacity()));

    try {
      for (const TypeInfoRef& type : requestedTypes) {
        if (type->IsInternal()) {
          continue;
        }

        auto typeNodeId = type->GetNodeId();
        if (typeNodeId < nodeTypeData18.size() &&
            !GetOffsets18(nodeTypeData18[typeNodeId], boundingBox, offsets)) {
          return false;
        }

        loadedTypes.Set(type);
      }
    }
    catch (IOException& e) {
      log.Error() << e.GetDescription();

      return false;
    }

    time.Stop();

    if (time.GetMilliseconds()>100) {
      log.Warn() << "Retrieving " << offsets.size()
                 << " node offsets from area index for "
                 << boundingBox.GetDisplayText()
                 << " took " << time.ResultString();
    }

    return true;
  }

}
