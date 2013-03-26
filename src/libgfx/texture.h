#ifndef GFX_TEXTURE_H
#define GFX_TEXTURE_H

#include "buffer.h"
#include "color.h"

namespace GFX {

  class Texture
  {
    public:
      Texture()
      {
      }

      Texture(const std::string &filename)
      {
        open(filename);
      }

      bool open(const std::string &filename);

      void mipmap();

      int width() const
      {
        return m_textures[0].width();
      }

      int height() const
      {
        return m_textures[0].height();
      }

      const Color& operator()(double u, double v) const
      {
        int x = static_cast<int>(u * m_textures[0].width()) % m_textures[0].width();
        int y = static_cast<int>(v * m_textures[0].height()) % m_textures[0].height();
        return m_textures[0](x, y);
      }

    private:
      std::vector<Buffer<Color> > m_textures;
  };

}

#endif
