#ifndef GFX_UTILITY_H
#define GFX_UTILITY_H

#include "point2d.h"
#include "types.h"

/**
 * @brief The GFX namespace.
 */
namespace GFX {

  struct SphereCoord;

  /**
   * @brief Round a floating-point value to the nearest integer value.
   *
   * @param value The floating-point value to round.
   *
   * @return The nearest integer value.
   */
  inline int nearest(Real value)
  {
    return (value < 0.0) ? value - 0.5 : value + 0.5;
  }

  /**
   * @brief Interpolate a Z-buffer value for a line.
   *
   * Given the Z-buffer values of two line endpoints, compute the Z-buffer
   * value for an intermediate point on the line.
   *
   * @param zA Z-buffer value for line endpoint A.
   * @param zB Z-buffer value for line endpoint B.
   * @param step The current step when drawing the line from point A to B.
   * @param numSteps The total number of steps needed to draw the line.
   *
   * @return The interpolated Z-buffer value for the intermediate point.
   */
  inline Real interpolateLineZ(Real zA, Real zB, Real step, Real numSteps)
  {
    Real p = step / numSteps;
    return (1.0 - p) / zA + p / zB;
  }

  /**
   * @brief Project a 3D vertex in eye-coordinates to the 2D plane.
   *
   * @param v The vertex to project.
   *
   * @return The 2D coordinates for the projected vertex.
   */
  inline Point2D project(const vec4 &v, Real d)
  {
    return Point2D(-(d * v.x()) / v.z(), -(d * v.y()) / v.z());
  }

  inline Real deg2rad(Real angle)
  {
    return angle * M_PI / 180.0;
  }

  inline std::size_t columnMajorIndex(std::size_t row, std::size_t col, std::size_t cols)
  {
    return row * cols + col;
  }

  inline std::size_t rowMajorIndex(std::size_t row, std::size_t col, std::size_t rows)
  {
    return row + rows * col;
  }



}

#endif
