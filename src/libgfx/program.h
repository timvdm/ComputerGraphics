#ifndef GFX_PROGRAM_H
#define GFX_PROGRAM_H

namespace GFX {

  template<typename VertexShaderType, typename FragmentShaderType>
  class Program
  {
    public:
      typedef typename VertexShaderType::varying_type varying_type;

      Program(VertexShaderType &vertexShader, FragmentShaderType &fragmentShader)
        : m_vertexShader(vertexShader), m_fragmentShader(fragmentShader)
      {
        static_assert(std::is_same<typename VertexShaderType::varying_type, 
                                   typename FragmentShaderType::varying_type>::value,
            "Vertex and fragment shader varying_type must be the same.");
      }

      const VertexShaderType& vertexShader() const
      {
        return m_vertexShader;
      }

      VertexShaderType& vertexShader()
      {
        return m_vertexShader;
      }

      const FragmentShaderType& fragmentShader() const
      {
        return m_fragmentShader;
      }

      FragmentShaderType& fragmentShader()
      {
        return m_fragmentShader;
      }

    private:
      VertexShaderType &m_vertexShader;
      FragmentShaderType &m_fragmentShader;
  };

}

#endif
