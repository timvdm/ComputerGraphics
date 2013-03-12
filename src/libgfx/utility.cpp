#include "utility.h"
#include "spherecoord.h"

namespace GFX {

  mat4 projectionMatrix(double x, double y, double z)
  {
    SphereCoord eye(vec3(x, y, z));

    mat4 m(mat4::Identity());

    m(0, 0) = - std::sin(eye.theta);
    m(0, 1) = - std::cos(eye.theta) * std::cos(eye.phi);
    m(0, 2) =   std::cos(eye.theta) * std::sin(eye.phi);
    m(1, 0) =   std::cos(eye.theta);
    m(1, 1) = - std::sin(eye.theta) * std::cos(eye.phi);
    m(1, 2) =   std::sin(eye.theta) * std::sin(eye.phi);
    m(2, 1) =   std::sin(eye.phi);
    m(2, 2) =   std::cos(eye.phi);
    m(3, 2) = - eye.r;

    return m;
  }

  mat4 lookAtMatrix(double eyeX,    double eyeY,    double eyeZ,
                    double centerX, double centerY, double centerZ,
                    double upX,     double upY,     double upZ)
  {
    mat4 m;

    vec3 n(eyeX - centerX, eyeY - centerY, eyeZ - centerZ);
    n.normalize();

    vec3 s = vec3(upX, upY, upZ).cross(n).normalized();

    vec3 u = n.cross(s);

    m(0, 0) = s.x();
    m(0, 1) = s.y();
    m(0, 2) = s.z();
    m(0, 3) = -eyeX;

    m(1, 0) = u.x();
    m(1, 1) = u.y();
    m(1, 2) = u.z();
    m(1, 3) = -eyeY;

    m(2, 0) = n.x();
    m(2, 1) = n.y();
    m(2, 2) = n.z();
    m(2, 3) = -eyeZ;

    m(3, 0) = 0.0;
    m(3, 1) = 0.0;
    m(3, 2) = 0.0;
    m(3, 3) = 1.0;

    return m;
  }
 
  mat4 frustumMatrix(double l, double r, double t, double b, double n, double f)
  {
    mat4 m;

    m(0, 0) = 2.0 / (r - l);
    m(0, 1) = 0.0;
    m(0, 2) = (r + l) / (r - l);
    m(0, 3) = 0.0;

    m(1, 0) = 0.0;
    m(1, 1) = 2.0 / (t - b);
    m(1, 2) = (t + b) / (t - b);
    m(1, 3) = 0.0;

    m(2, 0) = 0.0;
    m(2, 1) = 0.0;
    m(2, 2) = -(f + n) / (f - n);
    m(2, 3) = -2.0 * f * n / (f - n);

    m(3, 0) = 0.0;
    m(3, 1) = 0.0;
    m(3, 2) = -1.0;
    m(3, 3) = 0.0;

    return m;
  }
  
  mat4 translationMatrix(double x, double y, double z)
  {
    mat4 m(mat4::Identity());

    m(0, 3) = x;
    m(1, 3) = y;
    m(2, 3) = z;

    return m;
  }

  mat4 xRotationMatrix(double angle)
  {
    mat4 m(mat4::Identity());

    double c = std::cos(angle);
    double s = std::sin(angle);

    m(1, 1) =  c;
    m(1, 2) = -s;
    m(2, 1) =  s;
    m(2, 2) =  c;

    return m;
  }

  mat4 yRotationMatrix(double angle)
  {
    mat4 m(mat4::Identity());

    double c = std::cos(angle);
    double s = std::sin(angle);

    m(0, 0) =  c;
    m(0, 2) =  s;
    m(2, 0) = -s;
    m(2, 2) =  c;

    return m;
  }

  mat4 zRotationMatrix(double angle)
  {
    mat4 m(mat4::Identity());

    double c = std::cos(angle);
    double s = std::sin(angle);

    m(0, 0) =  c;
    m(0, 1) = -s;
    m(1, 0) =  s;
    m(1, 1) =  c;

    return m;
  }
  
  /*
  mat4 rotationMatrix(double angle, double x, double y, double z)
  {
    axis.normalise();
    double c = std::cos(angle);
    double s = std::sin(angle);

    double xs = axis.x * s;
    double ys = axis.y * s;
    double zs = axis.z * s;

    Matrix m;

    m(1, 1) = axis.x * axis.x * (1.0 - c) + c;
    m(1, 2) = axis.x * axis.y * (1.0 - c) - zs;
    m(1, 3) = axis.x * axis.z * (1.0 - c) + ys;
    m(2, 1) = axis.y * axis.x * (1.0 - c) + zs;
    m(2, 2) = axis.y * axis.y * (1.0 - c) + c;
    m(2, 3) = axis.y * axis.z * (1.0 - c) - xs;
    m(3, 1) = axis.z * axis.x * (1.0 - c) - ys;
    m(3, 2) = axis.z * axis.y * (1.0 - c) + xs;
    m(3, 3) = axis.z * axis.z * (1.0 - c) + c;

    return m;
  }
  */

  mat4 scaleMatrix(double factor)
  {
    mat4 m(mat4::Identity());

    m(0, 0) = factor;
    m(1, 1) = factor;
    m(2, 2) = factor;

    return m;
  }

  mat4 scaleMatrix(double x, double y, double z)
  {
    mat4 m(mat4::Identity());

    m(0, 0) = x;
    m(1, 1) = y;
    m(2, 2) = z;

    return m;
  }


}
