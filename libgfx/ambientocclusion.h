#ifndef GFX_AMBIENTOCCLUSION_H
#define GFX_AMBIENTOCCLUSION_H

#include "types.h"

#include <vector>

namespace GFX {

  struct AmbientOcclusion
  {
    AmbientOcclusion(Real accessibility_, const vec3 &avgUnoccluded_)
      : accessibility(accessibility_), avgUnoccluded(avgUnoccluded_)
    {
    }

    Real accessibility;
    vec3 avgUnoccluded;
  };
  
  vec3 random_direction_in_halfplane(const vec3 &normal);
  bool ray_intersects_triangle(const vec3 &raySource, const vec3 &rayNormal, const vec3 &A, const vec3 &B, const vec3 &C);
  bool ray_intersects_sphere(const vec3 &raySource, const vec3 &rayNormal, const vec3 &center, Real radius);

  std::vector<AmbientOcclusion> ambient_occlusion(Real *attributes, std::size_t size, std::size_t stride);

}

#endif
