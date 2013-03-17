#ifndef GFX_LIGHTING_H
#define GFX_LIGHTING_H

#include "types.h"

namespace GFX {

  struct LightSource
  {
    LightSource() : ambient(0.2, 0.2, 0.2, 0.0), diffuse(0.8, 0.8, 0.8, 0.0),
        specular(0.0, 0.0, 0.0, 0.0)
    {
    }

    LightSource(const vec4 &ambient_, const vec4 &diffuse_, const vec4 &specular_)
      : ambient(ambient_), diffuse(diffuse_), specular(specular_)
    {
    }

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
  };

  struct DirectionalLight
  {
    DirectionalLight() : direction(0, 1, 0)
    {
    }

    DirectionalLight(const vec3 &direction_, const LightSource &light_)
      : direction(direction_), light(light_)
    {
    }

    vec3 direction;
    LightSource light;
  };

  struct Material
  {
    Material() : ambient(0.2, 0.2, 0.2, 0.0), diffuse(0.8, 0.8, 0.8, 0.0),
        specular(0.0, 0.0, 0.0, 0.0)
    {
    }

    Material(const vec4 &ambient_, const vec4 &diffuse_, const vec4 &specular_)
      : ambient(ambient_), diffuse(diffuse_), specular(specular_)
    {
    }

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
  };

}

#endif
