#include "pdbwidget.h"

#include <libgfx/program.h>
#include <libgfx/render3d.h>
#include <libgfx/transform.h>
#include <libgfx/mesh.h>

#include <libgfx/lighting.h>
#include <libgfx/shaders.h>

#include <libgfx/context_traits.h>

#include <fstream>
#include <random>

using namespace GFX;

namespace chemistry {

  struct Atom
  {
    Atom()
    {
    }

    Atom (Real x_, Real y_, Real z_, int element_) : x(x_), y(y_), z(z_), element(element_)
    {
    }

    Real x;
    Real y;
    Real z;
    int element;
  };

  int symbol2element(char symbol)
  {
    switch (symbol) {
      case 'C':
        return 6;
      case 'N':
        return 7;
      case 'O':
        return 8;
      case 'S':
        return 16;
      default:
        assert(0);
    }

    return 0;
  }

  Real vdw_radius(int element)
  {
    switch (element) {
      case 6:
        return 1.70;
      case 7:
        return 1.55;
      case 8:
        return 1.52;
      case 16:
        return 1.80;
      default:
        assert(0);
    }

    return 1.50;
  }

  vec4 atom_color(int element)
  {
    switch (element) {
      case 6:
        return vec4(0.5, 0.5, 0.5, 1.0);
      case 7:
        return vec4(0.0, 0.0, 1.0, 1.0);
      case 8:
        return vec4(1.0, 0.0, 0.0, 1.0);
      case 16:
        return vec4(1.0, 1.0, 0.0, 1.0);
      default:
        assert(0);
    }

    return vec4(0.0, 1.0, 0.0, 1.0);
  }


  Real parse_real(const std::string &number)
  {
    Real real;
    std::stringstream ss(number);
    ss >> real;
    return real;
  }

  std::vector<Atom> parse_pdb(const std::string &filename)
  {
    std::ifstream ifs(filename.c_str());

    if (!ifs) {
      std::cerr << "Could not open " << filename << " for reading." << std::endl;
      return std::vector<Atom>();
    }

    std::string line;
    std::vector<Atom> atoms;
    while (std::getline(ifs, line)) {
      if (line.substr(0, 4) != "ATOM")
        continue;

      //std::cout << line << std::endl;
      atoms.resize(atoms.size() + 1);
      Atom &atom = atoms.back();
      atom.element = symbol2element(line[13]);
      atom.x = parse_real(line.substr(31, 7));
      atom.y = parse_real(line.substr(39, 7));
      atom.z = parse_real(line.substr(47, 7));
    }

    return atoms;
  }

}

struct VertexShader
{
  // s, t, radius, eyePosition
  typedef std::tuple<Real, Real, Real, vec4, Real> varying_type;

  vec4 exec(const double *attributes, varying_type &varying)
  {
    using namespace ShaderFunctions;

    // project sphere center using model-view matrix
    vec4 &pos = std::get<3>(varying);
    pos = u_mv * vec4(attributes[0], attributes[1], attributes[2], 1.0);

    // get references to varyings
    Real &s = std::get<0>(varying);
    Real &t = std::get<1>(varying);
    Real &radius = std::get<2>(varying);

    // assign attribute values to varyings
    s = attributes[3];
    t = attributes[4];
    radius = attributes[5];
    std::get<4>(varying) = attributes[6];

    // displace quad vertex
    pos.x() += s * radius;
    pos.y() += t * radius;

    return u_p * pos;
  }

  // uniforms
  mat4 u_mv; // model-view matrix
  mat4 u_p; // projection matrix
};

/**
 * This shader computes the sphere impostor's z value only.
 */
struct DepthMapFragmentShader
{
  // s, t, radius, eyePosition
  typedef std::tuple<Real, Real, Real, vec4, Real> varying_type;

  ColorF exec(const varying_type &varying, const std::vector<Texture> &textures, vec3 &pixel, bool backFace = false)
  {
    using namespace ShaderFunctions;

    // get varyings
    Real s = std::get<0>(varying);
    Real t = std::get<1>(varying);

    // check to make sure we are inside the sphere
    Real distFromCenter2 = 1.0 - s * s - t * t;
    if (distFromCenter2 <= 0.0)
      return Color::none();

    Real distFromCenter = std::sqrt(distFromCenter2);
    Real radius = std::get<2>(varying);
    vec4 pos = std::get<3>(varying);

    // compute depth value
    pos.z() += distFromCenter * radius;
    pos = u_p * pos;
    pixel.z() = (pos.z() / pos.w() + 1.0) / 2.0;

    return Color::white();
  }

