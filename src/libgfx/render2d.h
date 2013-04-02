#ifndef GFX_RENDER2D_H
#define GFX_RENDER2D_H

#include "color.h"
#include "utility.h"

#include <cmath>

namespace GFX {

  template<typename CanvasType>
  class Render2D
  {
    public:
      Render2D(CanvasType &canvas) : m_canvas(canvas)
      {
      }

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

        Real m = ((Real) y1 - (Real) y0) / ((Real) x1 - (Real) x0);

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

      void drawEllipse(unsigned int cx, unsigned int cy, unsigned int rx, unsigned int ry, const Color &color)
      {
        Real rx2 = rx * rx;
        Real ry2 = ry * ry;

        int x = 0;
        int y = ry;
        Real d1 = ry2 - rx2 * ry + 0.25 * rx2;

        ellipsePoints(cx, cy, x, y, color);

        // region 1: dx < dy
        while (rx2 * (y - 0.5) > ry2 * (x + 1)) {
          if (d1 < 0)
            d1 += ry2 * (2 * x + 3);
          else {
            d1 += ry2 * (2 * x + 3) + rx2 * (-2 * y + 2);
            --y;
          }
          ++x;
          ellipsePoints(cx, cy, x, y, color);
        }

        // region 2: dy > dx
        Real d2 = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;
        while (y > 0) {
          if (d2 < 0) {
            d2 += ry2 * (2 * x + 2) + rx2 * (-2 * y + 3);
            ++x;
          } else
            d2 += rx2 * (-2 * y + 3);
          --y;
          ellipsePoints(cx, cy, x, y, color);
        }
      }

      /**
       * @brief Draw a Bezier curve.
       *
       * The curve is drawn from endpoint P0 to endpoint P3 and has control
       * points P1 and P2.
       */
      void drawBezierCurve(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1,
                           unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3,
                           const Color &color)
      {
        /*
        if (std::abs(x0 - x3) <= 5 && std::abs(y0 - y3) <= 5) {
          m_canvas.setPixel(x0, y0, color);
          return;
        }
        */

        /*
        Real dist2 = (x3 - x0) * (x3 - x0) + (y3 - y0) * (y3 - y0);
        std::cout << "dist2 = " << dist2 << std::endl;
        if (dist2 < 20.0) {
          drawLine(x0, y0, x3, y3, color);
          return;
        }
        */

        // midpoint P0-P1
        Real xA = (x0 + x1) / 2.0; 
        Real yA = (y0 + y1) / 2.0;
        // midpoint P2-P3
        Real xB = (x2 + x3) / 2.0; 
        Real yB = (y2 + y3) / 2.0;
        // midpoint P1-P2
        Real xC = (x1 + x2) / 2.0; 
        Real yC = (y1 + y2) / 2.0;
        // midpoint A-C
        Real xA1 = (xA + xC) / 2.0; 
        Real yA1 = (yA + yC) / 2.0;
        // midpoint B-C
        Real xB1 = (xB + xC) / 2.0; 
        Real yB1 = (yB + yC) / 2.0;
        // midpoint A1-B1
        Real xC1 = (xA1 + xB1) / 2.0; 
        Real yC1 = (yA1 + yB1) / 2.0;

        if (std::abs(x0 - xC1) <= 5 && std::abs(y0 - yC1) <= 5)
          drawLine(x0, y0, xC1, yC1, color);
        else
          drawBezierCurve(x0, y0, xA, yA, xA1, yA1, xC1, yC1, color);
        if (std::abs(x3 - xC1) <= 5 && std::abs(y3 - yC1) <= 5)
          drawLine(x3, y3, xC1, yC1, color);
        else
          drawBezierCurve(xC1, yC1, xB1, yB1, xB, yB, x3, y3, color);
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
        m_canvas.setPixel(cx + x, cy - y, color);
        m_canvas.setPixel(cx - x, cy + y, color);
        m_canvas.setPixel(cx - x, cy - y, color);
      }

      CanvasType &m_canvas;
  };

}

#endif
