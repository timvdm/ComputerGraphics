#ifndef GFX_CONTEXT_TRAITS_H
#define GFX_CONTEXT_TRAITS_H

namespace GFX {

  template<typename VertexShaderType, typename FragmentShaderType>
  struct context_traits
  {
    typedef VertexShaderType vertex_shader_type;
    typedef FragmentShaderType fragment_shader_type;
    typedef typename vertex_shader_type::varying_type varying_type;
    typedef Program<vertex_shader_type, fragment_shader_type> program_type;
    typedef Renderer<program_type> renderer_type;
  };

}

#endif
