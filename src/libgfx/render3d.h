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
  Real triangleArea(const Point2D &A, const vec4 &B, const vec4 &C)
  {
    // shoelace or surveyor's formula
    return 0.5 * std::abs((A.x - C.x()) * (B.y() - A.y) - (A.x - B.x()) * (C.y() - A.y));
  }

  Real distance(Real x0, Real y0, Real x1, Real y1)
  {
    Real dx = x1 - x0;
    Real dy = y1 - y0;
    return std::sqrt(dx * dx + dy * dy);
  }

  Color interpolateVarying(const Color &cA, const Color &cB, const vec4 &A, const vec4 &B, const Point2D &I)
  {
    Real a = distance(B.x(), B.y(), I.x, I.y) / distance(A.x(), A.y(), B.x(), B.y());
    Real b = 1.0 - a;
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
                           const vec4 &A, const vec4 &B, const vec4 &C,
                           Real a, Real b, Real c)
  {
    return Color(cA.r * a + cB.r * b + cC.r * c,
                 cA.g * a + cB.g * b + cC.g * c,
                 cA.b * a + cB.b * b + cC.b * c);
  }

  TexCoord interpolateVarying(const TexCoord &uvA, const TexCoord &uvB, const TexCoord &uvC,
                              const vec4 &A, const vec4 &B, const vec4 &C,
                              Real a, Real b, Real c)
  {
    Real one_over_wA = 1.0 / A.w();
    Real one_over_wB = 1.0 / B.w();
    Real one_over_wC = 1.0 / C.w();

    Real u_over_wA = uvA.u * one_over_wA;
    Real u_over_wB = uvB.u * one_over_wB;
    Real u_over_wC = uvC.u * one_over_wC;

    Real v_over_wA = uvA.v * one_over_wA;
    Real v_over_wB = uvB.v * one_over_wB;
    Real v_over_wC = uvC.v * one_over_wC;

    Real one_over_w = one_over_wA * a + one_over_wB * b + one_over_wC * c;

    return TexCoord((u_over_wA * a + u_over_wB * b + u_over_wC * c) / one_over_w,
                    (v_over_wA * a + v_over_wB * b + v_over_wC * c) / one_over_w);
  }
 
  /**
   * @brief Interpolate normals across triangle.
   */
  vec4 interpolateVarying(const vec4 &nA, const vec4 &nB, const vec4 &nC,
                          const vec4 &A, const vec4 &B, const vec4 &C,
                          Real a, Real b, Real c)
  {
    return nA;
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I == sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...>&, const std::tuple<Tp...>&, const std::tuple<Tp...>&,
      const vec4&, const vec4&, const vec4&, Real, Real, Real, std::tuple<Tp...>&)
  {
  }

  template<int I = 0, typename... Tp>
  typename std::enable_if<I < sizeof...(Tp), void>::type callInterpolationFunction(
      const std::tuple<Tp...> &varyingA, const std::tuple<Tp...> &varyingB, const std::tuple<Tp...> &varyingC,
      const vec4 &A, const vec4 &B, const vec4 &C, Real a, Real b, Real c, std::tuple<Tp...> &varying)
  {
    std::get<I>(varying) = interpolateVarying(std::get<I>(varyingA), std::get<I>(varyingB), std::get<I>(varyingC), A, B, C, a, b, c);
    callInterpolationFunction<I + 1, Tp...>(varyingA, varyingB, varyingC, A, B, C, a, b, c, varying);
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
        Real x = 0;
        Real y = 0;
        Real w = m_context.width() / 2.0;
        Real h = m_context.height() / 2.0;
        Real n = 0;
        Real f = 1;

        v.x() = w * v.x() + (x + w);
        v.y() = h * v.y() + (y + h);
        v.z() = ((f - n) / 2.0) * v.z() + (f + n) / 2.0;
      }
      
      void perspectiveDivide(vec4 &v)
      {
        Real one_over_w = 1.0 / v.w();
        v.x() *= one_over_w;
        v.y() *= one_over_w;
        v.z() *= one_over_w;
      }

      void drawLines(const Real *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 2)) == 0);

        for (std::size_t i = stride; i < size; i += stride * 2)
          drawLine(attributes + i - stride, attributes + i);
      }

      void drawTriangles(const Real *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 3)) == 0);

        for (std::size_t i = stride * 2; i < size; i += stride * 3)
          drawTriangle(attributes + i - stride * 2, attributes + i - stride, attributes + i);
      }

      void drawQuads(const Real *attributes, std::size_t size, std::size_t stride)
      {
        assert((size % (stride * 4)) == 0);

        for (std::size_t i = stride * 3; i < size; i += stride * 4) {
          drawTriangle(attributes + i - stride * 3, attributes + i - stride * 2, attributes + i - stride);
          drawTriangle(attributes + i - stride * 3, attributes + i - stride, attributes + i);
        }
      }
    
    private:

      void drawLine(const Real *attributesA, const Real *attributesB)
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
        
        Real m = ((Real) B.y() - (Real) A.y()) / ((Real) B.x() - (Real) A.x());
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

      bool intersects(Real y, const vec4 &P, const vec4 &Q)
      {
        return (P.y() != Q.y()) && ((y - P.y()) * (y - Q.y()) <= 0.0);
      }

      Real intersection(Real y, const vec4 &P, const vec4 &Q)
      {
        return Q.x() + (P.x() - Q.x()) * (y - Q.y()) / (P.y() - Q.y());
      }

      void barycentricCoeff(const vec4 &A, const vec4 &B, const vec4 &C, const Point2D &I, Real &a, Real &b, Real &c)
      {
        Real total = triangleArea(Point2D(A.x(), A.y()), B, C);
        a = triangleArea(I, B, C) / total;
        b = triangleArea(I, A, C) / total;
        c = triangleArea(I, A, B) / total;
      }

      std::pair<int, int> computeXRange(int y, const vec4 &A, const vec4 &B, const vec4 &C)
      {
        // compute the intersections between scanline and triangle edges
        int intersections = 0;
        if (intersects(y, A, B))
          intersections |= 1;
        if (intersects(y, A, C))
          intersections |= 2;
        if (intersects(y, B, C))
          intersections |= 4;

        Real xa, xb;
        switch (intersections) {
          case 3:
            // A-B and A-C intersect scanline
            xa = intersection(y, A, B);
            xb = intersection(y, A, C);
            break;
          case 5:
            // A-B and B-C intersect scanline
            xa = intersection(y, A, B);
            xb = intersection(y, B, C);
            break;
          case 6:
            // A-C and B-C intersect scanline
            xb = intersection(y, A, C);
            xa = intersection(y, B, C);
            break;
        }

        if (xa < xb)
          return std::make_pair(nearest(xa + 0.5), nearest(xb - 0.5));
        return std::make_pair(nearest(xb + 0.5), nearest(xa - 0.5));
      }

      void drawTriangle(const Real *attributesA, const Real *attributesB, const Real *attributesC)
      {
        // varyings to be filled in by vertex shader
        varying_type varyingA;
        varying_type varyingB;
        varying_type varyingC;

        // execute vertex shader (model coordinates -> clip coordinates)
        vec4 A = m_program.vertexShader().exec(attributesA, varyingA);
        vec4 B = m_program.vertexShader().exec(attributesB, varyingB);
        vec4 C = m_program.vertexShader().exec(attributesC, varyingC);

        // perspective divide (clip coordinates -> normalized device coordinates (NDC))
        perspectiveDivide(A);
        perspectiveDivide(B);
        perspectiveDivide(C);

        // compute screen coordinates (NDC -> screen coordinates)
        screenCoordinates(A);
        screenCoordinates(B);
        screenCoordinates(C);
       
        // determine y range in screen coordinates
        int minY = nearest(std::min(A.y(), std::min(B.y(), C.y())) + 0.5);
        int maxY = nearest(std::max(A.y(), std::max(B.y(), C.y())) - 0.5);

        if (minY < 0)
          minY = 0;
        if (maxY >= m_context.height())
          maxY = m_context.height() - 1;

        // interpolated varying for fragment shader
        varying_type varying;

        for (int y = minY; y <= maxY; ++y) {
          // compute x range in screen coordinates
          std::pair<int, int> xRange = computeXRange(y, A, B, C);
          int xL = std::max(xRange.first, 0);
          int xR = std::min(xRange.second, m_context.width() - 1);

          for (int x = xL; x <= xR; ++x) {
            // compute barycontric coefficients for interpolation            
            Real a, b, c;
            barycentricCoeff(A, B, C, GFX::Point2D(x, y), a, b, c);
            
            // interpolate 1/z
            Real z = (1.0 / A.z()) * a + (1.0 / B.z()) * b + (1.0 / C.z()) * c;
            
            // interpolate varyings
            callInterpolationFunction(varyingA, varyingB, varyingC, A, B, C, a, b, c, varying);

            // execute fragment shader
            Color color = m_program.fragmentShader().exec(varying);

            // draw the pixel
            m_context.drawPixel(x, y, z, color);
          }
        }
      }
      
      Context &m_context;
      ProgramType &m_program;
  };

}

#endif
