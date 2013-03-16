#ifndef GFX_RENDERER_H
#define GFX_RENDERER_H

#include "utility.h"
#include "context.h"

#include <limits>
#include <tuple>

namespace GFX {

  /**
   * @brief Compute triangle area given three 2D coordinates.
   *
   * @param A The 2D coordinates for the first vertex.
   * @param B The 2D coordinates for the second vertex.
   * @param C The 2D coordinates for the last vertex.
   *
   * @return The interpolated color.
   */
  double triangleArea(const Point2D &A, const Point2D &B, const Point2D &C)
  {
    // shoelace or surveyor's formula
    return 0.5 * std::abs((A.x - C.x) * (B.y - A.y) - (A.x - B.x) * (C.y - A.y));
  }

  Color interpolateVarying(const Color &cA, const Color &cB, const Point2D &A, const Point2D &B, const Point2D &I)
  {
    double a = Point2D::distance(B, I) / Point2D::distance(A, B);
    double b = 1.0 - a;
    return Color(cA.r * a + cB.r * b, cA.g * a + cB.g * b, cA.b * a + cB.b * b);
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I == sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...>&, const std::tuple<Tp...>&, const Point2D &,
      const Point2D &, const Point2D &, std::tuple<Tp...>&)
  {
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I < sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...> &varyingA, const std::tuple<Tp...> &varyingB, 
      const Point2D &A, const Point2D &B, const Point2D &C, std::tuple<Tp...> &varying)
  {
    std::get<I>(varying) = interpolateVarying(std::get<I>(varyingA), std::get<I>(varyingB), A, B, C);
    callInterpolationFunction<I + 1, Tp...>(varyingA, varyingB, A, B, C, varying);
  }


  /**
   * @brief Interpolate a Color across a triangle using barycentric interpolation.
   *
   * The barycentric interpolation uses the area of the triangles ABC, IAB, IAC
   * and IBC to compute the percentages of each color.
   *
   * @param cA The color for vertex A.
   * @param cB The color for vertex B.
   * @param cC The color for vertex C.
   * @param A The 2D projected coordinates for vertex A.
   * @param B The 2D projected coordinates for vertex B.
   * @param C The 2D projected coordinates for vertex C.
   * @param I The 2D projected coordinates for which to interpolate the color.
   *
   * @return The interpolated color.
   */
  Color interpolateVarying(const Color &cA, const Color &cB, const Color &cC,
      const Point2D &A, const Point2D &B, const Point2D &C, const Point2D &I)
  {
    double total = triangleArea(A, B, C);
    double a = triangleArea(I, B, C) / total;
    double b = triangleArea(I, A, C) / total;
    double c = triangleArea(I, A, B) / total;
    return Color(cA.r * a + cB.r * b + cC.r * c,
                 cA.g * a + cB.g * b + cC.g * c,
                 cA.b * a + cB.b * b + cC.b * c);
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I == sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...>&, const std::tuple<Tp...>&, const std::tuple<Tp...>&,
      const Point2D&, const Point2D&, const Point2D&, const Point2D&, std::tuple<Tp...>&)
  {
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I < sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...> &varyingA, const std::tuple<Tp...> &varyingB, const std::tuple<Tp...> &varyingC,
      const Point2D &A, const Point2D &B, const Point2D &C, const Point2D &E, std::tuple<Tp...> &varying)
  {
    std::get<I>(varying) = interpolateVarying(std::get<I>(varyingA), std::get<I>(varyingB), std::get<I>(varyingC), A, B, C, E);
    callInterpolationFunction<I + 1, Tp...>(varyingA, varyingB, varyingC, A, B, C, E, varying);
  }

  template<typename ProgramType>
  class Renderer
  {
    public:
      typedef typename ProgramType::varying_type varying_type;

      Renderer(Context &context, ProgramType &program) : m_context(context), m_program(program)
      {
      }

      const ProgramType& program() const
      {
        return m_program;
      }

      ProgramType& program()
      {
        return m_program;
      }

      void screenCoordinates(vec4 &v)
      {
        double x = 0;
        double y = 0;
        double w = m_context.width() / 2.0;
        double h = m_context.height() / 2.0;
        double n = 0;
        double f = 1;

        v.x() = w * v.x() + (x + w);
        v.y() = h * v.y() + (y + h);
        v.z() = ((f - n) / 2.0) * v.z() + (f + n) / 2.0;
      }

      void screenCoordinates(Point2D &p)
      {
        double x = 0;
        double y = 0;
        double w = m_context.width() / 2.0;
        double h = m_context.height() / 2.0;

        p.x = w * p.x + (x + w);
        p.y = h * p.y + (y + h);
      }

