#ifndef GFX_COLOR_H
#define GFX_COLOR_H

#include "types.h"

#include <cassert>

#include <iostream>

namespace GFX {

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

    /*
    Color(const img::Color &color)
      : r(color.red / 255.0), g(color.green / 255.0), b(color.blue / 255.0)
    {
    }
    */

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
    /*
    operator img::Color() const
    {
      return img::Color(r * 255, g * 255, b * 255);
    }
    */

    Real r; //!< The red component.
    Real g; //!< The green component.
    Real b; //!< The blue component.
  };

  inline std::ostream& operator<<(std::ostream &os, const Color &color)
  {
    return os << "rgb(" << color.r << ", " << color.g << ", " << color.b << ")";
  }

}

#endif
