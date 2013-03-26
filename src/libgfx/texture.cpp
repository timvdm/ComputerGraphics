#include "texture.h"

#include "EasyImage.h"

#include <fstream>

namespace GFX {

  bool Texture::open(const std::string &filename)
  {
    //std::cout << "Texture::open()" << std::endl;
    assert(0);
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

    return true;
  }

}
