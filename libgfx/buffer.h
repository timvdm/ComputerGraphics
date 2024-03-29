#ifndef GFX_BUFFER_H
#define GFX_BUFFER_H

#include <vector>
#include <cassert>

namespace GFX {

  template<typename ValueType>
  class Buffer
  {
    public:
      Buffer(int width = 1, int height = 1) : m_buffer(width * height), m_width(width), m_height(height)
      {
      }

      Buffer(const Buffer<ValueType> &other)
      {
        m_buffer = other.m_buffer;
        m_width = other.m_width;
        m_height = other.m_height;
      }

      Buffer& operator=(const Buffer<ValueType> &other)
      {
        m_buffer = other.m_buffer;
        m_width = other.m_width;
        m_height = other.m_height;
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
        m_buffer.resize(width * height);
      }

      const ValueType& operator()(int x, int y) const
      {
        assert(x >= 0 && x < m_width && y >= 0 && y < m_height);
        return m_buffer[index(x, y)];
      }
      
      ValueType& operator()(int x, int y)
      {
        assert(x >= 0 && x < m_width && y >= 0 && y < m_height);
        return m_buffer[index(x, y)];
      }

      void clear(const ValueType &value)
      {
        for (int i = 0; i < m_width * m_height; ++i)
          m_buffer[i] = value;
      }

    private:
      std::size_t index(int x, int y) const
      {
        return x + m_width * y;
      }

      std::vector<ValueType> m_buffer;
      int m_width;
      int m_height;
  };

}

#endif
