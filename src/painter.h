#ifndef CG_XXX_H
#define CG_XXX_H

#include <algorithm>
#include <cmath>
#include <cassert>

namespace CG {

  struct Color
  {
    Color()
    {
    }

    Color(uint8_t red, uint8_t green, uint8_t blue)
      : r(red), g(green), b(blue)
    {
    }

    static Color fromDoubles(double r, double g, double b)
    {
      return Color(r * 255, g * 255, b * 255);
    }
    
    static Color black()  { return Color(  0,   0,   0); }
    static Color white()  { return Color(255, 255, 255); }
    static Color red()    { return Color(255,   0,   0); }
    static Color green()  { return Color(  0, 255,   0); }
    static Color blue()   { return Color(  0,   0, 255); }
    static Color yellow() { return Color(255, 255,   0); }

    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  /**
   * @section canvas Canvas Requirements
   *
   * The following statements must be valid:
   *
   * @code
   * Canvas canvas(1024, 768); // constructor accepting width and height
   * const Color &color = canvas(42, 42); // operator() taking two integers and returning const reference to Color
   * Color &color = canvas(42, 42); // operator() taking two integers and returning reference to Color
   * @endcode
   */
  template<typename Canvas>
  class Painter
  {
    public:
      Painter(unsigned int width, unsigned int height)
        : m_canvas(width, height), m_width(width), m_height(height)
      {
      }

      const Canvas& canvas() const
      {
        return m_canvas;
      }

      void drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, const Color &color = Color::black())
      {
        // 
        // copied from EasyImage.cc
        // Copyright (C) 2011  Daniel van den Akker
        //
        assert(x0 < m_width && y0 < m_height);
        assert(x1 < m_width && y1 < m_height);

        if (x0 == x1) {
          //special case for x0 == x1
          for (unsigned int i = std::min(y0, y1); i <= std::max(y0, y1); i++)
            m_canvas(x0, i) = color;
          return;
        } 
        
        if (y0 == y1) {
          //special case for y0 == y1
          for (unsigned int i = std::min(x0, x1); i <= std::max(x0, x1); i++)
            m_canvas(i, y0) = color;
          return;
        }
        
        if (x0 > x1) {
          //flip points if x1>x0: we want x0 to have the lowest value
          std::swap(x0, x1);
          std::swap(y0, y1);
        }
        
        double m = ((double) y1 - (double) y0) / ((double) x1 - (double) x0);
        if (-1.0 <= m && m <= 1.0) {
          for (unsigned int i = 0; i <= (x1 - x0); i++)
            m_canvas(x0 + i, (unsigned int) round(y0 + m * i)) = color;
        } else if (m > 1.0) {
          for (unsigned int i = 0; i <= (y1 - y0); i++)
            m_canvas((unsigned int) round(x0 + (i / m)), y0 + i) = color;
        } else if (m < -1.0) {
          for (unsigned int i = 0; i <= (y0 - y1); i++)
            m_canvas((unsigned int) round(x0 - (i / m)), y0 - i) = color;
        }
      }

    private:
      Canvas m_canvas;
      unsigned int m_width;
      unsigned int m_height;
  };

}

#endif
