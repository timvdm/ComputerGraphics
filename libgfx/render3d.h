#ifndef GFX_RENDERER_H
#define GFX_RENDERER_H

#include "utility.h"
#include "context.h"
#include "transform.h"

#include <limits>
#include <tuple>

namespace GFX {

  namespace impl {

    /**
     * @brief Compute triangle area given three 2D coordinates.
     *
     * @param A The 2D coordinates for the first vertex.
     * @param B The 2D coordinates for the second vertex.
     * @param C The 2D coordinates for the last vertex.
     *
     §* @return The interpolated color.
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

    ColorF interpolateVarying(const ColorF &cA, const ColorF &cB, const vec4 &A, const vec4 &B, const Point2D &I)
    {
      Real a = distance(B.x(), B.y(), I.x, I.y) / distance(A.x(), A.y(), B.x(), B.y());
      Real b = 1.0 - a;
      return ColorF(cA.r * a + cB.r * b, cA.g * a + cB.g * b, cA.b * a + cB.b * b);
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
     * @brief Interpolate Real across triangle.
     */
    Real interpolateVarying(Real rA, Real rB, Real rC,
        const vec4 &A, const vec4 &B, const vec4 &C,
        Real a, Real b, Real c)
    {
      return rA * a + rB * b + rC * c;
    }

