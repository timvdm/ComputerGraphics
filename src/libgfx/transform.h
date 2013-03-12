#ifndef GFX_TRANSFORM_H
#define GFX_TRANSFORM_H

#include "types.h"

/**
 * @brief The GFX namespace.
 */
namespace GFX {

  /**
   * @brief Compute the projection matrix given the eye's polar coordinates.
   *
   * @param eye The eye (i.e. camera) polar coordinates.
   *
   * @return The projection matrix.
   */
  mat4 projectionMatrix(double x, double y, double z);


  mat4 lookAtMatrix(double eyeX,    double eyeY,    double eyeZ,
                    double centerX, double centerY, double centerZ,
                    double upX,     double upY,     double upZ);

  mat4 frustumMatrix(double left, double right, double top, double bottom, double near, double far);

  mat4 translationMatrix(double x, double y, double z);

  mat4 xRotationMatrix(double angle);
  mat4 yRotationMatrix(double angle);
  mat4 zRotationMatrix(double angle);

  //mat4 rotationMatrix(double angle, double x, double y, double z);

  mat4 scaleMatrix(double factor);
  mat4 scaleMatrix(double x, double y, double z);

}

#endif
