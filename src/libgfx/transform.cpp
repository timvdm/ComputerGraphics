#include "transform.h"
#include "spherecoord.h"

#include <cmath>

namespace GFX {

  mat4 projectionMatrix(double x, double y, double z)
  {
    SphereCoord eye(vec3(x, y, z));

    mat4 m(mat4::Identity());

    double st = std::sin(eye.theta);
    double ct = std::cos(eye.theta);
    double sp = std::sin(eye.phi);
    double cp = std::cos(eye.phi);

    m(0, 0) = - st;
    m(1, 0) = - ct * cp;
    m(2, 0) =   ct * sp;
    m(0, 1) =   ct;
    m(1, 1) = - st * cp;
    m(2, 1) =   st * sp;
    m(1, 2) =   sp;
    m(2, 2) =   cp;
    m(2, 3) = - eye.r;

    return m;
  }

  mat4 orthoMatrix(double l, double r, double t,  double b, double n, double f)
  {
    mat4 m(mat4::Identity());

    m(0, 0) = 2.0 / (r - l);
    m(0, 3) = -(r + l) / (r - l);
    m(1, 1) = 2.0 / (t - b);
    m(1, 3) = -(t + b) / (t - b);
    m(2, 2) = -2.0 / (f - n);
    m(2, 3) = -(f + n) / (f - n);

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

  mat4 perspectiveMatrix(double fovY, double aspect, double n, double f)
  {
    mat4 m(mat4::Zero());

    double c = std::cos(fovY / 2.0) / std::sin(fovY / 2.0);

    m(0, 0) = c / aspect;
    m(1, 1) = c;
    m(2, 2) = -(f + n) / (f - n);
    m(2, 3) = -2.0 * n * f / (f - n);
    m(3, 2) = -1.0;

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
    m(2, 1) = -s;
    m(1, 2) =  s;
    m(2, 2) =  c;

    return m;
  }

  mat4 yRotationMatrix(double angle)
  {
    mat4 m(mat4::Identity());

    double c = std::cos(angle);
    double s = std::sin(angle);

    m(0, 0) =  c;
    m(2, 0) =  s;
    m(0, 2) = -s;
    m(2, 2) =  c;

    return m;
  }

  mat4 zRotationMatrix(double angle)
  {
    mat4 m(mat4::Identity());

    double c = std::cos(angle);
    double s = std::sin(angle);

    m(0, 0) =  c;
    m(1, 0) = -s;
    m(0, 1) =  s;
    m(1, 1) =  c;

    return m;
  }

  mat4 rotationMatrix(double angle, double x, double y, double z)
  {
    vec3 axis(x, y, z);
    axis.normalize();

    x = axis.x();
    y = axis.y();
    z = axis.z();

    double c = std::cos(angle);
    double s = std::sin(angle);

    double xs = x * s;
    double ys = y * s;
    double zs = z * s;

    mat4 m(mat4::Identity());

    m(0, 0) = x * x * (1.0 - c) + c;
    m(0, 1) = x * y * (1.0 - c) - zs;
    m(0, 2) = x * z * (1.0 - c) + ys;
    m(1, 0) = y * x * (1.0 - c) + zs;
    m(1, 1) = y * y * (1.0 - c) + c;
    m(1, 2) = y * z * (1.0 - c) - xs;
    m(2, 0) = z * x * (1.0 - c) - ys;
    m(2, 1) = z * y * (1.0 - c) + xs;
    m(2, 2) = z * z * (1.0 - c) + c;

    return m;
  }

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
