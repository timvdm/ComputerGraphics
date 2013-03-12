#ifndef GFX_MESH_H
#define GFX_MESH_H

#include "../utils.h"

namespace GFX {

  class Mesh
  {
    public:
      void addVertex(const vec4 &v)
      {
        m_vertices.push_back(v);
      }

      void addVertex(const std::vector<double> &v)
      {
        m_vertices.push_back(vec4(v[0], v[1], v[2], 1.0));
      }

      void addFace(const std::vector<int> &face)
      {
        m_faces.push_back(face);
      }

      const std::vector<vec4>& vertices() const
      {
        return m_vertices;
      }

      const std::vector<std::vector<int> >& faces() const
      {
        return m_faces;
      }

    private:
      std::vector<vec4> m_vertices;
      std::vector<std::vector<int> > m_faces;
  };

}

#endif
