#ifndef GFX_MESH_H
#define GFX_MESH_H

#include "types.h"

#include <vector>

namespace GFX {

  class Mesh
  {
    public:
      typedef std::vector<int> Face;

      void addVertex(double x, double y, double z)
      {
        m_vertices.push_back(vec4(x, y, z, 1.0));
      }

      void addVertex(const vec3 &v)
      {
        m_vertices.push_back(vec4(v.x(), v.y(), v.z(), 1.0));
      }

      void addVertex(const vec4 &v)
      {
        m_vertices.push_back(v);
      }

      void addVertex(const std::vector<double> &v)
      {
        m_vertices.push_back(vec4(v[0], v[1], v[2], 1.0));
      }

      void addFace(int i, int j)
      {
        m_faces.resize(m_faces.size() + 1);
        m_faces.back().push_back(i);
        m_faces.back().push_back(j);
      }

      void addFace(int i, int j, int k)
      {
        m_faces.resize(m_faces.size() + 1);
        m_faces.back().push_back(i);
        m_faces.back().push_back(j);
        m_faces.back().push_back(k);
      }

      void addFace(int i, int j, int k, int l)
      {
        m_faces.resize(m_faces.size() + 1);
        m_faces.back().push_back(i);
        m_faces.back().push_back(j);
        m_faces.back().push_back(k);
        m_faces.back().push_back(l);
      }

      void addFace(int i, int j, int k, int l, int m)
      {
        m_faces.resize(m_faces.size() + 1);
        m_faces.back().push_back(i);
        m_faces.back().push_back(j);
        m_faces.back().push_back(k);
        m_faces.back().push_back(l);
        m_faces.back().push_back(m);
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

      static Face make_face(int i, int j, int k = -1, int l = -1, int m = -1)
      {
        Face face;
        face.push_back(i);
        face.push_back(j);
        if (k >= 0)
          face.push_back(k);
        if (l >= 0)
          face.push_back(l);
        if (m >= 0)
          face.push_back(m);
        return face;
      }

    private:
      std::vector<vec4> m_vertices;
      std::vector<Face> m_faces;
  };

}

#endif