  mat4 u_p; // projection matrix
};

struct AmbientOcclusionMapFragmentShader
{
  // s, t, radius, eyePosition
  typedef std::tuple<Real, Real, Real, vec4, Real> varying_type;

  AmbientOcclusionMapFragmentShader(const Buffer<Real> &depthMap, Buffer<ColorF> &aoMap) : u_depthMap(depthMap), u_aoMap(aoMap)
  {
  }

  Real sign(Real value) const
  {
    if (value < 0.0)
      return -1.0;
    return 1.0;
  }

  GFX::ColorF exec(const varying_type &varying, const std::vector<GFX::Texture> &textures, GFX::vec3 &pixel, bool backFace = false)
  {
    using namespace ShaderFunctions;

    // get varyings
    Real s = std::get<0>(varying);
    Real t = std::get<1>(varying);

    // check to make sure we are inside the sphere
    Real distFromCenter2 = 1.0 - s * s - t * t;
    if (distFromCenter2 <= 0.0)
      return Color::none();

    Real distFromCenter = std::sqrt(distFromCenter2);
    Real radius = std::get<2>(varying);
    vec4 pos = std::get<3>(varying);

    // normal
    vec3 N(s, t, distFromCenter);

    // compute depth value
    pos.z() += N.z() * radius;
    pos = u_p * pos;
    pixel.z() = (pos.z() / pos.w() + 1.0) / 2.0;

    if (std::abs(u_depthMap(pixel.x(), pixel.y()) - pixel.z()) < 0.01) {
      // texture uv lookup
      vec4 aoNormal(s, t, distFromCenter, 0.0);
      aoNormal = u_inv_mv * aoNormal;
      aoNormal.z() = -aoNormal.z();

      // see Tarini et. al. page 3, eq. (4)
      Real d = std::abs(aoNormal.x()) + std::abs(aoNormal.y()) + std::abs(aoNormal.z());
      TexCoord uv;
      if (aoNormal.z() <= 0.0) {
        uv.u = aoNormal.x() / d;
        uv.v = aoNormal.y() / d;
      } else {
        uv.u = sign(aoNormal.x()) * (1.0 - std::abs(aoNormal.y()) / d);
        uv.v = sign(aoNormal.y()) * (1.0 - std::abs(aoNormal.x()) / d);
      }
      // translate/scale from [-1, 1] to [0, 1]
      uv.u = 0.5 + 0.5 * uv.u;
      uv.v = 0.5 + 0.5 * uv.v;

      // scale [0,1] to u_aoMap size
      Real u = (u_aoMap.height() - 1) * uv.u;
      Real v = (u_aoMap.height() - 1) * uv.v;

      // interpolate texel color from 4 surrounding texel
      int atom = std::get<4>(varying) + 0.5;
      int offset = atom * u_aoMap.height();
      int u1 = offset + static_cast<int>(u + 0.5) % u_aoMap.height();

      u_aoMap(u1, v + 0.5).r = 1.0;
    }

    return Color::white();
  }

  mat4 u_inv_mv;
  mat4 u_p; // projection matrix
  const Buffer<Real> &u_depthMap;
  Buffer<ColorF> &u_aoMap;
};




struct FragmentShader
{
  // s, t, radius, eyePosition
  typedef std::tuple<Real, Real, Real, vec4, Real> varying_type;

  FragmentShader(const Buffer<ColorF> &aoMap) : u_aoMap(aoMap)
  {
  }

  Real sign(Real value) const
  {
    if (value < 0.0)
      return -1.0;
    return 1.0;
  }

