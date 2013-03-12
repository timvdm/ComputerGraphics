#ifndef GFX_CONTEXT_H
#define GFX_CONTEXT_H

#include "buffer.h"
#include "color.h"

#include <iostream>

namespace GFX {

  class Context
  {
    public:
      enum EnableFlags
      {
        GFX_ZBUFFER = 1
      };

      Context(int width, int height) : m_colorBuffer(width, height),
          m_zBuffer(width, height), m_width(width), m_height(height),
          m_enabled(GFX_ZBUFFER)
      {
        m_near = 1.0;
      }

      int width() const
      {
        return m_width;
      }

      int height() const
      {
        return m_height;
      }

      double near() const
      {
        return m_near;
      }

      void setNear(double near)
      {
        m_near = near;
      }

      bool isEnabled(EnableFlags flag) const
      {
        return m_enabled & flag;
      }

      void enable(EnableFlags flag)
      {
        m_enabled = static_cast<EnableFlags>(m_enabled | flag);
      }

      void disable(EnableFlags flag)
      {
        m_enabled = static_cast<EnableFlags>(m_enabled & ~flag);
      }

      const Buffer<Color>& colorBuffer() const
      {
        return m_colorBuffer;
      }

      Buffer<Color>& colorBuffer()
      {
        return m_colorBuffer;
      }

      const Buffer<double>& zBuffer() const
      {
        return m_zBuffer;
      }

      Buffer<double>& zBuffer()
      {
        return m_zBuffer;
      }

      void drawPixel(int x, int y, double z, const Color &color)
      {
        //std::cout << "Context::drawPixel()" << std::endl;

        // ensure the pixel is within the color buffer
        if (x < 0 || x >= m_colorBuffer.width() || y < 0 || y > m_colorBuffer.height())
          return;

        // check z-buffer if it is enabled
        if (isEnabled(GFX_ZBUFFER)) {
          // if the Z-buffer value is greater, don't draw the pixel
          if (z > m_zBuffer(x, y))
            return;
          // update Z-buffer value
          m_zBuffer(x, y) = z;
        }

        //std::cout << "    drawing pixel @ (" << x << ", " << y << ")" << std::endl;

        // draw the pixel
        m_colorBuffer(x, y) = color;
      }

    private:
      Buffer<Color> m_colorBuffer;
      Buffer<double> m_zBuffer;
      double m_near;
      int m_width;
      int m_height;
      EnableFlags m_enabled;
  };

}

#endif
