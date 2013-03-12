#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <Eigen/Geometry>

namespace GFX {

  /**
   * @brief Type to represent real numbers.
   */
  typedef double Real;

  typedef Eigen::Vector2d vec2;
  typedef Eigen::Vector3d vec3;
  typedef Eigen::Vector4d vec4;

  typedef Eigen::Matrix2d mat2;
  typedef Eigen::Matrix3d mat3;
  typedef Eigen::Matrix4d mat4;

}

#endif