  GFX::ColorF exec(const varying_type &varying, const std::vector<GFX::Texture> &textures, GFX::vec3 &pixel, bool backFace = false)
  {
    using namespace ShaderFunctions;

    // get varyings
    Real s = std::get<0>(varying);
    Real t = std::get<1>(varying);
    Real radius = std::get<2>(varying);

    // check to make sure we are inside the sphere
    Real distFromCenter2 = 1.0 - s * s - t * t;
    if (distFromCenter2 <= -u_zeta)
      return Color::none();
    if (distFromCenter2 <= 0.0) {
      pixel.z() += u_eta * (u_zeta - radius);
      return Color::black();
    }

    Real distFromCenter = std::sqrt(distFromCenter2);
    vec4 pos = std::get<3>(varying);

    // normal
    vec3 N(s, t, distFromCenter);
    // light direction
    const vec3 &L = u_light.direction;
    // eye direction
    vec3 E(0, 0, 1);
    vec3 H((L + E).normalized());

    // diffuse factor
    Real df = std::max(0.0, N.dot(L));
    // specular factor
    Real sf = std::pow(std::max(0.0, N.dot(H)), 50.0);

    // compute depth value
    pos.z() += N.z() * radius;
    pos = u_p * pos;
    pixel.z() = (pos.z() / pos.w() + 1.0) / 2.0;
    
    Real accessibility = 1.0;

    if (u_aoOnly || u_aoEnabled) {
      // texture uv lookup
      vec4 aoNormal(s, t, distFromCenter, 0.0);
      aoNormal = u_inv_mv * aoNormal;
      aoNormal.z() = -aoNormal.z();

      // see Tarini et. al. page 3, eq. (4)
      Real d = std::abs(aoNormal.x()) + std::abs(aoNormal.y()) + std::abs(aoNormal.z());
      TexCoord uv;
      if (aoNormal.z() <= 0.0) {
        uv.u = aoNormal.x() / d;
        uv.v = aoNormal.y() / d;
      } else {
        uv.u = sign(aoNormal.x()) * (1.0 - std::abs(aoNormal.y()) / d);
        uv.v = sign(aoNormal.y()) * (1.0 - std::abs(aoNormal.x()) / d);
      }
      // translate/scale from [-1, 1] to [0, 1]
      uv.u = 0.5 + 0.5 * uv.u;
      uv.v = 0.5 + 0.5 * uv.v;

      // scale [0,1] to u_aoMap size
      Real u = (u_aoMap.height() - 1) * uv.u; // note: sould work with -1 too, but for some reason there is
      Real v = (u_aoMap.height() - 1) * uv.v; //       a column of black pixels between each ambient occlussion map

      // interpolate texel color from 4 surrounding texel
      int atom = std::get<4>(varying) + 0.5;
      int offset = atom * u_aoMap.height();
      int u1 = offset + static_cast<int>(u) % u_aoMap.height();
      int u2 = offset + static_cast<int>(u + 1) % u_aoMap.height();
      int v1 = static_cast<int>(v) % u_aoMap.height();
      int v2 = static_cast<int>(v + 1) % u_aoMap.height();

      const ColorF &c1 = u_aoMap(u1, v1);
      const ColorF &c2 = u_aoMap(u2, v1);
      const ColorF &c3 = u_aoMap(u1, v2);
      const ColorF &c4 = u_aoMap(u2, v2);

      Real alpha = u - std::floor(u);
      Real beta = v - std::floor(v);

      ColorF color1(c1.r * (1.0 - alpha) + c2.r * alpha,
                    c1.g * (1.0 - alpha) + c2.g * alpha,
                    c1.b * (1.0 - alpha) + c2.b * alpha);

      ColorF color2(c3.r * (1.0 - alpha) + c4.r * alpha,
                    c3.g * (1.0 - alpha) + c4.g * alpha,
                    c3.b * (1.0 - alpha) + c4.b * alpha);

      ColorF ao(color1.r * (1.0 - beta) + color2.r * beta,
                color1.g * (1.0 - beta) + color2.g * beta,
                color1.b * (1.0 - beta) + color2.b * beta);

      if (u_aoOnly)
        return ao;

      accessibility = ao.r;
    }

    // compute color
    vec4 ambient = 0.1 * u_material.ambient;
    vec4 diffuse = 0.9 * u_material.diffuse;
    vec4 specular = 0.4 * u_material.specular;
    vec4 color = ambient + accessibility * df * diffuse;// + accessibility * sf * specular;
    //vec4 color = ambient + df * diffuse;// + accessibility * sf * specular;


    return ColorF(color.x(), color.y(), color.z());
  }

