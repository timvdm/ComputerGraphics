#ifndef GFX_TRANSFORM_H
#define GFX_TRANSFORM_H

#include "types.h"

/**
 * @brief The GFX namespace.
 */
namespace GFX {

  //! @name Projection Matrices
  //@{

  /**
   * @brief Perspective projection matrix from course material.
   *
   * B. van Houdt, Computer Graphics, University Antwerp, p. 21, code 4123, 2013
   *
   * @param x The x coordinate for the eye/camera.
   * @param y The y coordinate for the eye/camera.
   * @param z The z coordinate for the eye/camera.
   *
   * @return The projection matrix.
   */
  mat4 projectionMatrix(double x, double y, double z);

  /**
   * @brief Orthographic projection matrix.
   *
   * This projection matrix is the same as the OpenGL glOrtho() matrix.
   *
   * @param left Coordinate of the left clipping plane.
   * @param right Coordinate of the right clipping plane.
   * @param top Coordinate of the top clipping plane.
   * @param bottom Coordinate of the bottom clipping plane.
   * @param near Coordinate of the near clipping plane.
   * @param far Coordinate of the far clipping plane.
   *
   * @return The projection matrix.
   */
   mat4 orthoMatrix(double left, double right,
                    double top,  double bottom,
                    double near, double far);

  /**
   * @brief Perspective projection matrix specifying a view frustum.
   *
   * This projection matrix is the same as the OpenGL glFrustum() matrix.
   *
   * @param left The left side of the view frustum.
   * @param right The right side of the view frustum.
   * @param top The top side of the view frustum.
   * @param bottom The bottom side of the view frustum.
   * @param near The distance from the camera to the near plane.
   * @param far The distance from the camera to the far plane.
   *
   * @return The projection matrix.
   */
   mat4 frustumMatrix(double left, double right,
                      double top,  double bottom,
                      double near, double far);

  /**
   * @brief Perspective projection matrix specifying a view frustum.
   *
   * This projection matrix is the same as the OpenGL gluPerspective() matrix.
   *
   * @param fovY The field of view in the y direction.
   * @param aspect The aspect ratio.
   * @param near Coordinate for the near plane.
   * @param far Coordinate for the far plane.
   *
   * @return The projection matrix.
   */
  mat4 perspectiveMatrix(double fovY, double aspect, double near, double far);

  //@}

  //! @name Viewing Matrices
  //@{

  /**
   * @brief View matrix from eye to center with up direction.
   *
   * This view matrix is the same as the OpenGL gluLookAt() matrix.
   *
   * @param eyeX X coordinate for the eye position.
   * @param eyeY Y coordinate for the eye position.
   * @param eyeZ Z coordinate for the eye position.
   * @param centerX X coordinate for the center position.
   * @param centerY Y coordinate for the center position.
   * @param centerZ Z coordinate for the center position.
   * @param upX X coordinate for the up direction.
   * @param upY Y coordinate for the up direction.
   * @param upZ Z coordinate for the up direction.
   *
   * @return The look at matrix.
   */
  mat4 lookAtMatrix(double eyeX,    double eyeY,    double eyeZ,
                    double centerX, double centerY, double centerZ,
                    double upX,     double upY,     double upZ);

  //@}

  //! @name Modeling Matrices
  //@{

  mat4 translationMatrix(double x, double y, double z);

  mat4 xRotationMatrix(double angle);
  mat4 yRotationMatrix(double angle);
  mat4 zRotationMatrix(double angle);

  mat4 rotationMatrix(double angle, double x, double y, double z);

  mat4 scaleMatrix(double factor);
  mat4 scaleMatrix(double x, double y, double z);

  //@}

}

#endif
