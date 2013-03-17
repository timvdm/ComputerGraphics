#ifndef GFX_SHADERS_H
#define GFX_SHADERS_H

#include "types.h"

#include <cmath>

namespace GFX {

  /**
   * @brief Vertex & Fragment shader functions.
   *
   * These are the built-in functions available to GLSL and OpenGL ES shaders.
   */
  namespace ShaderFunctions {

    /**
     * @name Angle & Trigonometry Functions
     *
     * Component-wise operation. Parameters specified as angle are assumed to
     * be in units of radians. T is double, vec2, vec3, vec4.
     */
    //@{

    /**
     * @brief Degrees to radians.
     */
    template<typename T>
    T radians(const T &value)
    {
      return value * M_PI / 180.0;
    }

    //@cond IMPL
    template<>
    vec2 radians<vec2>(const vec2 &v)
    {
      return vec2(radians(v.x()), radians(v.y()));
    }
    template<>
    vec3 radians<vec3>(const vec3 &v)
    {
      return vec3(radians(v.x()), radians(v.y()), radians(v.z()));
    }
    template<>
    vec4 radians<vec4>(const vec4 &v)
    {
      return vec4(radians(v.x()), radians(v.y()), radians(v.z()), radians(v.w()));
    }
    //@endcond

    /**
     * @brief Radians to degrees.
     */
    template<typename T>
    T degrees(const T &value)
    {
      return value * 180.0 / M_PI;
    }

    //@cond IMPL
    template<>
    vec2 degrees<vec2>(const vec2 &v)
    {
      return vec2(degrees(v.x()), degrees(v.y()));
    }
    template<>
    vec3 degrees<vec3>(const vec3 &v)
    {
      return vec3(degrees(v.x()), degrees(v.y()), degrees(v.z()));
    }
    template<>
    vec4 degrees<vec4>(const vec4 &v)
    {
      return vec4(degrees(v.x()), degrees(v.y()), degrees(v.z()), degrees(v.w()));
    }
    //@endcond

    /**
     * @brief Sine.
     */
    template<typename T>
    T sin(const T &value)
    {
      return std::sin(value);
    }

    //@cond IMPL
    template<>
    vec2 sin<vec2>(const vec2 &v)
    {
      return vec2(sin(v.x()), sin(v.y()));
    }
    template<>
    vec3 sin<vec3>(const vec3 &v)
    {
      return vec3(sin(v.x()), sin(v.y()), sin(v.z()));
    }
    template<>
    vec4 sin<vec4>(const vec4 &v)
    {
      return vec4(sin(v.x()), sin(v.y()), sin(v.z()), sin(v.w()));
    }
    //@endcond
    
    /**
     * @brief Cosine.
     */
    template<typename T>
    T cos(const T &value)
    {
      return std::cos(value);
    }

    //@cond IMPL
    template<>
    vec2 cos<vec2>(const vec2 &v)
    {
      return vec2(cos(v.x()), cos(v.y()));
    }
    template<>
    vec3 cos<vec3>(const vec3 &v)
    {
      return vec3(cos(v.x()), cos(v.y()), cos(v.z()));
    }
    template<>
    vec4 cos<vec4>(const vec4 &v)
    {
      return vec4(cos(v.x()), cos(v.y()), cos(v.z()), cos(v.w()));
    }
    //@endcond
 
    /**
     * @brief Tangent.
     */
    template<typename T>
    T tan(const T &value)
    {
      return std::tan(value);
    }

    //@cond IMPL
    template<>
    vec2 tan<vec2>(const vec2 &v)
    {
      return vec2(tan(v.x()), tan(v.y()));
    }
    template<>
    vec3 tan<vec3>(const vec3 &v)
    {
      return vec3(tan(v.x()), tan(v.y()), tan(v.z()));
    }
    template<>
    vec4 tan<vec4>(const vec4 &v)
    {
      return vec4(tan(v.x()), tan(v.y()), tan(v.z()), tan(v.w()));
    }
    //@endcond

    /**
     * @brief Arc sine.
     */
    template<typename T>
    T asin(const T &value)
    {
      return std::asin(value);
    }

    //@cond IMPL
    template<>
    vec2 asin<vec2>(const vec2 &v)
    {
      return vec2(asin(v.x()), asin(v.y()));
    }
    template<>
    vec3 asin<vec3>(const vec3 &v)
    {
      return vec3(asin(v.x()), asin(v.y()), asin(v.z()));
    }
    template<>
    vec4 asin<vec4>(const vec4 &v)
    {
      return vec4(asin(v.x()), asin(v.y()), asin(v.z()), asin(v.w()));
    }
    //@endcond
 
    /**
     * @brief Arc cosine.
     */
    template<typename T>
    T acos(const T &value)
    {
      return std::acos(value);
    }

    //@cond IMPL
    template<>
    vec2 acos<vec2>(const vec2 &v)
    {
      return vec2(acos(v.x()), acos(v.y()));
    }
    template<>
    vec3 acos<vec3>(const vec3 &v)
    {
      return vec3(acos(v.x()), acos(v.y()), acos(v.z()));
    }
    template<>
    vec4 acos<vec4>(const vec4 &v)
    {
      return vec4(acos(v.x()), acos(v.y()), acos(v.z()), acos(v.w()));
    }
    //@endcond

    /**
     * @brief Arc tangent.
     */
    template<typename T>
    T atan2(const T &y, const T &x)
    {
      return std::atan2(y, x);
    }

    //@cond IMPL
    template<>
    vec2 atan2<vec2>(const vec2 &y, const vec2 &x)
    {
      return vec2(atan2(y.x(), x.x()), atan2(y.y(), x.y()));
    }
    template<>
    vec3 atan2<vec3>(const vec3 &y, const vec3 &x)
    {
      return vec3(atan2(y.x(), x.x()), atan2(y.y(), x.y()), atan2(y.z(), x.z()));
    }
    template<>
    vec4 atan2<vec4>(const vec4 &y, const vec4 &x)
    {
      return vec4(atan2(y.x(), x.x()), atan2(y.y(), x.y()), atan2(y.z(), x.z()), atan2(y.w(), x.w()));
    }
    //@endcond
 
    /**
     * @brief Arc tangent.
     */
    template<typename T>
    T atan(const T &y_over_x)
    {
      return std::atan(y_over_x);
    }

    //@cond IMPL
    template<>
    vec2 atan<vec2>(const vec2 &y_over_x)
    {
      return vec2(atan(y_over_x.x()), atan(y_over_x.y()));
    }
    template<>
    vec3 atan<vec3>(const vec3 &y_over_x)
    {
      return vec3(atan(y_over_x.x()), atan(y_over_x.y()), atan(y_over_x.z()));
    }
    template<>
    vec4 atan<vec4>(const vec4 &y_over_x)
    {
      return vec4(atan(y_over_x.x()), atan(y_over_x.y()), atan(y_over_x.z()), atan(y_over_x.w()));
    }
    //@endcond
 






    //@}

  }
}

#endif