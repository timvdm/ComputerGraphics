#include "render.h"

#include "../libgfx/mesh.h"

#include <limits>

using namespace GFX;

std::pair<Point2D, Point2D> get_min_max(const Lines2D &lines)
{
  Real xMin = std::numeric_limits<Real>::max();
  Real xMax = std::numeric_limits<Real>::min();
  Real yMin = std::numeric_limits<Real>::max();
  Real yMax = std::numeric_limits<Real>::min();

  for (Lines2D::const_iterator line = lines.begin(); line != lines.end(); ++line) {
    // p1.x
    if (line->p1.x < xMin)
      xMin = line->p1.x;
    if (line->p1.x > xMax)
      xMax = line->p1.x;
    // p1.y
    if (line->p1.y < yMin)
      yMin = line->p1.y;
    if (line->p1.y > yMax)
      yMax = line->p1.y;
    // p2.x
    if (line->p2.x < xMin)
      xMin = line->p2.x;
    if (line->p2.x > xMax)
      xMax = line->p2.x;
    // p2.y
    if (line->p2.y < yMin)
      yMin = line->p2.y;
    if (line->p2.y > yMax)
      yMax = line->p2.y;
  }

  return std::make_pair(Point2D(xMin, yMin), Point2D(xMax, yMax));
}

std::pair<int, int> get_image_sizes(const std::pair<Point2D, Point2D> &minMax, int size)
{
  Real xRange = minMax.second.x - minMax.first.x;
  Real yRange = minMax.second.y - minMax.first.y;

  int xSize = size * xRange / std::max(xRange, yRange);
  int ySize = size * yRange / std::max(xRange, yRange);

  return std::make_pair(xSize, ySize);
}

Real get_scale_factor(const std::pair<Point2D, Point2D> &minMax, int xSize, Real border)
{
  return border * xSize / (minMax.second.x - minMax.first.x);
}

void scale_lines(Lines2D &lines, Real d)
{
  for (Lines2D::iterator line = lines.begin(); line != lines.end(); ++line) {
    line->p1.x *= d;
    line->p1.y *= d;
    line->p2.x *= d;
    line->p2.y *= d;
  }
}

Point2D get_center(const std::pair<Point2D, Point2D> &minMax, Real d)
{
  Real xDC = d * (minMax.first.x + minMax.second.x) / 2;
  Real yDC = d * (minMax.first.y + minMax.second.y) / 2;

  return Point2D(xDC, yDC);
}

void center_lines(Lines2D &lines, const std::pair<int, int> &imageSizes, const Point2D &center)
{
  Real xAdd = imageSizes.first / 2.0 - center.x;
  Real yAdd = imageSizes.second / 2.0 - center.y;

  for (Lines2D::iterator line = lines.begin(); line != lines.end(); ++line) {
    line->p1.x += xAdd;
    line->p1.y += yAdd;
    line->p2.x += xAdd;
    line->p2.y += yAdd;
  }
}

img::EasyImage draw_lines(Lines2D &lines, int size, const img::Color &bgColor)
{
  // compute some properties for the lines
  std::pair<Point2D, Point2D> minMax = get_min_max(lines);
  std::pair<int, int> imageSizes = get_image_sizes(minMax, size);
  double d = get_scale_factor(minMax, imageSizes.first);
  Point2D center = get_center(minMax, d);

  // scale the lines
  scale_lines(lines, d);
  // center the lines
  center_lines(lines, imageSizes, center);

  // draw the lines
  img::EasyImage image(imageSizes.first, imageSizes.second, bgColor);
  for (std::size_t i = 0; i < lines.size(); ++i) {
    //std::cout << lines[i].p1 << " -> " << lines[i].p2 << std::endl;
    image.draw_line(lines[i].p1.x + 0.5, lines[i].p1.y + 0.5, lines[i].p2.x + 0.5, lines[i].p2.y + 0.5, 
        img::Color(lines[i].color.r, lines[i].color.g, lines[i].color.b));
  }

  return image;
}

void renderMesh(const GFX::Mesh &mesh, const GFX::Color &color, const GFX::mat4 &T, GFX::Lines2D &lines)
{
  GFX::vec4 p1, p2;
  for (std::size_t i = 0; i < mesh.faces().size(); ++i) {
    const std::vector<int> &face = mesh.faces()[i];
    for (std::size_t j = 0; j < face.size(); ++j) {
      if (!j) {
        p1 = mesh.vertices()[face[face.size() - 1]];
        p2 = mesh.vertices()[face[0]];
      } else {
        p1 = mesh.vertices()[face[j-1]];
        p2 = mesh.vertices()[face[j]];
      }

      p1 = T * p1;
      p2 = T * p2;

      GFX::Point2D projP1(p1.x() / -p1.z(), p1.y() / -p1.z());
      GFX::Point2D projP2(p2.x() / -p2.z(), p2.y() / -p2.z());

      lines.push_back(GFX::Line2D(projP1, projP2, color));
    }
  }
}

