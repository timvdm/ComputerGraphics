#ifndef GFX_LINE3D_H
#define GFX_LINE3D_H

#include "types.h"
#include "color.h"

#include <vector>

namespace GFX {


  /**
   * @brief Class representing a colored line in 3D.
   */
  struct Line3D
  {
    /**
     * @brief Constructor.
     *
     * @param p1_ The starting point of the line.
     * @param p2_ The end point of the line.
     * @param color_ The line's color.
     */
    Line3D(const vec3 &p1_, const vec3 &p2_, const Color &color_ = Color::black())
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
    Line3D(Real x1, Real y1, Real z1, Real x2, Real y2, Real z2, 
        const Color &color_ = Color::black())
        : p1(x1, y1, z1), p2(x2, y2, z2), color(color_)
    {
    }

    vec3 p1; //!< The line's starting point.
    vec3 p2; //!< The line's end point.
    Color color; //!< The line's color.
  };

  /**
   * @brief A set of 3D lines.
   */
  typedef std::vector<Line3D> Lines3D;

}

#endif
