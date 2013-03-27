#include "texture.h"

#include "EasyImage.h"

#include <fstream>

namespace GFX {

  bool Texture::open(const std::string &filename)
  {
    //std::cout << "Texture::open()" << std::endl;

    m_textures.clear();
    m_mipmap.clear();

    std::ifstream ifs(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!ifs)
      return false;

    img::EasyImage image;
    if (!(ifs >> image))
      return false;
    
    m_textures.resize(1);
    m_textures[0] = Buffer<Color>(image.get_width(), image.get_height());
    
    //std::cout << m_texture.width() << "x" << m_texture.height() << std::endl;

    for (int y = 0; y < image.get_height(); ++y)
      for (int x = 0; x < image.get_width(); ++x) {
        const img::Color &color = image(x, y);
        m_textures[0](x, y) = Color(color.red, color.green, color.blue, 255);
      }

    int size = std::max(m_textures.back().width(), m_textures.back().height());
    m_mipmap.resize(size);

    mipmap();

    return true;
  }

  void Texture::mipmap()
  {
    if (m_textures.empty())
      return;

    while (true) {
      int width = m_textures.back().width();
      int height = m_textures.back().height();

      int newWidth = (width - 3) / 2;
      int newHeight = (height - 3) / 2;

      if (!newWidth || !newHeight)
        break;

      const Buffer<Color> &large = m_textures.back();
      m_textures.resize(m_textures.size() + 1);
      m_textures.back() = Buffer<Color>(newWidth, newHeight);
      Buffer<Color> &small = m_textures.back();

      std::cout << width << "x" << height << std::endl;
      for (int y = 0; y < newHeight; ++y)
        for (int x = 0; x < newWidth; ++x) {
          //std::cout << x << ", " << y << " -> " << x * 2 << ", " << y * 2 << std::endl;
          const Color &color1 = large(x * 2, y * 2);
          const Color &color2 = large(x * 2 + 1, y * 2);
          const Color &color3 = large(x * 2, y * 2 + 1);
          const Color &color4 = large(x * 2 + 1, y * 2 + 1);

          small(x, y) = Color((color1.r + color2.r + color3.r + color4.r) / 4,
                              (color1.g + color2.g + color3.g + color4.g) / 4,
                              (color1.b + color2.b + color3.b + color4.b) / 4,
                              (color1.a + color2.a + color3.a + color4.a) / 4);
        }
    }


    /*
    for (std::size_t i = 0; i < m_textures.size(); ++i) {
      int size = std::max(m_textures[i].width(), m_textures[i].height());
      for (std::size_t j = 0; j < size; ++j)
        m_mipmap[j] = i;
    }
    */
  }

}
