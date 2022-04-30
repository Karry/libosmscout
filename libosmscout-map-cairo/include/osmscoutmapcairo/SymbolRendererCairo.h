#ifndef OSMSCOUT_MAP_QT_SYMBOLRENDERERCAIRO_H
#define OSMSCOUT_MAP_QT_SYMBOLRENDERERCAIRO_H

/*
  This source is part of the libosmscout-map library
  Copyright (C) 2010  Tim Teulings
  Copyright (C) 2022  Lukas Karas

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

#include <osmscoutmapcairo/MapCairoImportExport.h>
#include <osmscoutmap/SymbolRenderer.h>

#include <osmscoutmap/Styles.h>

#if defined(__WIN32__) || defined(WIN32)
#include <cairo.h>
#else
#include <cairo/cairo.h>
#endif

namespace osmscout {

/**
 * \ingroup Renderer
 */
class OSMSCOUT_MAP_CAIRO_API SymbolRendererCairo: public SymbolRenderer {
private:
  cairo_t *draw;
public:
  explicit SymbolRendererCairo(cairo_t *draw);
  SymbolRendererCairo(const SymbolRendererCairo&) = default;
  SymbolRendererCairo(SymbolRendererCairo&&) = default;

  ~SymbolRendererCairo() override = default;

  SymbolRendererCairo& operator=(const SymbolRendererCairo&) = default;
  SymbolRendererCairo& operator=(SymbolRendererCairo&&) = default;

protected:
  void DrawPolygon(const std::vector<Vertex2D> &polygonPixels) const override;
  void DrawRect(double x, double y, double w, double h) const override;
  void DrawCircle(double x, double y, double radius) const override;
  void AfterDraw(const FillStyleRef &fillStyle, const BorderStyleRef &borderStyle, double screenMmInPixel) const override;

private:
  void SetLineAttributes(const Color &color,
                         double width,
                         const std::vector<double> &dash) const;
};
}

#endif // OSMSCOUT_MAP_QT_SYMBOLRENDERERCAIRO_H
