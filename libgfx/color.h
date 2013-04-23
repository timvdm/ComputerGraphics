#ifndef GFX_COLOR_H
#define GFX_COLOR_H

#include "types.h"

#include <cassert>
#include <vector>
#include <iostream>

namespace GFX {

  struct ColorF;

  /**
   * @brief Class representing a RGB color. The RGB comonents are in the range [0,255].
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
    Color(unsigned char r_ = 0, unsigned char g_ = 0, unsigned char b_ = 0, unsigned char a_ = 255) : r(r_), g(g_), b(b_), a(a_)
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
     * @return No color, alpha transparancy set to 0.
     */
    static Color none()
    {
      return Color(0, 0, 0, 0);
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

    inline operator ColorF() const;

    unsigned char b; //!< The blue component.
    unsigned char g; //!< The green component.
    unsigned char r; //!< The red component.
    unsigned char a; //!< The alpha component
  };

  /**
   * @brief Class representing a RGB color with floating points. The RGB comonents are in the range [0,1].
   */
  struct ColorF
  {
    /**
     * @brief Constructor.
     *
     * @param r_ The red component.
     * @param g_ The green component.
     * @param b_ The blue component.
     */
    ColorF(double r_ = 0, double g_ = 0, double b_ = 0, double a_ = 1.0) : r(r_), g(g_), b(b_), a(a_)
    {
    }

    operator Color() const
    {
      return Color(std::min(255 * r, 255.0), std::min(255 * g, 255.0), std::min(255 * b, 255.0), std::min(255 * a, 255.0));
    }

    void clamp()
    {
      if (r > 1.0)
        r = 1.0;
      if (g > 1.0)
        g = 1.0;
      if (b > 1.0)
        b = 1.0;
      if (a > 1.0)
        a = 1.0;
    }

    double r; //!< The red component.
    double g; //!< The green component.
    double b; //!< The blue component.
    double a; //!< The alpha component
  };
    
  Color::operator ColorF() const
  {
    double f = 1.0 / 255.0;
    return ColorF(r * f, g * f, b * f, a * f);
  }

  inline std::ostream& operator<<(std::ostream &os, const Color &color)
  {
    return os << "rgb(" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")";
  }

  inline std::ostream& operator<<(std::ostream &os, const ColorF &color)
  {
    return os << "rgb(" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")";
  }

}

#endif