  Material u_material;
  DirectionalLight u_light;
  mat4 u_inv_mv;
  mat4 u_p; // projection matrix
  const Buffer<ColorF> &u_aoMap;
  Real u_zeta;
  Real u_eta;
  bool u_aoOnly;
  bool u_aoEnabled;
};


void mirror_texels(Buffer<ColorF> &buffer)
{
  assert((buffer.height() % 2) == 0);
  assert((buffer.width() % buffer.height()) == 0);

  int size = buffer.height();
  int n = buffer.width() / size;
  for (int i = 0; i < n; ++i) {
    int offset = i * size;
    // left edge
    for (int y = 0; y < size / 2; ++y)
      buffer(offset, y) = buffer(offset, size - 1 - y);
    // right edge
    for (int y = 0; y < size / 2; ++y)
      buffer(offset + size - 1, y) = buffer(offset + size - 1, size - 1 - y);
    // top edge
    for (int x = 0; x < size / 2; ++x)
      buffer(offset + x, 0) = buffer(offset + size - 1 - x, 0);
    // bottom edge
    for (int x = 0; x < size / 2; ++x)
      buffer(offset + x, size - 1) = buffer(offset + size - 1 - x, size - 1);
  }
}

const int attrPerVertex = 7; // x, y, z, s, t, r, atom
const int attrPerAtom = 4 * attrPerVertex;

vec3 random_normal()
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-1.0, 1.0);

  while (true) {
    vec3 dir(dis(gen), dis(gen), dis(gen));

    // ignore dirs outside unit
    if (dir.norm() > 1.0)
      continue;

    dir.normalize();
    return dir;
  }
}

PDBWidget::PDBWidget(QWidget *parent) : GfxWidget(500, 500, parent)
{
  setEyeZ(1);

  m_zeta = 0.0;
  m_eta = 0.5;
  m_numRays = 50;
  m_aoMapSize = 10;
  m_aoOnly = false;
  m_aoEnabled = false;
  
  qDebug() << "PDBWidget";
}

void PDBWidget::open(const std::string &filename)
{
  setEyeZ(1);

  Real zeta = m_zeta;
  zeta = 0.0;

  std::vector<chemistry::Atom> atoms = chemistry::parse_pdb(filename);
  std::cout << "num_atoms: " << atoms.size() << std::endl;

  /*
  atoms.clear();
  atoms.push_back(chemistry::Atom(-3.0, 0.0, 0.0, 8));
  atoms.push_back(chemistry::Atom(0.0, 0.0, 0.0, 8));
  atoms.push_back(chemistry::Atom(3.0, 0.0, 0.0, 8));
  */

  // compute molecule min/max
  std::vector<Real> x;
  std::vector<Real> y;
  std::vector<Real> z;
  for (std::size_t i = 0; i < atoms.size(); ++i) {
    x.push_back(atoms[i].x);
    y.push_back(atoms[i].y);
    z.push_back(atoms[i].z);
  }
  Real minX = *std::min_element(x.begin(), x.end());
  Real maxX = *std::max_element(x.begin(), x.end());
  Real minY = *std::min_element(y.begin(), y.end());
  Real maxY = *std::max_element(y.begin(), y.end());
  Real minZ = *std::min_element(z.begin(), z.end());
  Real maxZ = *std::max_element(z.begin(), z.end());
  Real dx = maxX - minX;
  Real dy = maxY - minY;
  Real dz = maxZ - minZ;
  Real d = 1.0 / std::max(dx, std::max(dy, dz));

  // compute center
  dx = (minX + maxX) / 2.0;
  dy = (minY + maxY) / 2.0;
  dz = (minZ + maxZ) / 2.0;

  // center molecule
  for (std::size_t i = 0; i < atoms.size(); ++i) {
    atoms[i].x -= dx;
    atoms[i].y -= dy;
    atoms[i].z -= dz;
  }

  // scale molecule
  Real s = 1.0 / (1.0 / d + 4.0);
  for (std::size_t i = 0; i < atoms.size(); ++i) {
    atoms[i].x *= s;
    atoms[i].y *= s;
    atoms[i].z *= s;
  }

  for (std::size_t i = 0; i < atoms.size(); ++i) {
    m_colors.push_back(chemistry::atom_color(atoms[i].element));
    Real r = d * chemistry::vdw_radius(atoms[i].element);
    // top-right corner
    m_attributes.push_back(atoms[i].x);
    m_attributes.push_back(atoms[i].y);
    m_attributes.push_back(atoms[i].z);
    m_attributes.push_back(1 + m_zeta);
    m_attributes.push_back(1 + m_zeta);
    m_attributes.push_back(r);
    m_attributes.push_back(i);
    // top-left corner
    m_attributes.push_back(atoms[i].x);
    m_attributes.push_back(atoms[i].y);
    m_attributes.push_back(atoms[i].z);
    m_attributes.push_back(-1 - m_zeta);
    m_attributes.push_back(1 + m_zeta);
    m_attributes.push_back(r);
    m_attributes.push_back(i);
    // bottom left-corner
    m_attributes.push_back(atoms[i].x);
    m_attributes.push_back(atoms[i].y);
    m_attributes.push_back(atoms[i].z);
    m_attributes.push_back(-1 - m_zeta);
    m_attributes.push_back(-1 - m_zeta);
    m_attributes.push_back(r);
    m_attributes.push_back(i);
    // bottom-right corner
    m_attributes.push_back(atoms[i].x);
    m_attributes.push_back(atoms[i].y);
    m_attributes.push_back(atoms[i].z);
    m_attributes.push_back(1 + m_zeta);
    m_attributes.push_back(-1 - m_zeta);
    m_attributes.push_back(r);
    m_attributes.push_back(i);
  }

  m_zeta = zeta;
}
 
