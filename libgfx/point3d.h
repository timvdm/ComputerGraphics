#ifndef GFX_POINT3D_H
#define GFX_POINT3D_H

#include "types.h"

#include <cmath>

#include <iostream>

namespace GFX {

  /**
   * @brief Class representing a point in 3D.
   */
  struct Point3D
  {
    /**
     * @brief Default constructor.
     *
     * Initializes the point to (0,0).
     */
    Point3D() : x(0.0), y(0.0), z(0.0)
    {
    }

    /**
     * @brief Constructor.
     *
     * @param x_ The point's x coorinate.
     * @param y_ The point's y coorinate.
     * @param z_ The point's z coorinate.
     */
    Point3D(Real x_, Real y_, Real z_) : x(x_), y(y_), z(z_)
    {
    }

    static Real distance(const Point3D &A, const Point3D &B)
    {
      Real dx = A.x - B.x;
      Real dy = A.y - B.y;
      return std::sqrt(dx * dx + dy * dy);
    }

    Real x; //!< The point's x coordinate.
    Real y; //!< The point's y coordinate.
    Real z; //!< The point's z coordinate.
  };

  inline std::ostream& operator<<(std::ostream &os, const Point3D &p)
  {
    return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
  }

}

#endif
