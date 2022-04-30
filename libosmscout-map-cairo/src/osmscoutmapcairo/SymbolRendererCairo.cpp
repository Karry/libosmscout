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

#include <osmscout/util/Logger.h>
#include <osmscoutmapcairo/SymbolRendererCairo.h>

namespace osmscout {

SymbolRendererCairo::SymbolRendererCairo(cairo_t *draw):
  draw(draw)
{}

void SymbolRendererCairo::DrawPolygon(const std::vector<Vertex2D> &polygonPixels) const
{
  cairo_new_path(draw);

  for (auto pixel=polygonPixels.begin();
       pixel!=polygonPixels.end();
       ++pixel) {
    if (pixel==polygonPixels.begin()) {
      cairo_move_to(draw,
                    pixel->GetX(),
                    pixel->GetY());
    }
    else {
      cairo_line_to(draw,
                    pixel->GetX(),
                    pixel->GetY());
    }
  }

  cairo_close_path(draw);
}

void SymbolRendererCairo::DrawRect(double x, double y, double w, double h) const
{
  cairo_new_path(draw);
  cairo_rectangle(draw, x, y, w, h);
}

void SymbolRendererCairo::DrawCircle(double x, double y, double radius) const
{
  cairo_new_path(draw);
  cairo_arc(draw, x, y, radius, 0,2*M_PI);
}

void SymbolRendererCairo::SetLineAttributes(const Color &color,
                                            double width,
                                            const std::vector<double> &dash) const
{
  assert(dash.size() <= 10);

  cairo_set_source_rgba(draw,
                        color.GetR(),
                        color.GetG(),
                        color.GetB(),
                        color.GetA());

  cairo_set_line_width(draw, width);

  if (dash.empty()) {
    cairo_set_dash(draw, nullptr, 0, 0);
  }
  else {
    std::array<double,10> dashArray;

    for (size_t i = 0; i < dash.size(); i++) {
      dashArray[i] = dash[i] * width;
    }
    cairo_set_dash(draw, dashArray.data(), static_cast<int>(dash.size()), 0.0);
  }
}


void SymbolRendererCairo::AfterDraw(const FillStyleRef &fill,
                                    const BorderStyleRef &border,
                                    double screenMmInPixel) const
{
  bool hasFill = false;
  bool hasBorder = false;

  if (fill) {
    if (fill->HasPattern()) {
      log.Warn() << "Pattern is not supported for symbols";
    }
    if (fill->GetFillColor().IsVisible()) {
      Color color = fill->GetFillColor();
      cairo_set_source_rgba(draw,
                            color.GetR(),
                            color.GetG(),
                            color.GetB(),
                            color.GetA());
      hasFill = true;
    }
  }

  if (border) {
    hasBorder = border->GetWidth() > 0 &&
                border->GetColor().IsVisible();
  }

  if (hasFill && hasBorder) {
    cairo_fill_preserve(draw);
  } else if (hasFill) {
    cairo_fill(draw);
  }

  if (hasBorder) {
    double borderWidth = screenMmInPixel * border->GetWidth();

    SetLineAttributes(border->GetColor(),
                      borderWidth,
                      border->GetDash());

    cairo_set_line_cap(draw, CAIRO_LINE_CAP_BUTT);

    cairo_stroke(draw);
  }
}
}
