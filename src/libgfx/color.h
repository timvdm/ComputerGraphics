#ifndef GFX_COLOR_H
#define GFX_COLOR_H

#include "types.h"

#include <cassert>
#include <vector>
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
    Color(unsigned char r_ = 0, unsigned char g_ = 0, unsigned char b_ = 0, unsigned char a_ = 0) : r(r_), g(g_), b(b_), a(a_)
    {
    }

    /**
     * @brief Constructor.
     *
     * @param rgb A std::vector containing the RGB components.
     */
    Color(const std::vector<float> &rgb) : r(255 * rgb[0]), g(255 * rgb[1]), b(255 * rgb[2]), a(255)
    {
      assert(rgb.size() == 3);
    }
    Color(const std::vector<double> &rgb) : r(255 * rgb[0]), g(255 * rgb[1]), b(255 * rgb[2]), a(255)
    {
      assert(rgb.size() == 3);
    }


    /**
     * @return The color black.
     */
    static Color black()
    {
      return Color(0, 0, 0);
    }

    /**
     * @return The color black.
     */
    static Color white()
    {
      return Color(255, 255, 255);
    }

    /**
     * @return The color black.
     */
    static Color red()
    {
      return Color(255, 0, 0);
    }

    /**
     * @return The color black.
     */
    static Color green()
    {
      return Color(0, 255, 0);
    }

    /**
     * @return The color black.
     */
    static Color blue()
    {
      return Color(0, 0, 255);
    }

    /**
     * @return The color black.
     */
    static Color yellow()
    {
      return Color(255, 255, 0);
    }

    const unsigned int& toARGB() const
    {
      return reinterpret_cast<const unsigned int&>(*this);
    }

    unsigned char b; //!< The blue component.
    unsigned char g; //!< The green component.
    unsigned char r; //!< The red component.
    unsigned char a; //!< The alpha component
  };

  inline std::ostream& operator<<(std::ostream &os, const Color &color)
  {
    return os << "rgb(" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")";
  }

}

#endif
