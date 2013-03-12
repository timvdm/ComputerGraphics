#ifndef CG_MESH_H
#define CG_MESH_H

#include "../utils.h"

namespace CG {

  namespace XYZ {

    class Mesh
    {
      public:
        void addVertex(const Vector3D &v)
        {
          m_vertices.push_back(v);
        }

        void addVertex(const std::vector<double> &v)
        {
          m_vertices.push_back(Vector3D::point(v[0], v[1], v[2]));
        }

        void addFace(const std::vector<int> &face)
        {
          m_faces.push_back(face);
        }

        const std::vector<Vector3D>& vertices() const
        {
          return m_vertices;
        }

        const std::vector<std::vector<int> >& faces() const
        {
          return m_faces;
        }

      private:
        std::vector<Vector3D> m_vertices;
        std::vector<std::vector<int> > m_faces;
    };

  }

}

#endif
