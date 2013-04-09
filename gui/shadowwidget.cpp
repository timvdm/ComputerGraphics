#include "shadowwidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>

#include <libgfx/context_traits.h>

using namespace GFX;
using namespace ShaderFunctions;

int shadowMapSize = 100;

struct PerFragmentLightVertexShader
{
  typedef std::tuple<vec3, TexCoord, Real> varying_type;
    
  vec4 exec(const double *attributes, varying_type &varying)
  {
    using namespace ShaderFunctions;

    // position and normal in model coordinates
    vec4 mcPosition(attributes[0], attributes[1], attributes[2], 1.0);
    vec4 mcNormal(attributes[3], attributes[4], attributes[5], 0.0);

    // calculate and normalize eye space normal
    vec3 ecNormal(vec4to3(u_mv * mcNormal));
    //ecNormal.normalize();
    
    std::get<0>(varying) = ecNormal;
    std::get<1>(varying) = TexCoord(attributes[6], attributes[7]);
    std::get<2>(varying) = attributes[8];
    
    // transform the vertex position
    return u_mvp * mcPosition;
  }

  // uniforms
  mat4 u_mv; // model-view matrix (transform normals)
  mat4 u_mvp; // model-view-projection matrix (transform vertices)
};

struct PerFragmentLightFragmentShader
{
  typedef std::tuple<vec3, TexCoord, Real> varying_type;

  ColorF exec(const varying_type &varying, const std::vector<Texture> &textures, vec3 &pos, bool backFace = false)
  {
    using namespace ShaderFunctions;

    vec3 ecNormal = std::get<0>(varying);
    ecNormal.normalize();

    Real ecNormalDotLightDirection = max(0.0, dot(ecNormal, u_light.direction));

    // ambient light
    vec4 ambient = u_light.light.ambient.array() * u_material.ambient.array();

    // diffuse light
    vec4 diffuse = ecNormalDotLightDirection * u_light.light.diffuse.array() * u_material.diffuse.array();

    // shadow
    const TexCoord &uv = std::get<1>(varying);
    int quad = std::get<2>(varying) + 0.5;

    diffuse *= u_shadowMap(quad * shadowMapSize + uv.u * shadowMapSize, uv.v * shadowMapSize).r;

    // color
    vec4 color = ambient + diffuse;
    return ColorF(color.x(), color.y(), color.z());
  }

  DirectionalLight u_light;
  Material u_material;
  Buffer<Color> u_shadowMap;
};





struct ShadowMapVertexShader
{
  typedef std::tuple<TexCoord, Real> varying_type;
    
  vec4 exec(const double *attributes, varying_type &varying)
  {
    std::get<0>(varying) = TexCoord(attributes[3], attributes[4]);
    std::get<1>(varying) = attributes[5];
    //std::cout << vec4_string(u_mvp * vec4(attributes[0], attributes[1], attributes[2], 1.0)) << std::endl;
    return u_mvp * vec4(attributes[0], attributes[1], attributes[2], 1.0);
  }

  // uniforms
  mat4 u_mvp; // model-view-projection matrix (transform vertices)
};

struct ShadowMapFragmentShader
{
  typedef std::tuple<TexCoord, Real> varying_type;

  ColorF exec(const varying_type &varying, const std::vector<Texture> &textures, vec3 &pixel, bool backFace = false)
  {
    if (u_renderShadowMap) {
      const TexCoord &uv = std::get<0>(varying);
      int quad = std::get<1>(varying) + 0.5;
   
      if (std::abs(u_depthMap(pixel.x(), pixel.y()) - pixel.z()) < 0.01)
        u_shadowMap(quad * shadowMapSize + uv.u * shadowMapSize, uv.v * shadowMapSize) = Color::white();
    }

    return Color::white();
  }

  bool u_renderShadowMap;
  Buffer<Real> u_depthMap;
  Buffer<Color> u_shadowMap;
};




ShadowWidget::ShadowWidget(int width, int height, QWidget *parent) : GfxWidget(width, height, parent)
{
  setEyeZ(3);
}

