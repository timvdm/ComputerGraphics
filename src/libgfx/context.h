#ifndef GFX_CONTEXT_H
#define GFX_CONTEXT_H

#include "buffer.h"
#include "color.h"
#include "texture.h"

#include <iostream>

namespace GFX {

  enum ContextFlagTypes {
    GFX_BUFFER = 0,
    GFX_FACE_CULLING = 1,

    LastFlagType
  };

  enum ConextFlags {
    // face culling
    GFX_BACK_FACE = 1,
    GFX_FRONT_FACE = 2,
    GFX_NO_FACE = 0,
    // buffers
    GFX_Z_BUFFER = 1,
    GFX_COLOR_BUFFER = 2
  };

  class Context
  {
    public:
      Context(int width, int height) : m_colorBuffer(width, height),
          m_zBuffer(width, height), m_width(width), m_height(height)
      {
        m_near = 1.0;
        // clear flags
        for (int i = 0; i < LastFlagType; ++i)
          m_flags[i] = 0;
        // set default flags
        enable(GFX_BUFFER, GFX_Z_BUFFER | GFX_COLOR_BUFFER);
        enable(GFX_FACE_CULLING, GFX_BACK_FACE);
      }

      Context(const Context &other)
      {
        m_colorBuffer = other.m_colorBuffer;
        m_zBuffer = other.m_zBuffer;
        m_textures = other.m_textures;
        m_near = other.m_near;
        m_width = other.m_width;
        m_height = other.m_height;
        for (int i = 0; i < LastFlagType; ++i)
          m_flags[i] = other.m_flags[i];
      }

      Context& operator=(const Context &other)
      {
        m_colorBuffer = other.m_colorBuffer;
        m_zBuffer = other.m_zBuffer;
        m_textures = other.m_textures;
        m_near = other.m_near;
        m_width = other.m_width;
        m_height = other.m_height;
        for (int i = 0; i < LastFlagType; ++i)
          m_flags[i] = other.m_flags[i];
        return *this;
      }

      int width() const
      {
        return m_width;
      }

      int height() const
      {
        return m_height;
      }

      void resize(int width, int height)
      {
        m_width = width;
        m_height = height;
        m_zBuffer.resize(width, height);
        m_colorBuffer.resize(width, height);
      }

      Real near() const
      {
        return m_near;
      }

      void setNear(Real near)
      {
        m_near = near;
      }

      const Buffer<Color>& colorBuffer() const
      {
        return m_colorBuffer;
      }

      Buffer<Color>& colorBuffer()
      {
        return m_colorBuffer;
      }

      const Buffer<Real>& zBuffer() const
      {
        return m_zBuffer;
      }

      Buffer<Real>& zBuffer()
      {
        return m_zBuffer;
      }

      void clearZBuffer()
      {
        m_zBuffer.clear(std::numeric_limits<Real>::max());
      }

      void clearColorBuffer(const Color &color = Color::black())
      {
        m_colorBuffer.clear(color);
      }

      void drawPixel(int x, int y, Real z, const Color &color)
      {
        if (!color.a)
          return;
        //std::cout << "Context::drawPixel()" << std::endl;

        // ensure the pixel is within the color buffer
        if (x < 0 || x >= m_colorBuffer.width() || y < 0 || y > m_colorBuffer.height())
          return;

        // check z-buffer if it is enabled
        if (flags(GFX_BUFFER) & GFX_Z_BUFFER) {
          // if the Z-buffer value is greater, don't draw the pixel
          if (z > m_zBuffer(x, y))
            return;
          // update Z-buffer value
          m_zBuffer(x, y) = z;
        }

        //std::cout << "    drawing pixel @ (" << x << ", " << y << ")" << std::endl;

        // draw the pixel
        m_colorBuffer(x, y) = color;
      }

      const std::vector<Texture>& textures() const
      {
        return m_textures;
      }

      std::vector<Texture>& textures()
      {
        return m_textures;
      }

      int createTexture(const std::string &filename)
      {
        m_textures.resize(m_textures.size() + 1);
        m_textures.back().open(filename);
        return m_textures.size() - 1;
      }

      int createTexture(int width, int height)
      {
        m_textures.push_back(Texture(width, height));
        return m_textures.size() - 1;
      }

      void setMipmaps(int x, int y)
      {
        for (std::size_t i = 0; i < m_textures.size(); ++i)
          m_textures[i].setMipmap(x, y);
      }

      int flags(int type) const
      {
        return m_flags[type];
      }

      void set(int type, int falgs)
      {
        m_flags[type] = falgs;
      }

      void enable(int type, int flags)
      {
        m_flags[type] = m_flags[type] | flags;
      }

      void disable(int type, int flags)
      {
        m_flags[type] = m_flags[type] & ~flags;
      }

    private:
      Buffer<Color> m_colorBuffer;
      Buffer<Real> m_zBuffer;
      std::vector<Texture> m_textures;
      Real m_near;
      int m_width;
      int m_height;
      int m_flags[LastFlagType];
  };

}

#endif
