#ifndef GFX_RENDER2D_H
#define GFX_RENDER2D_H

#include "color.h"

#include <cmath>

namespace GFX {

  template<typename CanvasType>
  class Render2D
  {
    public:
      Render2D(CanvasType &canvas) : m_canvas(canvas)
      {
      }

      /*
      void drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, const Color &color)
      {
        if (x0 == x1) {
          for (unsigned int y = std::min(y0, y1); y <= std::max(y0, y1); ++y)
            m_canvas.setPixel(x0, y, color);
          return;
        }
        
        if (y0 == y1) {
          for (unsigned int x = std::min(x0, x1); x <= std::max(x0, x1); ++x)
            m_canvas.setPixel(x, y0, color);
          return;
        }

        if (x0 > x1) {
          std::swap(x0, x1);
          std::swap(y0, y1);
        }

        int dx = x1 - x0;
        int dy = y1 - y0;
        int d = 2 * dy - dx;
        int incrE = 2 * dy;
        int incrNE = 2 * (dy - dx);
        int x = x0;
        int y = y0;

        m_canvas.setPixel(x, y, color);

        while (x < x1) {
          if (d <= 0) {
            d += incrE;
            ++x;
          } else {
            d += incrNE;
            ++x;
            ++y;
          }
          m_canvas.setPixel(x, y, color);
        }
      }
      */

      void drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, const Color &color)
      {
        if (x0 == x1) {
          for (unsigned int i = std::min(y0, y1); i <= std::max(y0, y1); i++)
            m_canvas.setPixel(x0, i, color);
          return;
        }

        if (y0 == y1) {
          for (unsigned int i = std::min(x0, x1); i <= std::max(x0, x1); i++)
            m_canvas.setPixel(i, y0, color);
          return;
        }

        if (x0 > x1) {
          std::swap(x0, x1);
          std::swap(y0, y1);
        }

        double m = ((double) y1 - (double) y0) / ((double) x1 - (double) x0);

        if (-1.0 <= m && m <= 1.0) {
          for (unsigned int i = 0; i <= (x1 - x0); i++)
            m_canvas.setPixel(x0 + i, (unsigned int) round(y0 + m * i), color);
        } else if (m > 1.0)  {
          for (unsigned int i = 0; i <= (y1 - y0); i++)
            m_canvas.setPixel((unsigned int) round(x0 + (i / m)), y0 + i, color);
        } else if (m < -1.0) {
          for (unsigned int i = 0; i <= (y0 - y1); i++)
            m_canvas.setPixel((unsigned int) round(x0 - (i / m)), y0 - i, color);
        }
      }

      void drawCircle(unsigned int cx, unsigned int cy, unsigned int radius, const Color &color)
      {
        int x = 0;
        int y = radius;
        int d = 1 - radius;
        int deltaE = 3;
        int deltaSE = -2 * radius + 5;

        circlePoints(cx, cy, x, y, color);

        while (y > x) {
          if (d < 0) {
            d += deltaE;
            deltaE += 2;
            deltaSE += 2;
          } else {
            d += deltaSE;
            deltaE += 2;
            deltaSE += 4;
            --y;
          }
          ++x;
          circlePoints(cx, cy, x, y, color);
        }
      }

      void drawEllipse(unsigned int cx, unsigned int cy, unsigned int a, unsigned int b, const Color &color)
      {
        double Pk = b * b - b * a * a + 0.25 * a * a;
        int x = 0;
        int y = b;
        
        ellipsePoints(cx, cy, x, y, color);

        while (2 * x * b * b <= 2 * y * a * a) {
          if (Pk < 0) {
            x++;
            Pk = Pk + 2 * x * b * b + 3 * b * b;
          } else {
            x++;
            y--;
            Pk = Pk + 2 * x * b * b + 3 * b * b - 2 * y * a * a + 2 * a * a;
          }
          ellipsePoints(cx, cy, x, y, color);
        }

        Pk = (x + 0.5) * (x + 0.5) * b * b + (y - 1) * (y - 1) * a * a - a * a * b * b;
        ellipsePoints(cx, cy, x, y, color);
        while (y > 0) {
          if (Pk > 0) {
            y--;
            Pk = Pk - 2 * y * a * a + 3 * a * a;
          } else {
            x++;
            y--;
            Pk = Pk - 2 * y * a * a + 3 * a * a + 2 * x * b * b + 2 * b * b;
          }
          ellipsePoints(cx, cy, x, y, color);
        }

        /*
        int rx2 = rx * rx;
        int ry2 = ry * ry;

        int x = 0;
        int y = ry;
        double d1 = ry2 - rx2 * ry + 0.25 * rx2;

        ellipsePoints(cx, cy, x, y, color);

        // region 1: dx < dy
        while (rx2 * (y - 0.5) > ry2 * (x + 1)) {
          std::cout << "d1 = " << d1 << std::endl;
          if (d1 < 0)
            d1 += ry2 * (2 * x + 3);
          else {
            d1 += ry2 * (2 * x + 3) + rx2 * (-2 * y + 2);
            --y;
          }
          ++x;
          std::cout << "(" << x << ", " << y << ")" << std::endl;
          ellipsePoints(cx, cy, x, y, color);
        }

        // region 2: dy > dx
        double d2 = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;
        while (y > 0) {
          if (d2 < 0) {
            d2 += ry2 * (2 * x + 2) + rx2 * (-2 * y + 3);
            ++x;
          } else
            d2 += rx2 * (-2 * y + 3);
          --y;
          ellipsePoints(cx, cy, x, y, color);
        }
        */
      }


    private:
      void circlePoints(unsigned int cx, unsigned int cy, unsigned int x, unsigned int y, const Color &color)
      {
        m_canvas.setPixel(cx + x, cy + y, color);
        m_canvas.setPixel(cx + y, cy + x, color);
        m_canvas.setPixel(cx - x, cy + y, color);
        m_canvas.setPixel(cx - y, cy + x, color);
        m_canvas.setPixel(cx - x, cy - y, color);
        m_canvas.setPixel(cx - y, cy - x, color);
        m_canvas.setPixel(cx + x, cy - y, color);
        m_canvas.setPixel(cx + y, cy - x, color);
      }
      
      void ellipsePoints(unsigned int cx, unsigned int cy, unsigned int x, unsigned int y, const Color &color)
      {
        m_canvas.setPixel(cx + x, cy + y, color);
        //m_canvas.setPixel(cx + x, cy - y, color);
        //m_canvas.setPixel(cx - x, cy + y, color);
        //m_canvas.setPixel(cx - x, cy - y, color);
      }

      CanvasType &m_canvas;
  };

}

#endif
