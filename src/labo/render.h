#ifndef LABO_RENDER_H
#define LABO_RENDER_H

#include "../utils/EasyImage.h"

#include <libgfx/line2d.h>
#include <libgfx/line3d.h>
#include <libgfx/buffer.h>

#include <memory>



struct Material
{
  Material(const GFX::ColorF &ambient_ = GFX::Color::black(), const GFX::ColorF &diffuse_ = GFX::Color::black(),
           const GFX::ColorF &specular_ = GFX::Color::black(), GFX::Real reflection_ = 0.0)
    : ambient(ambient_), diffuse(diffuse_), specular(specular_), reflection(reflection_)
  {
  }

  GFX::ColorF ambient;
  GFX::ColorF diffuse;
  GFX::ColorF specular;
  GFX::Real reflection;
};

inline std::ostream& operator<<(std::ostream &os, const Material &material)
{
  os << "Material:" << std::endl;
  os << "    ambient: (" << material.ambient.r << ", " << material.ambient.g << ", " << material.ambient.b << ")" << std::endl;
  os << "    diffuse: (" << material.diffuse.r << ", " << material.diffuse.g << ", " << material.diffuse.b << ")" << std::endl;
  os << "    specular: (" << material.specular.r << ", " << material.specular.g << ", " << material.specular.b << ")" << std::endl;
  os << "    reflection: " << material.reflection;
  return os;
}

struct Light
{
  enum Type {
    InfLight,
    PointLight
  };

  Light(int type_, const GFX::ColorF &ambient_ = GFX::Color::black(), const GFX::ColorF &diffuse_ = GFX::Color::black(),
        const GFX::ColorF &specular_ = GFX::Color::black(), const GFX::vec4 &vec = GFX::vec4::Zero())
    : type(type_), ambient(ambient_), diffuse(diffuse_), specular(specular_), m_vec(vec)
  {
  }

  GFX::vec3 dir() const
  {
    return GFX::vec3(m_vec.data());
  }

  GFX::vec3 pos() const
  {
    return GFX::vec3(m_vec.data());
  }

  GFX::vec4& vec()
  {
    return m_vec;
  }

  const GFX::vec4& vec() const
  {
    return m_vec;
  }

  int type;
  GFX::ColorF ambient;
  GFX::ColorF diffuse;
  GFX::ColorF specular;
private:
  GFX::vec4 m_vec;
};

inline std::ostream& operator<<(std::ostream &os, const Light &light)
{
  if (light.type == Light::InfLight)
    os << "InfLight:" << std::endl;
  else
    os << "PointLight:" << std::endl;
  os << "    ambient: (" << light.ambient.r << ", " << light.ambient.g << ", " << light.ambient.b << ")" << std::endl;
  os << "    diffuse: (" << light.diffuse.r << ", " << light.diffuse.g << ", " << light.diffuse.b << ")" << std::endl;
  os << "    specular: (" << light.specular.r << ", " << light.specular.g << ", " << light.specular.b << ")" << std::endl;
  if (light.type == Light::InfLight)
    os << "    direction: ";
  else
    os << "    location: ";
  os << "(" << light.dir().x() << ", " << light.dir().y() << ", " << light.dir().z() << ")";
  return os;
}



/**
 * @brief Find the min. and max. paints for a set of lines.
 *
 * The returned std::pair contains two Point2D objects representing the lower-left
 * and upper-right corners of the smallest rectangle containing all lines.
 *
 * @param lines The set of lines.
 *
 * @return The min. and max. points for the specified set of lines.
 */
template<typename LinesXD>
std::pair<GFX::Point2D, GFX::Point2D> get_min_max(const LinesXD &lines);


/**
 * @brief Compute the size for an image given the min. and max. points.
 *
 * @param minMax The min. and max. points (e.g. result of get_min_max()).
 * @param size The size for the largest range.
 *
 * @return A std::pair containing the size along the x and y axis.
 */
std::pair<int, int> get_image_sizes(const std::pair<GFX::Point2D, GFX::Point2D> &minMax, int size);

