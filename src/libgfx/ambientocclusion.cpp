#include "ambientocclusion.h"

#include "shaders.h"

#include <random>
#include <iostream>

namespace GFX {

  namespace impl {

    struct Triangle
    {
      Triangle(const vec3 &A_, const vec3 &B_, const vec3 &C_) : A(A_), B(B_), C(C_)
      {
      }

      vec3 A, B, C;
    };
  
  }

  bool ray_intersects_triangle(const vec3 &raySource, const vec3 &rayNormal, const vec3 &A, const vec3 &B, const vec3 &C)
  {
    // compute normal
    vec3 u = B - A;
    vec3 v = C - A;
    vec3 n = u.cross(v);

    // is triangle degenerate?
    if (n.norm() < 0.0001)
      return false;

    vec3 w0 = A - raySource;
    Real a = -n.dot(w0);
    Real b = n.dot(rayNormal);
    // ray parallel to triangle?
    if (std::abs(b) < 0.0001)
      return false;

    // compute ray/triangle plane intersection point
    Real r = a / b;
    // ray points away from triangle?
    if (r < 0.0)
      return false;

    // intersection point I
    vec3 I = raySource + r * rayNormal;

    Real uu = u.dot(u);
    Real vv = v.dot(v);
    Real uv = u.dot(v);
    vec3 w = I - A;
    Real wu = w.dot(u);
    Real wv = w.dot(v);

    Real denom = uv * uv - uu * vv;
    Real s = (uv * wv - vv * wu) / denom;
    if (s < 0.0 || s > 1.0)
      return false;
    Real t = (uv * wu - uu * wv) / denom;
    if (t < 0.0 || (s + t) > 1.0)
      return false;

    return true;
  }

  bool ray_intersects_sphere(const vec3 &raySource, const vec3 &rayNormal, const vec3 &center, Real radius)
  {
    using namespace ShaderFunctions;

    vec3 a = center - raySource;
    const vec3 &b = rayNormal;

    if (a.norm() > 4 * radius)
      return false;

    // check if sphere is in the right halfplane
    Real angle = a.dot(b);
    if (angle < 0.0)
      return false;
    
    vec3 r  = a -  a.dot(b) * b;

    if (r.norm() > radius)
      return false;

    return true;
  }

  vec3 random_direction_in_halfplane(const vec3 &normal)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    while (true) {
      vec3 dir(dis(gen), dis(gen), dis(gen));

      // ignore dirs outside unit
      if (dir.norm() > 1.0)
        continue;

      // ignore dirs in other half-plane
      if (dir.dot(normal) < 0)
        continue;

      dir.normalize();
      return dir;
    }
  }

  std::vector<AmbientOcclusion> ambient_occlusion(Real *attributes, std::size_t size, std::size_t stride)
  {
    assert((size % (stride * 3)) == 0);

    std::vector<impl::Triangle> triangles;
    for (std::size_t i = stride * 2; i < size; i += stride * 3)
      triangles.push_back(impl::Triangle(vec3(attributes[i - 2 * stride], attributes[i - 2 * stride + 1], attributes[i - 2 * stride + 2]), 
                                         vec3(attributes[i -     stride], attributes[i -     stride + 1], attributes[i -     stride + 2]), 
                                         vec3(attributes[i             ], attributes[i              + 1], attributes[i              + 2])));

    std::vector<AmbientOcclusion> result;

    int NUM_RAYS = 128;
    for (std::size_t i = 0; i < triangles.size(); ++i) {
      std::cout << i << "/" << triangles.size() << "\r";
      // compute normal
      vec3 u = triangles[i].B - triangles[i].A;
      vec3 v = triangles[i].C - triangles[i].A;
      vec3 n = u.cross(v);

      // compute triangle center
      vec3 G = (triangles[i].A + triangles[i].B + triangles[i].C) / 3.0;

      vec3 avgUnoccluded(vec3::Zero());
      int numUnoccluded = 0;
      for (int j = 0; j < NUM_RAYS; ++j) {
        vec3 ray = random_direction_in_halfplane(n);
        bool intersects = false;
        for (std::size_t k = 0; k < triangles.size(); ++k) {
          if (i == k)
            continue;
          if (ray_intersects_triangle(G, ray, triangles[k].A, triangles[k].B, triangles[k].C)) {
            intersects = true;
            break;
          }
        }

        if (!intersects) {
          avgUnoccluded += ray;
          ++numUnoccluded;
        }
      }

      avgUnoccluded.normalize();
      Real accessibility = static_cast<Real>(numUnoccluded) / NUM_RAYS;
      result.push_back(AmbientOcclusion(accessibility, avgUnoccluded));
    }

    return result;
  }

}
