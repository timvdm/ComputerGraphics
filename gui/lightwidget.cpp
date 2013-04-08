#include "lightwidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>

#include <libgfx/context_traits.h>

// 
// Uniform:
//
//   model-view matrix (mat4)
//   model-view-proejection matrix (mat4)
//
// Attribute:
// 
//   0-2: vertex xyz
//   3-5: normal xyz
//
// Varying:
//
//   0: color (Color)
//

struct PerVertexLightVertexShader
{
  typedef std::tuple<GFX::ColorF> varying_type;
    
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
    GFX::vec4 color = ambient + diffuse;
    std::get<0>(varying) = GFX::ColorF(color.x(), color.y(), color.z());
    
    // transform the vertex position
    return u_mvp * mcPosition;
  }

  // uniforms
  GFX::mat4 u_mv; // model-view matrix (transform normals)
  GFX::mat4 u_mvp; // model-view-projection matrix (transform vertices)
  GFX::DirectionalLight u_light;
  GFX::Material u_material;
};

struct PerVertexLightFragmentShader
{
  typedef std::tuple<GFX::ColorF> varying_type;

  GFX::ColorF exec(const varying_type &varying, const std::vector<GFX::Texture> &textures, GFX::vec3 &pos, bool backFace = false)
  {
    return std::get<0>(varying);
  }
};

// 
// Uniform:
//
//   model-view matrix (mat4)
//   model-view-proejection matrix (mat4)
//
// Attribute:
// 
//   0-2: vertex xyz
//   3-5: normal xyz
//
// Varying:
//
//   0: normal (vec4)
//

struct PerFragmentLightVertexShader
{
  typedef std::tuple<GFX::vec3> varying_type;
    
  GFX::vec4 exec(const double *attributes, varying_type &varying)
  {
    using namespace GFX::ShaderFunctions;

    // position and normal in model coordinates
    GFX::vec4 mcPosition(attributes[0], attributes[1], attributes[2], 1.0);
    GFX::vec4 mcNormal(attributes[3], attributes[4], attributes[5], 0.0);

    // calculate and normalize eye space normal
    GFX::vec3 ecNormal(vec4to3(u_mv * mcNormal));
    //ecNormal.normalize();

    std::get<0>(varying) = ecNormal;
    
    // transform the vertex position
    return u_mvp * mcPosition;
  }

  // uniforms
  GFX::mat4 u_mv; // model-view matrix (transform normals)
  GFX::mat4 u_mvp; // model-view-projection matrix (transform vertices)
};

struct PerFragmentLightFragmentShader
{
  typedef std::tuple<GFX::vec3> varying_type;

  GFX::ColorF exec(const varying_type &varying, const std::vector<GFX::Texture> &textures, GFX::vec3 &pos, bool backFace = false)
  {
    using namespace GFX::ShaderFunctions;

    GFX::vec3 ecNormal = std::get<0>(varying);
    ecNormal.normalize();

    GFX::Real ecNormalDotLightDirection = max(0.0, dot(ecNormal, u_light.direction));

    // ambient light
    GFX::vec4 ambient = u_light.light.ambient.array() * u_material.ambient.array();

    // diffuse light
    GFX::vec4 diffuse = ecNormalDotLightDirection * u_light.light.diffuse.array() * u_material.diffuse.array();

    // color
    GFX::vec4 color = ambient + diffuse;
    return GFX::ColorF(color.x(), color.y(), color.z());
  }

  GFX::DirectionalLight u_light;
  GFX::Material u_material;
};






LightWidget::LightWidget(int width, int height, QWidget *parent) : GfxWidget(width, height, parent)
{
  setEyeZ(3);

  std::shared_ptr<GFX::Mesh> sphere = GFX::Mesh::sphere(2);
  sphere->triangulate();
  sphere->computeNormals(true);
  int numTriangles = sphere->faces().size();

  m_attributes = sphere->triangleAttributes(true);
}

void LightWidget::render()
{
  //typedef GFX::context_traits<PerVertexLightVertexShader, PerVertexLightFragmentShader> T;
  typedef GFX::context_traits<PerFragmentLightVertexShader, PerFragmentLightFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(context(), program);

  // clear color & Z buffers
  context().clearColorBuffer(GFX::Color::black());
  context().clearZBuffer();

  // setup light
  //vertexShader.u_material = GFX::Material(GFX::vec4(0.2, 0.0, 0.0, 0.0), GFX::vec4(0.8, 0.0, 0.0, 0.0), GFX::vec4());
  //vertexShader.u_light = GFX::DirectionalLight(GFX::vec3(0, 1, -1).normalized(), GFX::LightSource(GFX::vec4(1.0, 0.0, 0.0, 0.0), GFX::vec4(1.0, 0.0, 0.0, 0.0), GFX::vec4()));
  fragmentShader.u_material = GFX::Material(GFX::vec4(0.2, 0.0, 0.0, 0.0), GFX::vec4(0.8, 0.0, 0.0, 0.0), GFX::vec4());
  fragmentShader.u_light = GFX::DirectionalLight(GFX::vec3(0, 1, 1).normalized(), GFX::LightSource(GFX::vec4(1.0, 0.0, 0.0, 0.0), GFX::vec4(1.0, 0.0, 0.0, 0.0), GFX::vec4()));

  // create transformation matrices
  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX(), 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY(), 1, 0, 0);
  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.0, 10);
  GFX::mat4 view = GFX::lookAtMatrix(0, 0, eyeZ(), 0, 0, 0, 0, 1, 0);

  // set vertex shader's matrices
  vertexShader.u_mv = view * rotateX * rotateY;
  vertexShader.u_mvp = project * view * rotateX * rotateY;

  // draw the sphere
  renderer.drawTriangles(&m_attributes[0], m_attributes.size(), 6);

  /*
  double ground[] = {
     2, -1,  2,  0,  1,  0,
    -2, -1,  2,  0,  1,  0,
    -2, -1, -2,  0,  1,  0,
     2, -1, -2,  0,  1,  0
  };
  
  renderer.drawQuads(ground, 24, 6);
  */


  copyColorBufferToImage();
}