void PDBWidget::computeAmbientOcclusion()
{
  m_aoMap = Buffer<ColorF>(m_aoMapSize * m_attributes.size() / attrPerAtom, m_aoMapSize);
  for (int i = 0; i < m_aoMap.width(); ++i)
    for (int j = 0; j < m_aoMap.height(); ++j)
      m_aoMap(i, j) = Color::black();
      //m_aoMap(i, j) = ColorF(static_cast<Real>(rand()) / RAND_MAX, static_cast<Real>(rand()) / RAND_MAX, static_cast<Real>(rand()) / RAND_MAX);


  Real factor = 1.0 / m_numRays;
  for (int r = 0; r < m_numRays; ++r) {
    std::cout << "\rComputing Ambient Occlusion maps: " << r + 1 << "/" << m_numRays << std::flush;
    vec3 ray = random_normal();
    createAmbientOcclusionMap(ray);

    //if (((r + 1) % 100) == 0)
    //  updatePixmap((r + 1) / 10, m_aoMap);

    mirror_texels(m_aoMap);
    for (int i = 0; i < m_aoMap.width(); ++i)
      for (int j = 0; j < m_aoMap.height(); ++j) {
        if (m_aoMap(i, j).r) {
          m_aoMap(i, j).r = 0;
          m_aoMap(i, j).b += factor;
        }
      }
   
  }
  std::cout << std::endl;

  // find max accessibility
  Real max = 0.0;
  for (int i = 0; i < m_aoMap.width(); ++i)
    for (int j = 0; j < m_aoMap.height(); ++j)
      if (m_aoMap(i, j).b > max)
        max = m_aoMap(i, j).b;

  factor = 1.0 / max;
  for (int i = 0; i < m_aoMap.width(); ++i)
    for (int j = 0; j < m_aoMap.height(); ++j) {
      Real ao = m_aoMap(i, j).b * factor;
      m_aoMap(i, j) = ColorF(ao, ao, ao);
    }

   //updatePixmap(3, m_aoMap);
}

Buffer<Real> PDBWidget::createDepthMap(const vec3 &dir)
{
  Context ctx(context().width(), context().height());
  typedef context_traits<VertexShader, DepthMapFragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader;
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(ctx, program);

  // disable face culling
  ctx.set(GFX_FACE_CULLING, GFX_NO_FACE);

  // clear Z buffer
  ctx.clearZBuffer();

  // create transformation matrices
  mat4 view = lookAtMatrix(0, 0, 1, 0, 0, 0, 0, 1, 0);
  Real aspect = static_cast<Real>(context().width()) / context().height();
  mat4 project = frustumMatrix(-aspect, aspect, -1, 1, 1.0, 10);
  mat4 trans = translationMatrix(0, 0, 0);

  // set vertex shader's matrices to view from light position
  Real theta = std::acos(dir.dot(vec3(0, 0, 1)));
  vec3 axis = dir.cross(vec3(0, 0, -1));
  mat4 rot = rotationMatrix(-theta, axis.x(), axis.y(), axis.z());

  vertexShader.u_mv = view * rot;
  vertexShader.u_p = project;
  fragmentShader.u_p = project;

  renderer.drawQuads(&m_attributes[0], m_attributes.size(), attrPerVertex);

  return ctx.zBuffer();
}

