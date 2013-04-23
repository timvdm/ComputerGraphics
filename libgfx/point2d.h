#ifndef GFX_POINT2D_H
#define GFX_POINT2D_H

#include "types.h"

#include <cmath>

#include <iostream>

namespace GFX {

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

    static Real distance(const Point2D &A, const Point2D &B)
    {
      Real dx = A.x - B.x;
      Real dy = A.y - B.y;
      return std::sqrt(dx * dx + dy * dy);
    }

    Real x; //!< The point's x coordinate.
    Real y; //!< The point's y coordinate.
  };

  inline std::ostream& operator<<(std::ostream &os, const Point2D &p)
  {
    return os << "(" << p.x << ", " << p.y << ")";
  }

}

#endif