    /**
     * @brief Interpolate a ColorF across a triangle using barycentric interpolation.
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
    ColorF interpolateVarying(const ColorF &cA, const ColorF &cB, const ColorF &cC,
        const vec4 &A, const vec4 &B, const vec4 &C,
        Real a, Real b, Real c)
    {
      return ColorF(cA.r * a + cB.r * b + cC.r * c,
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
      return nA * a + nB * b + nC * c;
    }

    /**
     * @brief Interpolate normals across triangle.
     */
    vec3 interpolateVarying(const vec3 &nA, const vec3 &nB, const vec3 &nC,
        const vec4 &A, const vec4 &B, const vec4 &C,
        Real a, Real b, Real c)
    {
      return nA * a + nB * b + nC * c;
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

    void perspective_divide(vec4 &v)
    {
      Real one_over_w = 1.0 / v.w();
      v.x() *= one_over_w;
      v.y() *= one_over_w;
      v.z() *= one_over_w;
    }

    bool intersects(Real y, const vec4 &P, const vec4 &Q)
    {
      return (P.y() != Q.y()) && ((y - P.y()) * (y - Q.y()) <= 0.0);
    }

    Real intersection(Real y, const vec4 &P, const vec4 &Q)
    {
      return Q.x() + (P.x() - Q.x()) * (y - Q.y()) / (P.y() - Q.y());
    }

    void barycentric_coeff(const vec4 &A, const vec4 &B, const vec4 &C, const Point2D &I, Real &a, Real &b, Real &c)
    {
      Real total = triangleArea(Point2D(A.x(), A.y()), B, C);
      a = triangleArea(I, B, C) / total;
      b = triangleArea(I, A, C) / total;
      c = triangleArea(I, A, B) / total;
    }

    std::pair<int, int> compute_x_range(int y, const vec4 &A, const vec4 &B, const vec4 &C)
    {
      // compute the intersections between scanline and triangle edges
      int intersections = 0;
      if (intersects(y, A, B))
        intersections |= 1;
      if (intersects(y, A, C))
        intersections |= 2;
      if (intersects(y, B, C))
        intersections |= 4;

      Real xa = 0.0, xb = 0.0;
      switch (intersections) {
        case 1:
        case 2:
        case 4:
          assert(0);
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
        case 7:
          // All points on scanline
          xa = std::min(A.x(), std::min(B.x(), C.x()));
          xb = std::max(A.x(), std::max(B.x(), C.x()));
          break;
      }

      if (xa < xb)
        return std::make_pair(nearest(xa + 0.5), nearest(xb - 0.5));
      return std::make_pair(nearest(xb + 0.5), nearest(xa - 0.5));
    }

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

      void drawLine(const Real *attributesA, const Real *attributesB)
      {
        // varyings to be filled in by vertex shader
        varying_type varyingA;
        varying_type varyingB;

        // execute vertex shader (model coordinates -> clip coordinates)
        vec4 A = m_program.vertexShader().exec(attributesA, varyingA);
        vec4 B = m_program.vertexShader().exec(attributesB, varyingB);

        // perspective divide (clip coordinates -> normalized device coordinates (NDC))
        impl::perspective_divide(A);
        impl::perspective_divide(B);

        // compute screen coordinates (NDC -> screen coordinates)
        screenCoordinates(A);
        screenCoordinates(B);

        varying_type varying;

        if (A.x() == B.x()) {
          // special case for A.x() == B.x()
          int minY = nearest(std::min(A.y(), B.y()));
          int maxY = nearest(std::max(A.y(), B.y()));
          int numY = maxY - minY;

          if (minY < 0)
            minY = 0;
          if (maxY > m_context.height() - 1)
            maxY = m_context.height() - 1;

          for (int i = minY; i <= maxY; ++i) {
            GFX::Real z = interpolateLineZ(A.z(), B.z(), i, numY);
            impl::callInterpolationFunction(varyingA, varyingB, A, B, Point2D(A.x(), i), varying);
            vec3 pos(A.x(), i, z);
            ColorF color = m_program.fragmentShader().exec(varying, m_context.textures(), pos, false);
            m_context.drawPixel(pos.x(), pos.y(), pos.z(), color);
          }
          return;
        }

        if (A.y() == B.y()) {
          // special case for A.y() == B.y()
          int minX = nearest(std::min(A.x(), B.x()));
          int maxX = nearest(std::max(A.x(), B.x()));
          int numX = maxX - minX;

          if (minX < 0)
            minX = 0;
          if (maxX > m_context.width() - 1)
            maxX = m_context.width() - 1;

          for (int i = minX; i <= maxX; ++i) {
            GFX::Real z = interpolateLineZ(A.z(), B.z(), i, numX);
            impl::callInterpolationFunction(varyingA, varyingB, A, B, Point2D(i, A.y()), varying);
            vec3 pos(i, A.y(), z);
            ColorF color = m_program.fragmentShader().exec(varying, m_context.textures(), pos, false);
            m_context.drawPixel(pos.x(), pos.y(), pos.z(), color);
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
          int num = B.x() - A.x();
          for (int i = 0; i <= (B.x() - A.x()); ++i) {
            int x = A.x() + i;
            int y = round(A.y() + m * i);

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            GFX::Real z = interpolateLineZ(A.z(), B.z(), i, num);
            impl::callInterpolationFunction(varyingA, varyingB, A, B, Point2D(x, y), varying);
            vec3 pos(x, y, z);
            ColorF color = m_program.fragmentShader().exec(varying, m_context.textures(), pos, false);
            m_context.drawPixel(pos.x(), pos.y(), pos.z(), color);
          }
        } else if (m > 1.0) {
          int num = B.y() - A.y();
          for (int i = 0; i <= (B.y() - A.y()); ++i) {
            int x = round(A.x() + i / m);
            int y = A.y() + i;

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            GFX::Real z = interpolateLineZ(A.z(), B.z(), i, num);
            impl::callInterpolationFunction(varyingA, varyingB, A, B, Point2D(x, y), varying);
            vec3 pos(x, y, z);
            ColorF color = m_program.fragmentShader().exec(varying, m_context.textures(), pos, false);
            m_context.drawPixel(pos.x(), pos.y(), pos.z(), color);
          }
        } else if (m < -1.0) {
          int num = A.y() - B.y();
          for (int i = 0; i <= (A.y() - B.y()); ++i) {
            int x = round(A.x() - i / m);
            int y = A.y() - i;

            if (x < 0 || x > m_context.width() - 1 || y < 0 || y > m_context.height() - 1)
              continue;

            GFX::Real z = interpolateLineZ(A.z(), B.z(), i, num);
            impl::callInterpolationFunction(varyingA, varyingB, A, B, Point2D(x, y), varying);
            vec3 pos(x, y, z);
            ColorF color = m_program.fragmentShader().exec(varying, m_context.textures(), pos, false);
            m_context.drawPixel(pos.x(), pos.y(), pos.z(), color);
          }
        }
      }

      bool clip(const vec4 &v)
      {
        Real w = std::abs(v.w());
        if (v.x() < -w || v.x() > w)
          return true;
        if (v.y() < -w || v.y() > w)
          return true;
        if (v.z() < -w || v.z() > w)
          return true;
        return false;
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
        
        //std::cout << "z: " << A.z() << ", " << B.z() << ", " << C.z() << std::endl;
        //std::cout << "w: " << A.w() << ", " << B.w() << ", " << C.w() << std::endl;

        //if (clip(A) || clip(B) || clip(C))
        //  return;

        // perspective divide (clip coordinates -> normalized device coordinates (NDC))
        impl::perspective_divide(A);
        impl::perspective_divide(B);
        impl::perspective_divide(C);

        // compute screen coordinates (NDC -> screen coordinates)
        screenCoordinates(A);
        screenCoordinates(B);
        screenCoordinates(C);

        // compute face normal
        GFX::vec3 u = GFX::vec3(B.data()) - GFX::vec3(A.data());
        GFX::vec3 v = GFX::vec3(C.data()) - GFX::vec3(A.data());
        GFX::vec3 w = u.cross(v);
        Real angle = w.dot(GFX::vec3(0, 0, -1));
        bool backFacing = angle < 0;

        // perform face culling
        switch (m_context.flags(GFX_FACE_CULLING)) {
          case GFX_FRONT_FACE:
            if (!backFacing)
              return;
            break;
          case GFX_BACK_FACE:
            if (backFacing)
              return;
            break;
          case GFX_FRONT_FACE | GFX_BACK_FACE:
            return;
          default:
            break;
        }


        // determine y range in screen coordinates
        int minY = nearest(std::min(A.y(), std::min(B.y(), C.y())) + 0.5);
        int maxY = nearest(std::max(A.y(), std::max(B.y(), C.y())) - 0.5);

        // determine x range in screen coordinates
        int minX = std::min(A.x(), std::min(B.x(), C.x()));
        int maxX = std::max(A.x(), std::max(B.x(), C.x()));
        // select mipmaps based on x range
        m_context.setMipmaps(maxX - minX, maxY - minY);

        if (minY < 0)
          minY = 0;
        if (maxY >= m_context.height())
          maxY = m_context.height() - 1;

        // interpolated varying for fragment shader
        varying_type varying;

        for (int y = minY; y <= maxY; ++y) {
          // compute x range in screen coordinates
          std::pair<int, int> xRange = impl::compute_x_range(y, A, B, C);
          int xL = std::max(xRange.first, 0);
          int xR = std::min(xRange.second, m_context.width() - 1);

          for (int x = xL; x <= xR; ++x) {
            // compute barycentric coefficients for interpolation
            Real a, b, c;
            impl::barycentric_coeff(A, B, C, GFX::Point2D(x, y), a, b, c);

            // interpolate 1/z
            //Real z = 1.0 / ((1.0 / A.z()) * a + (1.0 / B.z()) * b + (1.0 / C.z()) * c);
            Real z = A.z() * a + B.z() * b + C.z() * c;

            vec3 pos(x, y, z);

            if (m_context.flags(GFX_BUFFER) & GFX_COLOR_BUFFER) {
              // interpolate varyings
              impl::callInterpolationFunction(varyingA, varyingB, varyingC, A, B, C, a, b, c, varying);

              // execute fragment shader
              ColorF color = m_program.fragmentShader().exec(varying, m_context.textures(), pos, backFacing);

              // draw the pixel
              m_context.drawPixel(pos.x(), pos.y(), pos.z(), color);
            }
          }
        }
      }

      Context &m_context;
      ProgramType &m_program;
  };

}

#endif