void PDBWidget::createAmbientOcclusionMap(const vec3 &dir)
{
  Context ctx(context().width(), context().height());
  typedef context_traits<VertexShader, AmbientOcclusionMapFragmentShader> T;
  //typedef context_traits<PerFragmentLightVertexShader, PerFragmentLightFragmentShader> T;

  Buffer<Real> depthMap = createDepthMap(dir);
  //updatePixmap(9, depthMap);

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader(depthMap, m_aoMap);
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(ctx, program);

  // disable color buffer
  //ctx.disable(GFX_BUFFER, GFX_COLOR_BUFFER);
  // disable face culling
  ctx.set(GFX_FACE_CULLING, GFX_NO_FACE);

  // clear Z buffer
  ctx.clearZBuffer();

  // create transformation matrices
  mat4 view = lookAtMatrix(0, 0, 1, 0, 0, 0, 0, 1, 0);
  Real aspect = static_cast<Real>(context().width()) / context().height();
  mat4 project = frustumMatrix(-aspect, aspect, -1, 1, 1.0, 10);
  mat4 trans = translationMatrix(0, 0, 0);


  // set vertex shader's matrices to view from light position
  Real theta = std::acos(dir.dot(vec3(0, 0, 1)));
  vec3 axis = dir.cross(vec3(0, 0, -1));
  mat4 rot = rotationMatrix(-theta, axis.x(), axis.y(), axis.z());

  vertexShader.u_mv = view * rot;
  vertexShader.u_p = project;
  fragmentShader.u_inv_mv = (view * rot).inverse();
  fragmentShader.u_p = project;

  renderer.drawQuads(&m_attributes[0], m_attributes.size(), attrPerVertex);
}

void PDBWidget::render()
{
  typedef context_traits<VertexShader, FragmentShader> T;

  T::vertex_shader_type   vertexShader;
  T::fragment_shader_type fragmentShader(m_aoMap);
  T::program_type         program(vertexShader, fragmentShader);
  T::renderer_type        renderer(context(), program);

  // clear color & Z buffers
  context().clearColorBuffer(Color(200, 200, 200));
  context().clearZBuffer();

  context().enable(GFX_FACE_CULLING, GFX_NO_FACE);

  // setup light
  fragmentShader.u_light = DirectionalLight(vec3(0.3, 1, 1).normalized(), LightSource(vec4(1.0, 1.0, 1.0, 0.0), vec4(1.0, 1.0, 1.0, 0.0), vec4()));

  // create transformation matrices
  mat4 rotateX = rotationMatrix(-angleX(), 0, 1, 0);
  mat4 rotateY = rotationMatrix(angleY(), 1, 0, 0);
  Real aspect = static_cast<Real>(context().width()) / context().height();
  mat4 project = frustumMatrix(-aspect, aspect, -1, 1, 1.0, 10);
  mat4 view = lookAtMatrix(0, 0, eyeZ(), 0, 0, 0, 0, 1, 0);

  // set vertex shader uniforms
  vertexShader.u_mv = view * rotateX * rotateY;
  vertexShader.u_p = project;

  // set fragment shader uniforms
  fragmentShader.u_inv_mv = (view * rotateX * rotateY).inverse();
  fragmentShader.u_p = project;
  fragmentShader.u_zeta = m_zeta;
  fragmentShader.u_eta = m_eta;
  fragmentShader.u_aoOnly = m_aoOnly;
  fragmentShader.u_aoEnabled = m_aoEnabled;

  // draw the atoms
  for (std::size_t i = 0; i < m_colors.size(); ++i) {
    fragmentShader.u_material = Material(m_colors[i], m_colors[i], vec4(1.0, 1.0, 1.0, 1.0));
    renderer.drawQuads(&m_attributes[i * attrPerAtom], attrPerAtom, attrPerVertex);
  }



  //updatePixmap(0, context().zBuffer());
  copyColorBufferToImage();
}
