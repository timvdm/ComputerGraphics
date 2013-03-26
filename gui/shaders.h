#include "gfxwidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>



struct SimpleVertexShader
{
  typedef std::tuple<GFX::Color> varying_type;

  GFX::vec4 exec(const double *attributes, varying_type &varying)
  {
    std::get<0>(varying) = GFX::Color(attributes[3], attributes[4], attributes[5]);
    // transform the vertex
    GFX::vec4 position(attributes[0], attributes[1], attributes[2], 1.0);
    return transform * position;
  }

  GFX::mat4 transform;
};

struct SimpleFragmentShader
{
  typedef std::tuple<GFX::Color> varying_type;

  GFX::Color exec(const varying_type &varying, bool backFace = false)
  {
    if (backFace)
      return GFX::Color(200, 200, 200);
    return std::get<0>(varying);
  }
};



// 
// Uniform:
//
//   transform (mat4)
//
// Attribute:
// 
//   0-2: vertex xyz
//   3-5: normal xyz
//   6-9: color rgba
//
// Varying:
//
//   0: normal (vec4)
//   1: color (Color)
//

struct LightVertexShader
{
  typedef std::tuple<GFX::Color> varying_type;
    
  GFX::vec4 exec(const double *attributes, varying_type &varying)
  {
    using namespace GFX::ShaderFunctions;

    // position and normal in model coordinates
    GFX::vec4 mcPosition(attributes[0], attributes[1], attributes[2], 1.0);
    GFX::vec4 mcNormal(attributes[3], attributes[4], attributes[5], 0.0);

    // calculate and normalize eye space normal
    GFX::vec3 ecNormal(vec4to3(u_mv * mcNormal));
    ecNormal.normalize();

    // do light calculation
    double ecNormalDotLightDirection = max(0.0, dot(ecNormal, u_light.direction));

    // ambient light
    GFX::vec4 ambient = u_light.light.ambient.array() * u_material.ambient.array();

    // diffuse light
    GFX::vec4 diffuse = ecNormalDotLightDirection * u_light.light.diffuse.array() * u_material.diffuse.array();

    // color
    GFX::vec4 color = 255 * (ambient + diffuse);
    std::get<0>(varying) = GFX::Color(color.x(), color.y(), color.z());
    // transform the vertex position
    return u_mvp * mcPosition;
  }

  // uniforms
  GFX::mat4 u_mv; // model-view matrix (transform normals)
  GFX::mat4 u_mvp; // model-view-projection matrix (transform vertices)
  GFX::DirectionalLight u_light;
  GFX::Material u_material;
};

struct LightFragmentShader
{
  typedef std::tuple<GFX::Color> varying_type;

  GFX::Color exec(const varying_type &varying, bool backFace = false)
  {
    if (backFace)
      return GFX::Color(200, 200, 200);
    return std::get<0>(varying);
  }
};




struct TextureVertexShader
{
  typedef std::tuple<GFX::TexCoord> varying_type;
    
  GFX::vec4 exec(const double *attributes, varying_type &varying)
  {
    using namespace GFX::ShaderFunctions;

    // position and normal in model coordinates
    GFX::vec4 mcPosition(attributes[0], attributes[1], attributes[2], 1.0);

    std::get<0>(varying) = GFX::TexCoord(attributes[3], attributes[4]);
    // transform the vertex position
    return u_mvp * mcPosition;
  }

  // uniforms
  GFX::mat4 u_mvp; // model-view-projection matrix (transform vertices)
};

struct TextureFragmentShader
{
  typedef std::tuple<GFX::TexCoord> varying_type;

  GFX::Color texel(double u, double v) const
  {
    bool i = static_cast<int>(u * 10) % 2;
    bool j = static_cast<int>(v * 10) % 2;
    if (i == j)
      return GFX::Color::white();
    return GFX::Color::blue();
  }

  GFX::Color exec(const varying_type &varying, bool backFace = false)
  {
    const GFX::TexCoord &uv = std::get<0>(varying);
    return texel(uv.u, uv.v);
  }

};

