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

      Texture(int width, int height) : m_mipmap(0)
      {
        m_textures.push_back(Buffer<Color>(width, height));
      }

      Texture(const std::string &filename, bool mipMap = true) : m_mipmap(0)
      {
        open(filename, mipMap);
      }

      bool open(const std::string &filename, bool mipMap = true);

      int width() const
      {
        return m_textures[m_mipmap].width();
      }

      int height() const
      {
        return m_textures[m_mipmap].height();
      }

      const Color& operator()(Real u, Real v) const
      {
        int x = static_cast<int>(u * m_textures[m_mipmap].width()) % m_textures[m_mipmap].width();
        int y = static_cast<int>(v * m_textures[m_mipmap].height()) % m_textures[m_mipmap].height();
        assert(m_textures.size());
        return m_textures[m_mipmap](x, y);
      }

      void mipmap();
      void setMipmap(int x, int y);

    private:
      std::vector<Buffer<Color> > m_textures;
      std::size_t m_mipmap;
  };

}

#endif
