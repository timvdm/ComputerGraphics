#ifndef GFX_SPHERECOORD_H
#define GFX_SPHERECOORD_H

#include "utility.h"

#include <vector>

namespace GFX {

  struct SphereCoord
  {
    SphereCoord(Real theta_, Real phi_, Real r_) : theta(theta_), phi(phi_), r(r_)
    {
    }

    SphereCoord(const vec3 &v)
    {
      r = std::sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
      theta = std::atan2(v.y(), v.x());
      phi = std::acos(v.z() / r);
    }

    template<typename T>
    SphereCoord(const std::vector<T> &v)
    {
      *this = SphereCoord(vec3(v[0], v[1], v[2]));
    }

    operator vec3() const
    {
      return vec3(r * std::sin(theta) * std::cos(phi),
                  r * std::sin(theta) * std::sin(phi),
                  r * std::cos(theta));
    }

    Real theta;
    Real phi;
    Real r;
  };

}

#endif