/**
 * @brief Get the scaling factor for the image.
 *
 * @param minMax The min. and max. coordinate for the drawing (e.g. result of get_min_max()).
 * @param xSize The size of the image along the x axis.
 * @param border A scaling factor applied to the image to ensure there is some padding along the edges.
 *
 * @return The scaling factor d.
 */
GFX::Real get_scale_factor(const std::pair<GFX::Point2D, GFX::Point2D> &minMax, int xSize, GFX::Real border = 0.95);

/**
 * @brief Get the center for a set of lines.
 *
 * @param minMax The min. and max. coordinate for the drawing (e.g. result of get_min_max()).
 * @param d The scaling factor for the image.
 *
 * @return The center for the set of lines.
 */
GFX::Point2D get_center(const std::pair<GFX::Point2D, GFX::Point2D> &minMax, GFX::Real d);

/**
 * @brief Apply a scaling factor to a set of lines.
 *
 * @param lines The set of lines.
 * @param d The scaling factor for the image.
 */
template<typename LinesXD>
void scale_lines(LinesXD &lines, GFX::Real d);

/**
 * @brief Apply a translation to a set of lines to ensure the image is centered
 * inside the image when drawing.
 *
 * @param lines The set of lines.
 * @param imageSizes The x and y size of the final image.
 * @param center The current center of the lines.
 */
template<typename LinesXD>
void center_lines(LinesXD &lines, const std::pair<int, int> &imageSizes, const GFX::Point2D &center);

/**
 * @brief Draw a set of lines in an EasyImage.
 *
 * @param lines The lines to draw.
 * @param size The maximal size in the x or y direction for the image.
 * @param bgColor The background color for the image.
 *
 * @return The EasyImage with the drawing.
 */
img::EasyImage draw_lines(GFX::Lines2D &lines, int size, const img::Color &bgColor);

namespace GFX {
  class Mesh;
}

void mesh_to_lines2d(const GFX::Mesh &mesh, const GFX::Color &color, const GFX::mat4 &T, GFX::Lines2D &lines);
void mesh_to_lines3d(const GFX::Mesh &mesh, const GFX::Color &color, const GFX::mat4 &T, GFX::Lines3D &lines);

img::EasyImage draw_zbuffered_lines(GFX::Lines3D &lines, int size, const img::Color &bgColor);

std::pair<GFX::Point2D, GFX::Point2D> get_min_max(const GFX::Mesh &mesh, const GFX::mat4 &T);

img::EasyImage draw_zbuffered_mesh(const GFX::Mesh &mesh, const GFX::mat4 &T, const GFX::Color &color, int size, const img::Color &bgColor);
img::EasyImage draw_zbuffered_meshes(const std::vector<std::shared_ptr<GFX::Mesh> > &meshes, const GFX::mat4 &T,
    const std::vector<GFX::mat4> &modelMatrices, const std::vector<GFX::Color> &colors, int size, const img::Color &bgColor);


struct ShadowMask
{
  ShadowMask(const GFX::Buffer<GFX::Real> &mask_, const GFX::mat4 &view_, GFX::Real d_, GFX::Real dx_, GFX::Real dy_)
    : mask(mask_), view(view_), d(d_), dx(dx_), dy(dy_)
  {
  }

  GFX::Buffer<GFX::Real> mask; // the shadow mask (z-buffer)
  GFX::mat4 view; // view matrix for light position
  GFX::Real d; // distance from origin to camera (light position)
  GFX::Real dx; // x displacement to screen coords
  GFX::Real dy; // y displacement to screen coords
};


extern GFX::Real shadowEpsilon;

img::EasyImage draw_zbuffered_meshes(const std::vector<std::shared_ptr<GFX::Mesh> > &meshes, const GFX::mat4 &project,
    const std::vector<GFX::mat4> &modelMatrices, const std::vector<Light> &lights, const std::vector<Material> &materials,
    const std::vector<ShadowMask> &shadowMasks, int size, const img::Color &bgColor);


ShadowMask draw_shadow_mask(const std::vector<std::shared_ptr<GFX::Mesh> > &meshes, const GFX::mat4 &project,
    const std::vector<GFX::mat4> &modelMatrices, int size);


#endif
