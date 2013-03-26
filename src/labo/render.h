#ifndef LABO_RENDER_H
#define LABO_RENDER_H

#include "../utils/EasyImage.h"


#include "../libgfx/line2d.h"
#include "../libgfx/line3d.h"

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

void renderMesh(const GFX::Mesh &mesh, const GFX::Color &color, const GFX::mat4 &T, GFX::Lines2D &lines);
void renderMesh(const GFX::Mesh &mesh, const GFX::Color &color, const GFX::mat4 &T, GFX::Lines3D &lines);

img::EasyImage draw_zbuffered_lines(GFX::Lines3D &lines, int size, const img::Color &bgColor);

#endif
