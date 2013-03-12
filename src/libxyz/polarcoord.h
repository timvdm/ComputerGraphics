#ifndef CG_POLARCOORD_H
#define CG_POLARCOORD_H

#include "../utils.h"

#include <cmath>

namespace CG {

  namespace XYZ {

    struct PolarCoord
    {
      PolarCoord(double theta_, double phi_, double r_) : theta(theta_), phi(phi_), r(r_)
      {
      }

      PolarCoord(const Vector3D &v)
      {
        r = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        theta = std::atan2(v.y, v.x);
        phi = std::acos(v.z / r);
      }

      template<typename T>
      PolarCoord(const std::vector<T> &v)
      {
        *this = PolarCoord(Vector3D::point(v[0], v[1], v[2]));
      }


      double theta;
      double phi;
      double r;
    };

  }

}

#endif
