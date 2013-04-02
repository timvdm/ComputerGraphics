#include "render.h"

#include "../libgfx/mesh.h"
#include "../libgfx/buffer.h"
#include "../libgfx/utility.h"

#include <limits>
#include <algorithm>

using namespace GFX;

template<typename LinesXD>
std::pair<Point2D, Point2D> get_min_max(const LinesXD &lines)
{
  Real xMin = std::numeric_limits<Real>::max();
  Real xMax = std::numeric_limits<Real>::min();
  Real yMin = std::numeric_limits<Real>::max();
  Real yMax = std::numeric_limits<Real>::min();

  for (typename LinesXD::const_iterator line = lines.begin(); line != lines.end(); ++line) {
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

template std::pair<Point2D, Point2D> get_min_max<Lines2D>(const Lines2D&);
template std::pair<Point2D, Point2D> get_min_max<Lines3D>(const Lines3D&);

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

template<typename LinesXD>
void scale_lines(LinesXD &lines, Real d)
{
  for (typename LinesXD::iterator line = lines.begin(); line != lines.end(); ++line) {
    line->p1.x *= d;
    line->p1.y *= d;
    line->p2.x *= d;
    line->p2.y *= d;
  }
}

template void scale_lines<Lines2D>(Lines2D&, Real);
template void scale_lines<Lines3D>(Lines3D&, Real);

Point2D get_center(const std::pair<Point2D, Point2D> &minMax, Real d)
{
  Real xDC = d * (minMax.first.x + minMax.second.x) / 2;
  Real yDC = d * (minMax.first.y + minMax.second.y) / 2;

  return Point2D(xDC, yDC);
}

template<typename LinesXD>
void center_lines(LinesXD &lines, const std::pair<int, int> &imageSizes, const Point2D &center)
{
  Real xAdd = imageSizes.first / 2.0 - center.x;
  Real yAdd = imageSizes.second / 2.0 - center.y;

  for (typename LinesXD::iterator line = lines.begin(); line != lines.end(); ++line) {
    line->p1.x += xAdd;
    line->p1.y += yAdd;
    line->p2.x += xAdd;
    line->p2.y += yAdd;
  }
}

template void center_lines<Lines2D>(Lines2D&, const std::pair<int, int>&, const Point2D&);
template void center_lines<Lines3D>(Lines3D&, const std::pair<int, int>&, const Point2D&);

img::EasyImage draw_lines(Lines2D &lines, int size, const img::Color &bgColor)
{
  // compute some properties for the lines
  std::pair<Point2D, Point2D> minMax = get_min_max(lines);
  std::pair<int, int> imageSizes = get_image_sizes(minMax, size);
  Real d = get_scale_factor(minMax, imageSizes.first);
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

struct Ctx 
{
  Ctx(int width, int height, const img::Color &bgColor) : image(width, height, bgColor), zBuffer(width, height)
  {
    zBuffer.clear(std::numeric_limits<Real>::max());
  }

  void drawPixel(int x, int y, Real z, const Color &color)
  {
    if (z < zBuffer(x, y)) {
      image(x, y) = img::Color(color.r, color.g, color.b);
      zBuffer(x, y) = z;
    }
  }

  img::EasyImage image;
  GFX::Buffer<Real> zBuffer;
};

void draw_pixel(Ctx &ctx, int x, int y, Real z0, Real z1, Real i, Real a, const Color &color)
{
  Real p = i / a;
  Real z = p / z1 + (1.0 - p) / z0;
  ctx.drawPixel(x, y, z, color);
}

void draw_zbuf_line(Ctx &ctx, const Point3D &p1, const Point3D &p2, const Color &color)
{
  if (p1.x == p2.x) {
    // special case for p1.x == p2.x
    int minY = nearest(std::min(p1.y, p2.y));
    int maxY = nearest(std::max(p1.y, p2.y));
    int numY = maxY - minY;

    for (int i = minY; i <= maxY; ++i)
      draw_pixel(ctx, p1.x, i, p1.z, p2.z, i, numY, color);
    return;
  } 

  if (p1.y == p2.y) {
    // special case for p1.y == p2.y
    int minX = nearest(std::min(p1.x, p2.x));
    int maxX = nearest(std::max(p1.x, p2.x));
    int numX = maxX - minX;

    for (int i = minX; i <= maxX; ++i)
      draw_pixel(ctx, i, p1.y, p1.z, p2.z, i, numX, color);
    return;
  }

  int x0 = p1.x;
  int y0 = p1.y;
  Real z0 = p1.z;
  int x1 = p2.x;
  int y1 = p2.y;
  Real z1 = p2.z;

  if (p1.x > p2.x) {
    // flip points if p2.x > p1.x: we want p1.x to have the lowest value
    std::swap(x0, x1);
    std::swap(y0, y1);
    //std::swap(z0, z1);
  }

  Real m = ((Real) y1 - (Real) y0) / ((Real) x1 - (Real) x0);
  if (-1.0 <= m && m <= 1.0) {
    int num = x1 - x0;
    for (int i = 0; i <= (x1 - x0); ++i) {
      int x = x0 + i;
      int y = round(y0 + m * i);
      draw_pixel(ctx, x, y, z0, z1, i, num, color);
    }
  } else if (m > 1.0) {
    int num = y1 - y0;
    for (int i = 0; i <= (y1 - y0); ++i) {
      int x = round(x0 + i / m);
      int y = y0 + i;
      draw_pixel(ctx, x, y, z0, z1, i, num, color);
    }
  } else if (m < -1.0) {
    int num = y0 - y1;
    for (int i = 0; i <= (y0 - y1); ++i) {
      int x = round(x0 - i / m);
      int y = y0 - i;
      draw_pixel(ctx, x, y, z0, z1, i, num, color);
    }
  }
}


img::EasyImage draw_zbuffered_lines(GFX::Lines3D &lines, int size, const img::Color &bgColor)
{
  // compute some properties for the lines
  std::pair<Point2D, Point2D> minMax = get_min_max(lines);
  std::pair<int, int> imageSizes = get_image_sizes(minMax, size);
  Real d = get_scale_factor(minMax, imageSizes.first);
  Point2D center = get_center(minMax, d);

  // scale the lines
  scale_lines(lines, d);
  // center the lines
  center_lines(lines, imageSizes, center);

  Ctx ctx(imageSizes.first, imageSizes.second, bgColor);
  
  // draw the lines
  for (std::size_t i = 0; i < lines.size(); ++i) {
    //std::cout << lines[i].p1 << " -> " << lines[i].p2 << std::endl;
    draw_zbuf_line(ctx, lines[i].p1, lines[i].p2, lines[i].color);
  }

  return ctx.image;
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

void renderMesh(const GFX::Mesh &mesh, const GFX::Color &color, const GFX::mat4 &T, GFX::Lines3D &lines)
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

      GFX::Point3D projP1(p1.x() / -p1.z(), p1.y() / -p1.z(), p1.z());
      GFX::Point3D projP2(p2.x() / -p2.z(), p2.y() / -p2.z(), p2.z());
      
      //std::cout << "z1 = " << projP1.z << ", z2 = " << projP2.z << std::endl;

      lines.push_back(GFX::Line3D(projP1, projP2, color));
    }
  }
}