void ShadowWidget::createShadowMap()
{
  Context ctx(context().width(), context().height());
  typedef context_traits<ShadowMapVertexShader, ShadowMapFragmentShader> T;
  //typedef context_traits<PerFragmentLightVertexShader, PerFragmentLightFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(ctx, program);

  // disable color buffer
  //ctx.disable(GFX_BUFFER, GFX_COLOR_BUFFER);
  // disable face culling
  ctx.set(GFX_FACE_CULLING, GFX_NO_FACE);

  // clear Z buffer
  ctx.clearZBuffer();

  // create transformation matrices
  mat4 view = lookAtMatrix(0, 0, 3, 0, 0, 0, 0, 1, 0);
  //mat4 project = orthoMatrix(-2, 2, -2, 2, 1.0, 10);
  Real aspect = static_cast<Real>(context().width()) / context().height();
  mat4 project = frustumMatrix(-aspect, aspect, -1, 1, 1.0, 10);
  mat4 trans = translationMatrix(0, 0, 0);



  // set vertex shader's matrices
  vec3 dir(0, 1, 0);
  Real theta = std::acos(dir.dot(vec3(0, 0, 1)));
  vec3 axis = dir.cross(vec3(0, 0, -1));
  mat4 rot = rotationMatrix(-theta, axis.x(), axis.y(), axis.z());

  vertexShader.u_mvp = project * view * rot;



  // x, y, z, u, v, i
  double ground[] = {
     1,  0, -1,  1,  0,  0,
    -1,  0, -1,  0,  0,  0,
    -1,  0,  1,  0,  1,  0,
     1,  0,  1,  1,  1,  0,

     2, -1, -2,  1,  0,  1,
    -2, -1, -2,  0,  0,  1,
    -2, -1,  2,  0,  1,  1,
     2, -1,  2,  1,  1,  1
  };
  
  fragmentShader.u_renderShadowMap = false;
  renderer.drawQuads(ground, 48, 6);
  updatePixmap(1, ctx.zBuffer());

  fragmentShader.u_renderShadowMap = true;
  fragmentShader.u_depthMap = ctx.zBuffer();
  fragmentShader.u_shadowMap = Buffer<Color>(2 * shadowMapSize + 1, shadowMapSize + 1);
  fragmentShader.u_shadowMap.clear(Color::black());
  renderer.drawQuads(ground, 48, 6);
  updatePixmap(2, fragmentShader.u_shadowMap);

  m_shadowMap = fragmentShader.u_shadowMap;
}

void ShadowWidget::render()
{
  typedef context_traits<PerFragmentLightVertexShader, PerFragmentLightFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(context(), program);

  createShadowMap();

  // clear color & Z buffers
  context().clearColorBuffer(Color::black());
  context().clearZBuffer();

  // setup light
  fragmentShader.u_light = DirectionalLight(vec3(0, 1, 1).normalized(), LightSource(vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4()));

  // create transformation matrices
  mat4 rotateX = rotationMatrix(-angleX(), 0, 1, 0);
  mat4 rotateY = rotationMatrix(angleY(), 1, 0, 0);
  mat4 project = frustumMatrix(-1, 1, -1, 1, 1.0, 10);
  mat4 view = lookAtMatrix(0, 0, eyeZ(), 0, 0, 0, 0, 1, 0);

  // set vertex shader's matrices
  vertexShader.u_mv = view * rotateX * rotateY;
  vertexShader.u_mvp = project * view * rotateX * rotateY;
  
  fragmentShader.u_shadowMap = m_shadowMap;

  // x, y, z, u, v, i
  double ground[] = {
     1,  0, -1,  0,  1,  0,  1,  0,  0,
    -1,  0, -1,  0,  1,  0,  0,  0,  0,
    -1,  0,  1,  0,  1,  0,  0,  1,  0,
     1,  0,  1,  0,  1,  0,  1,  1,  0,

     2, -1, -2,  0,  1,  0,  1,  0,  1,
    -2, -1, -2,  0,  1,  0,  0,  0,  1,
    -2, -1,  2,  0,  1,  0,  0,  1,  1,
     2, -1,  2,  0,  1,  0,  1,  1,  1
  };  
  
  fragmentShader.u_material = Material(vec4(0.2, 0.0, 0.0, 1.0), vec4(0.8, 0.0, 0.0, 1.0), vec4());
  renderer.drawQuads(ground, 36, 9);
  fragmentShader.u_material = Material(vec4(0.0, 0.2, 0.0, 1.0), vec4(0.0, 0.8, 0.0, 1.0), vec4());
  renderer.drawQuads(ground + 36, 36, 9);


  //updatePixmap(0, context().zBuffer());

  copyColorBufferToImage();
}