      void drawLines(const double *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 2)) == 0);

        for (std::size_t i = stride; i < size; i += stride * 2)
          drawLine(attributes + i - stride, attributes + i);
      }

      void drawLine(const double *attributesA, const double *attributesB)
      {
        varying_type varyingA;
        varying_type varyingB;

        // execute vertex shader
        vec4 A = m_program.vertexShader().exec(attributesA, varyingA);
        vec4 B = m_program.vertexShader().exec(attributesB, varyingB);

        std::cout << "Line: " << std::endl;
        std::cout << "    clip:      " << print(A) << " -> " << print(B) << std::endl;

        // perspective divide
        A /= A.w();
        B /= B.w();

        std::cout << "    NDC:       " << print(A) << " -> " << print(B) << std::endl;


        //screenCoordinates(A);
        //screenCoordinates(B);
        //std::cout << "    screen:    " << A << " -> " << B << std::endl;


        // project eye-coordinates to 2D surface
        Point2D a = project(A, m_context.near());
        Point2D b = project(B, m_context.near());

        std::cout << "    projected: " << a << " -> " << b << std::endl;

        screenCoordinates(a);
        screenCoordinates(b);
        std::cout << "    screen:    " << a << " -> " << b << std::endl;

        //std::cout << "    Color: " << std::get<0>(varyingA) << " -> " << std::get<0>(varyingB) << std::endl;

        //a = Point2D(-a.x, -a.y);
        //b = Point2D(-b.x, -b.y);

        // primitive assembly
        //if ((a.x < 0 || a.x >= m_context.width() || a.y < 0 || a.y >= m_context.height()) && 
        //    (b.x < 0 || b.x >= m_context.width() || b.y < 0 || b.y >= m_context.height()))
        //  return;
          
        varying_type varying;

        if (a.x == b.x) {
          // special case for a.x == b.x
          int minY = nearest(std::min(a.y, b.y));
          int maxY = nearest(std::max(a.y, b.y));
          int numY = maxY - minY + 1;

          if (minY < 0)
            minY = 0;
          if (maxY > m_context.height() - 1)
            maxY = m_context.height() - 1;

          for (int i = minY; i <= maxY; ++i) {
            callInterpolationFunction(varyingA, varyingB, a, b, Point2D(a.x, i), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(a.x, i, interpolateLineZ(A.z(), B.z(), i, numY), color);
          }
          return;
        } 
        
        if (a.y == b.y) {
          // special case for a.y == b.y
          int minX = nearest(std::min(a.x, b.x));
          int maxX = nearest(std::max(a.x, b.x));
          int numX = maxX - minX + 1;

          if (minX < 0)
            minX = 0;
          if (maxX > m_context.width() - 1)
            maxX = m_context.width() - 1;

          for (int i = minX; i <= maxX; ++i) {
            callInterpolationFunction(varyingA, varyingB, a, b, Point2D(i, a.y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(i, a.y, interpolateLineZ(A.z(), B.z(), i, numX), color);
          }
          return;
        }
        
        if (a.x > b.x) {
          // flip points if b.x > a.x: we want a.x to have the lowest value
          std::swap(a, b);
          std::swap(varyingA, varyingB);
        }
        
        double m = ((double) b.y - (double) a.y) / ((double) b.x - (double) a.x);
        if (-1.0 <= m && m <= 1.0) {
          int num = b.x - a.x + 1;
          for (int i = 0; i <= (b.x - a.x); ++i) {
            int x = a.x + i;
            int y = round(a.y + m * i);

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            callInterpolationFunction(varyingA, varyingB, a, b, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(x, y, interpolateLineZ(A.z(), B.z(), i, num), color);
          }
        } else if (m > 1.0) {
          int num = b.y - a.y + 1;
          for (int i = 0; i <= (b.y - a.y); ++i) {
            int x = round(a.x + i / m);
            int y = a.y + i;

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            callInterpolationFunction(varyingA, varyingB, a, b, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(x, y, interpolateLineZ(A.z(), B.z(), i, num), color);
          }
        } else if (m < -1.0) {
          int num = a.y - b.y + 1;
          for (int i = 0; i <= (a.y - b.y); ++i) {
            int x = round(a.x - i / m);
            int y = a.y - i;

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            callInterpolationFunction(varyingA, varyingB, a, b, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(x, y, interpolateLineZ(A.z(), B.z(), i, num), color);
          }
        }
      }

      void drawTriangles(const double *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 3)) == 0);

        for (std::size_t i = stride * 2; i < size; i += stride * 3)
          drawTriangle(attributes + i - stride * 2, attributes + i - stride, attributes + i);
      }

      bool intersects(double y, const Point2D &P, const Point2D &Q)
      {
        return (P.y != Q.y) && ((y - P.y) * (y - Q.y) <= 0.0);
      }

      double intersection(double y, const Point2D &P, const Point2D &Q)
      {
        return Q.x + (P.x - Q.x) * (y - Q.y) / (P.y - Q.y);
      }

      void drawTriangle(const double *attributesA, const double *attributesB, const double *attributesC)
      {
        varying_type varyingA;
        varying_type varyingB;
        varying_type varyingC;

        // execute vertex shader
        vec4 A = m_program.vertexShader().exec(attributesA, varyingA);
        vec4 B = m_program.vertexShader().exec(attributesB, varyingB);
        vec4 C = m_program.vertexShader().exec(attributesC, varyingC);

        // perspective divide
        A /= A.w();
        B /= B.w();
        C /= C.w();

        // project eye-coordinates to 2D surface
        Point2D a = project(A, m_context.near());
        Point2D b = project(B, m_context.near());
        Point2D c = project(C, m_context.near());

        screenCoordinates(a);
        screenCoordinates(b);
        screenCoordinates(c);

        
        Point2D G((a.x + b.x + c.x) / 3.0, (a.y + b.y + c.y) / 3.0);
        double zG = 1.0 / (3.0 * A.z()) + 1.0 / (3.0 * B.z()) + 1.0 / (3.0 * C.z());

        vec4 u = B - A;
        vec4 v = C - A;
        vec3 w = vec3(u.x(), u.y(), u.z()).cross(vec3(v.x(), v.y(), v.z()));
        double k = w.dot(vec3(A.x(), A.y(), A.z()));

        double dzdx = w.x() / -k;
        double dzdy = w.y() / -k;


        std::cout << "Triangle: " << a << " -> " << b << " -> " << c << std::endl;

        // primitive assembly
        if ((a.x < 0 || a.x >= m_context.width() || a.y < 0 || a.y >= m_context.height()) && 
            (b.x < 0 || b.x >= m_context.width() || b.y < 0 || b.y >= m_context.height()) &&
            (c.x < 0 || c.x >= m_context.width() || c.y < 0 || c.y >= m_context.height()))
          return;
 
        int minY = nearest(std::min(a.y, std::min(b.y, c.y)) + 0.5);
        int maxY = nearest(std::max(a.y, std::max(b.y, c.y)) - 0.5);

        if (minY < 0)
          minY = 0;
        if (maxY >= m_context.height())
          maxY = m_context.height() - 1;

        std::cout << "    minY = " << minY << ", maxY = " << maxY << std::endl;

        
        varying_type varying;

        for (int y = minY; y <= maxY; ++y) {

          // if the y value is outside the color buffer there is no need to draw it
          if (y < 0 || y >= m_context.height())
            continue;
          
          int intersections = 0;

          if (intersects(y, a, b))
            intersections |= 1;
          if (intersects(y, a, c))
            intersections |= 2;
          if (intersects(y, b, c))
            intersections |= 4;

          int xL, xR;
          double za, zb;
          switch (intersections) {
            case 3:
              // A-B and A-C intersect scanline
              {
                double x1 = intersection(y, a, b);
                double x2 = intersection(y, a, c);
                xL = nearest(std::min(x1, x2));
                xR = nearest(std::max(x1, x2));
                za = A.z() - (A.z() - B.z()) * (a.y - y) / (a.y - b.y);
                zb = A.z() - (A.z() - C.z()) * (a.y - y) / (a.y - c.y);
              }
              break;
            case 5:
              // A-B and B-C intersect scanline
              {
                double x1 = intersection(y, a, b);
                double x2 = intersection(y, b, c);
                xL = nearest(std::min(x1, x2));
                xR = nearest(std::max(x1, x2));
                za = B.z() - (B.z() - A.z()) * (b.y - y) / (b.y - a.y);
                zb = B.z() - (B.z() - C.z()) * (b.y - y) / (b.y - c.y);
              }
              break;
            case 6:
              // A-C and B-C intersect scanline
              {
                double x1 = intersection(y, a, c);
                double x2 = intersection(y, b, c);
                xL = nearest(std::min(x1, x2));
                xR = nearest(std::max(x1, x2));
                za = C.z() - (C.z() - A.z()) * (c.y - y) / (c.y - a.y);
                zb = C.z() - (C.z() - B.z()) * (c.y - y) / (c.y - b.y);
              }
              break;
          }


          for (int x = xL; x <= xR; ++x) {
            callInterpolationFunction(varyingA, varyingB, varyingC, a, b, c, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            //double z = 1.0001 * zG + (x - G.x) * dzdx + (y - G.y) * dzdy;
            double z = zb - (zb - za) * (xR - x) / static_cast<double>(xR - xL);
            m_context.drawPixel(x, y, z, color);
          }
        }
      }




    private:
      Context &m_context;
      ProgramType &m_program;
  };

}

#endif
