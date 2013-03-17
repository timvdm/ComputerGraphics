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
  double triangleArea(const Point2D &A, const vec4 &B, const vec4 &C)
  {
    // shoelace or surveyor's formula
    return 0.5 * std::abs((A.x - C.x()) * (B.y() - A.y) - (A.x - B.x()) * (C.y() - A.y));
  }

  double distance(double x0, double y0, double x1, double y1)
  {
    double dx = x1 - x0;
    double dy = y1 - y0;
    return std::sqrt(dx * dx + dy * dy);
  }

  Color interpolateVarying(const Color &cA, const Color &cB, const vec4 &A, const vec4 &B, const Point2D &I)
  {
    double a = distance(B.x(), B.y(), I.x, I.y) / distance(A.x(), A.y(), B.x(), B.y());
    double b = 1.0 - a;
    return Color(cA.r * a + cB.r * b, cA.g * a + cB.g * b, cA.b * a + cB.b * b);
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I == sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...>&, const std::tuple<Tp...>&, const vec4 &,
      const vec4 &, const Point2D &, std::tuple<Tp...>&)
  {
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I < sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...> &varyingA, const std::tuple<Tp...> &varyingB, 
      const vec4 &A, const vec4 &B, const Point2D &C, std::tuple<Tp...> &varying)
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
      const vec4 &A, const vec4 &B, const vec4 &C, const Point2D &I)
  {
    double total = triangleArea(Point2D(A.x(), A.y()), B, C);
    double a = triangleArea(I, B, C) / total;
    double b = triangleArea(I, A, C) / total;
    double c = triangleArea(I, A, B) / total;
    return Color(cA.r * a + cB.r * b + cC.r * c,
                 cA.g * a + cB.g * b + cC.g * c,
                 cA.b * a + cB.b * b + cC.b * c);
  }

  /**
   * @brief Interpolate normals across triangle.
   */
  vec4 interpolateVarying(const vec4 &nA, const vec4 &nB, const vec4 &nC,
      const vec4 &A, const vec4 &B, const vec4 &C, const Point2D &I)
  {
    return nA;
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I == sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...>&, const std::tuple<Tp...>&, const std::tuple<Tp...>&,
      const vec4&, const vec4&, const vec4&, const Point2D&, std::tuple<Tp...>&)
  {
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I < sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...> &varyingA, const std::tuple<Tp...> &varyingB, const std::tuple<Tp...> &varyingC,
      const vec4 &A, const vec4 &B, const vec4 &C, const Point2D &E, std::tuple<Tp...> &varying)
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

      void drawLines(const double *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 2)) == 0);

        for (std::size_t i = stride; i < size; i += stride * 2)
          drawLine(attributes + i - stride, attributes + i);
      }

      void drawTriangles(const double *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 3)) == 0);

        for (std::size_t i = stride * 2; i < size; i += stride * 3)
          drawTriangle(attributes + i - stride * 2, attributes + i - stride, attributes + i);
      }

      void drawQuads(const double *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 4)) == 0);

        for (std::size_t i = stride * 3; i < size; i += stride * 4) {
          drawTriangle(attributes + i - stride * 3, attributes + i - stride * 2, attributes + i - stride);
          drawTriangle(attributes + i - stride * 3, attributes + i - stride, attributes + i);
        }
      }
    
    private:

      void drawLine(const double *attributesA, const double *attributesB)
      {
        varying_type varyingA;
        varying_type varyingB;

        // execute vertex shader
        vec4 A = m_program.vertexShader().exec(attributesA, varyingA);
        vec4 B = m_program.vertexShader().exec(attributesB, varyingB);

        //std::cout << "Line: " << std::endl;
        //std::cout << "    clip:      " << print(A) << " -> " << print(B) << std::endl;

        // perspective divide
        A /= A.w();
        B /= B.w();

        //std::cout << "    NDC:       " << print(A) << " -> " << print(B) << std::endl;

        screenCoordinates(A);
        screenCoordinates(B);

        //std::cout << "    screen:    " << print(A) << " -> " << print(B) << std::endl;

        //std::cout << "    Color: " << std::get<0>(varyingA) << " -> " << std::get<0>(varyingB) << std::endl;

        varying_type varying;

        if (A.x() == B.x()) {
          // special case for A.x() == B.x()
          int minY = nearest(std::min(A.y(), B.y()));
          int maxY = nearest(std::max(A.y(), B.y()));
          int numY = maxY - minY + 1;

          if (minY < 0)
            minY = 0;
          if (maxY > m_context.height() - 1)
            maxY = m_context.height() - 1;

          for (int i = minY; i <= maxY; ++i) {
            callInterpolationFunction(varyingA, varyingB, A, B, Point2D(A.x(), i), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(A.x(), i, interpolateLineZ(A.z(), B.z(), i, numY), color);
          }
          return;
        } 
        
        if (A.y() == B.y()) {
          // special case for A.y() == B.y()
          int minX = nearest(std::min(A.x(), B.x()));
          int maxX = nearest(std::max(A.x(), B.x()));
          int numX = maxX - minX + 1;

          if (minX < 0)
            minX = 0;
          if (maxX > m_context.width() - 1)
            maxX = m_context.width() - 1;

          for (int i = minX; i <= maxX; ++i) {
            callInterpolationFunction(varyingA, varyingB, A, B, Point2D(i, A.y()), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(i, A.y(), interpolateLineZ(A.z(), B.z(), i, numX), color);
          }
          return;
        }
        
        if (A.x() > B.x()) {
          // flip points if B.x() > A.x(): we want A.x() to have the lowest value
          std::swap(A, B);
          std::swap(varyingA, varyingB);
        }
        
        double m = ((double) B.y() - (double) A.y()) / ((double) B.x() - (double) A.x());
        if (-1.0 <= m && m <= 1.0) {
          int num = B.x() - A.x() + 1;
          for (int i = 0; i <= (B.x() - A.x()); ++i) {
            int x = A.x() + i;
            int y = round(A.y() + m * i);

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            callInterpolationFunction(varyingA, varyingB, A, B, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(x, y, interpolateLineZ(A.z(), B.z(), i, num), color);
          }
        } else if (m > 1.0) {
          int num = B.y() - A.y() + 1;
          for (int i = 0; i <= (B.y() - A.y()); ++i) {
            int x = round(A.x() + i / m);
            int y = A.y() + i;

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            callInterpolationFunction(varyingA, varyingB, A, B, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(x, y, interpolateLineZ(A.z(), B.z(), i, num), color);
          }
        } else if (m < -1.0) {
          int num = A.y() - B.y() + 1;
          for (int i = 0; i <= (A.y() - B.y()); ++i) {
            int x = round(A.x() - i / m);
            int y = A.y() - i;

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            callInterpolationFunction(varyingA, varyingB, A, B, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);
            m_context.drawPixel(x, y, interpolateLineZ(A.z(), B.z(), i, num), color);
          }
        }
      }
      bool intersects(double y, const vec4 &P, const vec4 &Q)
      {
        return (P.y() != Q.y()) && ((y - P.y()) * (y - Q.y()) <= 0.0);
      }

      double intersection(double y, const vec4 &P, const vec4 &Q)
      {
        return Q.x() + (P.x() - Q.x()) * (y - Q.y()) / (P.y() - Q.y());
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
        
        //std::cout << "    NDC:       " << print(A) << " -> " << print(B) << " -> " << print(C) << std::endl;

        screenCoordinates(A);
        screenCoordinates(B);
        screenCoordinates(C);
        
        //std::cout << "Triangle: " << print(A) << " -> " << print(B) << " -> " << print(C) << std::endl;

        int minY = nearest(std::min(A.y(), std::min(B.y(), C.y())) + 0.5);
        int maxY = nearest(std::max(A.y(), std::max(B.y(), C.y())) - 0.5);

        if (minY < 0)
          minY = 0;
        if (maxY >= m_context.height())
          maxY = m_context.height() - 1;

        //std::cout << "    minY = " << minY << ", maxY = " << maxY << std::endl;

        varying_type varying;

        for (int y = minY; y <= maxY; ++y) {

          // if the y value is outside the color buffer there is no need to draw it
          if (y < 0 || y >= m_context.height())
            continue;
          
          int intersections = 0;

          if (intersects(y, A, B))
            intersections |= 1;
          if (intersects(y, A, C))
            intersections |= 2;
          if (intersects(y, B, C))
            intersections |= 4;

          int xL, xR;
          double za, zb;
          switch (intersections) {
            case 3:
              // A-B and A-C intersect scanline
              {
                double x1 = intersection(y, A, B);
                double x2 = intersection(y, A, C);
                if (x1 < x2) {
                  xL = nearest(x1 + 0.5);
                  xR = nearest(x2 - 0.5);
                  za = A.z() - (A.z() - B.z()) * (A.y() - y) / (A.y() - B.y());
                  zb = A.z() - (A.z() - C.z()) * (A.y() - y) / (A.y() - C.y());
                } else {
                  xL = nearest(x2 + 0.5);
                  xR = nearest(x1 - 0.5);
                  zb = A.z() - (A.z() - B.z()) * (A.y() - y) / (A.y() - B.y());
                  za = A.z() - (A.z() - C.z()) * (A.y() - y) / (A.y() - C.y());
                }
              }
              break;
            case 5:
              // A-B and B-C intersect scanline
              {
                double x1 = intersection(y, A, B);
                double x2 = intersection(y, B, C);
                if (x1 < x2) {
                  xL = nearest(x1 + 0.5);
                  xR = nearest(x2 - 0.5);
                  za = B.z() - (B.z() - A.z()) * (B.y() - y) / (B.y() - A.y());
                  zb = B.z() - (B.z() - C.z()) * (B.y() - y) / (B.y() - C.y());
                } else {
                  xL = nearest(x2 + 0.5);
                  xR = nearest(x1 - 0.5);
                  zb = B.z() - (B.z() - A.z()) * (B.y() - y) / (B.y() - A.y());
                  za = B.z() - (B.z() - C.z()) * (B.y() - y) / (B.y() - C.y());
                }
              }
              break;
            case 6:
              // A-C and B-C intersect scanline
              {
                double x1 = intersection(y, A, C);
                double x2 = intersection(y, B, C);
                if (x1 < x2) {
                  xL = nearest(x1 + 0.5);
                  xR = nearest(x2 - 0.5);
                  za = C.z() - (C.z() - A.z()) * (C.y() - y) / (C.y() - A.y());
                  zb = C.z() - (C.z() - B.z()) * (C.y() - y) / (C.y() - B.y());
                } else {
                  xL = nearest(x2 + 0.5);
                  xR = nearest(x1 - 0.5);
                  zb = C.z() - (C.z() - A.z()) * (C.y() - y) / (C.y() - A.y());
                  za = C.z() - (C.z() - B.z()) * (C.y() - y) / (C.y() - B.y());
                }
              }
              break;
          }

          double dzdx = (zb - za) / static_cast<double>(xR - xL + 1);

          for (int x = xL; x <= xR; ++x) {
            callInterpolationFunction(varyingA, varyingB, varyingC, A, B, C, Point2D(x, y), varying);
            Color color = m_program.fragmentShader().exec(varying);

            double z = zb - (xR - x) * dzdx;

            m_context.drawPixel(x, y, z, color);
          }
        }
      }
      
      Context &m_context;
      ProgramType &m_program;
  };

}

#endif
