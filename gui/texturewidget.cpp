#include "texturewidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>

#include <libgfx/context_traits.h>


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

  GFX::ColorF exec(const varying_type &varying, const std::vector<GFX::Texture> &textures, bool backFace = false)
  {
    const GFX::TexCoord &uv = std::get<0>(varying);
    return textures[u_activeTexture](uv.u, uv.v);
  }

  std::size_t u_activeTexture;
};



TextureWidget::TextureWidget(int width, int height, QWidget *parent) : GfxWidget(width, height, parent)
{
  context().createTexture("house.bmp3");
}

void TextureWidget::render()
{
  typedef GFX::context_traits<TextureVertexShader, TextureFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(context(), program);

  context().clearColorBuffer(GFX::Color::black());
  context().clearZBuffer();

  GFX::mat4 rotateX = GFX::rotationMatrix(-angleX(), 0, 1, 0);
  GFX::mat4 rotateY = GFX::rotationMatrix(angleY(), 1, 0, 0);

  GFX::mat4 project = GFX::frustumMatrix(-1, 1, -1, 1, 1.0, 10);
  GFX::mat4 view = GFX::lookAtMatrix(0, 0, eyeZ(), 0, 0, 0, 0, 1, 0);

  vertexShader.u_mvp = project * view * rotateX * rotateY;

  // x, y, z, u, v
  double walls[] = {
    // front
    -2, -1, -1,       0, 0.5,
     2, -1, -1, 2 / 3.0, 0.5,
     2,  1, -1, 2 / 3.0,   1,

    -2, -1, -1,       0, 0.5,
     2,  1, -1, 2 / 3.0,   1,
    -2,  1, -1,       0,   1,

    // right
     2, -1, -1, 2 / 3.0, 0.5,
     2, -1,  1,       1, 0.5,
     2,  1,  1,       1,   1,
     
     2, -1, -1, 2 / 3.0, 0.5,
     2,  1,  1,       1,   1,
     2,  1, -1, 2 / 3.0,   1,

    // back
     2, -1,  1, 1 / 3.0, 0.5,
    -2, -1,  1,       1, 0.5,
    -2,  1,  1,       1,   1,

     2, -1,  1, 1 / 3.0, 0.5,
    -2,  1,  1,       1,   1,
     2,  1,  1, 1 / 3.0,   1,

    // left
    -2, -1,  1, 2 / 3.0, 0.5,
    -2, -1, -1,       1, 0.5,
    -2,  1, -1,       1,   1,

    -2, -1,  1, 2 / 3.0, 0.5,
    -2,  1, -1,       1,   1,
    -2,  1,  1, 2 / 3.0,   1,

    // left triangle
    -2,  1,  1, 2 / 3.0,  0.5,
    -2,  1, -1,       1,  0.5,
    -2,  2,  0, 5 / 6.0, 0.75,

    // right triangle
     2,  1, -1, 2 / 3.0,  0.5,
     2,  1,  1,       1,  0.5,
     2,  2,  0, 5 / 6.0, 0.75
  };

  double roof[] = {
    -2,  1, -1, 1 / 3.0,   0,
     2,  1, -1,       1,   0,
     2,  2,  0,       1, 0.5,
    -2,  2,  0, 1 / 3.0, 0.5,

     2,  1,  1, 1 / 3.0,   0,
    -2,  1,  1,       1,   0,
    -2,  2,  0,       1, 0.5,
     2,  2,  0, 1 / 3.0, 0.5
  };

  double ground[] = {
    -3, -1, -3,       0, 0.5,
     3, -1, -3, 1 / 3.0, 0.5,
     3, -1,  3, 1 / 3.0,   0,
    -3, -1,  3,       0,   0
  };


  // render walls
  fragmentShader.u_activeTexture = 0;
  renderer.drawTriangles(walls, 150, 5);

  // render roof
  //fragmentShader.u_activeTexture = 1;
  renderer.drawQuads(roof, 40, 5);

  // render ground
  //fragmentShader.u_activeTexture = 2;
  renderer.drawQuads(ground, 20, 5);


  /*
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j) {
      GFX::mat4 save = vertexShader.u_mvp;
      vertexShader.u_mvp *= GFX::translationMatrix(6 * i - 12, 0, 6 * j - 12);

      // render walls
      fragmentShader.u_activeTexture = 0;
      renderer.drawTriangles(walls, 150, 5);

      // render roof
      fragmentShader.u_activeTexture = 1;
      renderer.drawQuads(roof, 40, 5);

      // render ground
      fragmentShader.u_activeTexture = 2;
      renderer.drawQuads(ground, 20, 5);

      vertexShader.u_mvp = save;
    }
  */

  copyColorBufferToImage();
}



