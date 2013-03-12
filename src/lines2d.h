#ifndef INC_DATASTRUCTS_H
#define INC_DATASTRUCTS_H

#include "utils/EasyImage.h"

#include <vector>
#include <cassert>

namespace CG {

  /**
   * @brief Type to represent real numbers.
   */
  typedef double Real;

  /**
   * @brief Class representing a RGB color. The RGB comonents are in the range [0,1].
   */
  struct Color
  {
    /**
     * @brief Constructor.
     *
     * @param r_ The red component.
     * @param g_ The green component.
     * @param b_ The blue component.
     */
    Color(Real r_ = 0.0, Real g_ = 0.0, Real b_ = 0.0) : r(r_), g(g_), b(b_)
    {
    }

    /**
     * @brief Constructor.
     *
     * @param rgb A std::vector containing the RGB components.
     */
    Color(const std::vector<Real> &rgb) : r(rgb[0]), g(rgb[1]), b(rgb[2])
    {
      assert(rgb.size() == 3);
    }

    Color(const img::Color &color)
      : r(color.red / 255.0), g(color.green / 255.0), b(color.blue / 255.0)
    {
    }

    /**
     * @return The color black.
     */
    static Color black()
    {
      return Color(0.0, 0.0, 0.0);
    }

    /**
     * @return The color black.
     */
    static Color white()
    {
      return Color(1.0, 1.0, 1.0);
    }

    /**
     * @return The color black.
     */
    static Color red()
    {
      return Color(1.0, 0.0, 0.0);
    }

    /**
     * @return The color black.
     */
    static Color green()
    {
      return Color(0.0, 1.0, 0.0);
    }

    /**
     * @return The color black.
     */
    static Color blue()
    {
      return Color(0.0, 0.0, 1.0);
    }

    /**
     * @return The color black.
     */
    static Color yellow()
    {
      return Color(1.0, 1.0, 0.0);
    }

    /**
     * @brief Conversion operator to an img::Color. The RGB components are also
     * rescaled to img::Color's [0,255] range.
     *
     * @return The converted color.
     */
    operator img::Color() const
    {
      return img::Color(r * 255, g * 255, b * 255);
    }

    Real r; //!< The red component.
    Real g; //!< The green component.
    Real b; //!< The blue component.
  };

  /**
   * @brief A set of RGB colors.
   */
  typedef std::vector<Color> Colors;

  /**
   * @brief Class representing a point in 2D.
   */
  struct Point2D
  {
    /**
     * @brief Default constructor.
     *
     * Initializes the point to (0,0).
     */
    Point2D() : x(0.0), y(0.0)
    {
    }

    /**
     * @brief Constructor.
     *
     * @param x_ The point's x coorinate.
     * @param y_ The point's y coorinate.
     */
    Point2D(Real x_, Real y_) : x(x_), y(y_)
    {
    }

    Real x; //!< The point's x coordinate.
    Real y; //!< The point's y coordinate.
  };

  /**
   * @brief A set of 2D points.
   */
  typedef std::vector<Point2D> Points2D;

  /**
   * @brief Class representing a colored line in 2D.
   */
  struct Line2D
  {
    /**
     * @brief Constructor.
     *
     * @param p1_ The starting point of the line.
     * @param p2_ The end point of the line.
     * @param color_ The line's color.
     */
    Line2D(const Point2D &p1_, const Point2D &p2_, const Color &color_ = Color::black())
        : p1(p1_), p2(p2_), color(color_)
    {
    }

    /**
     * @brief Constructor.
     *
     * @param x1 The x coordinate for the line's starting point.
     * @param y1 The y coordinate for the line's starting point.
     * @param x2 The x coordinate for the line's end point.
     * @param y2 The y coordinate for the line's end point.
     */
    Line2D(Real x1, Real y1, Real x2, Real y2, const Color &color_ = Color::black())
        : p1(x1, y1), p2(x2, y2), color(color_)
    {
    }

    Point2D p1; //!< The line's starting point.
    Point2D p2; //!< The line's end point.
    Color color; //!< The line's color.
  };

  /**
   * @brief A set of 2D lines.
   */
  typedef std::vector<Line2D> Lines2D;

  /**
   * @brief Find the min. and max. paints for a set of lines.
   *
   * The returned std::pair contains two Point2D objects representing the lower-left
   * and upper-right corners of the smallest rectangle containing all lines.
   *
   * @param lines The set of lines.
   *
   * @return The min. and max. points for the specified set of lines.
   */
  std::pair<Point2D, Point2D> get_min_max(const Lines2D &lines);

  /**
   * @brief Compute the size for an image given the min. and max. points.
   *
   * @param minMax The min. and max. points (e.g. result of get_min_max()).
   * @param size The size for the largest range.
   *
   * @return A std::pair containing the size along the x and y axis.
   */
  std::pair<int, int> get_image_sizes(const std::pair<Point2D, Point2D> &minMax, int size);

  /**
   * @brief Get the scaling factor for the image.
   *
   * @param minMax The min. and max. coordinate for the drawing (e.g. result of get_min_max()).
   * @param xSize The size of the image along the x axis.
   * @param border A scaling factor applied to the image to ensure there is some padding along the edges.
   *
   * @return The scaling factor d.
   */
  Real get_scale_factor(const std::pair<Point2D, Point2D> &minMax, int xSize, Real border = 0.95);

  /**
   * @brief Get the center for a set of lines.
   *
   * @param minMax The min. and max. coordinate for the drawing (e.g. result of get_min_max()).
   * @param d The scaling factor for the image.
   *
   * @return The center for the set of lines.
   */
  Point2D get_center(const std::pair<Point2D, Point2D> &minMax, Real d);

  /**
   * @brief Apply a scaling factor to a set of lines.
   *
   * @param lines The set of lines.
   * @param d The scaling factor for the image.
   */
  void scale_lines(Lines2D &lines, Real d);

  /**
   * @brief Apply a translation to a set of lines to ensure the image is centered
   * inside the image when drawing.
   *
   * @param lines The set of lines.
   * @param imageSizes The x and y size of the final image.
   * @param center The current center of the lines.
   */
  void center_lines(Lines2D &lines, const std::pair<int, int> &imageSizes, const Point2D &center);

  /**
   * @brief Draw a set of lines in an EasyImage.
   *
   * @param lines The lines to draw.
   * @param size The maximal size in the x or y direction for the image.
   * @param bgColor The background color for the image.
   *
   * @return The EasyImage with the drawing.
   */
  img::EasyImage draw_lines(Lines2D &lines, int size, const img::Color &bgColor);

}

#endif
