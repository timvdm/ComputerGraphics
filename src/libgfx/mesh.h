#ifndef GFX_MESH_H
#define GFX_MESH_H

#include "types.h"
#include "color.h"

#include <vector>
#include <memory>

namespace GFX {

  class Mesh
  {
    public:
      typedef std::vector<int> Face;

      Mesh() : m_color(Color::red())
      {
      }

      void addVertex(Real x, Real y, Real z)
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

      void addVertex(const std::vector<float> &v)
      {
        m_vertices.push_back(vec4(v[0], v[1], v[2], 1.0));
      }

      void addVertex(const std::vector<double> &v)
      {
        m_vertices.push_back(vec4(v[0], v[1], v[2], 1.0));
      }

      void addNormal(Real x, Real y, Real z)
      {
        m_normals.push_back(vec4(x, y, z, 1.0));
      }

      void addNormal(const vec3 &v)
      {
        m_normals.push_back(vec4(v.x(), v.y(), v.z(), 1.0));
      }

      void addNormal(const vec4 &v)
      {
        m_normals.push_back(v);
      }

      void addNormal(const std::vector<float> &v)
      {
        m_normals.push_back(vec4(v[0], v[1], v[2], 1.0));
      }

      void addNormal(const std::vector<double> &v)
      {
        m_normals.push_back(vec4(v[0], v[1], v[2], 1.0));
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

      /**
       * @brief Set a single color for the entire mesh.
       *
       * @param color The color for the mesh.
       */
      void setColor(const Color &color)
      {
        m_color = color;
      }

      /**
       * @brief Set the color for a specific face.
       *
       * @param index The face index (indexed from 0).
       * @param color The new face color.
       */
      void setFaceColor(int index, const Color &color);

      /**
       * @brief Set the color for a specific vertex.
       *
       * @param index The vertex index (indexed from 0).
       * @param color The new vertex color.
       */
      void setVertexColor(int index, const Color &color);

      /**
       * @brief Compute face normals.
       *
       * This method computes the face normals assuming face vertices are
       * ordered counter clockwise.
       */
      void computeNormals();

      std::vector<Real> triangleAttributes(bool normals = false, bool colors = false, bool texCoords = false);

      std::vector<Real> quadAttributes(bool normals = false, bool colors = false, bool texCoords = false);


      static Face make_face(int i, int j, int k = -1, int l = -1, int m = -1);

      static std::shared_ptr<Mesh> cube();
      static std::shared_ptr<Mesh> tetrahedron();
      static std::shared_ptr<Mesh> octahedron();
      static std::shared_ptr<Mesh> icosahedron();
      static std::shared_ptr<Mesh> dodecahedron();
      static std::shared_ptr<Mesh> cone(int n, Real h);
      static std::shared_ptr<Mesh> cylinder(int n, Real h);
      static std::shared_ptr<Mesh> sphere(int n);
      static std::shared_ptr<Mesh> torus(int n, int m, Real R, Real r);

    private:
      void addVertexAttributes(std::vector<Real> &attr, int f, int v, bool normals, bool colors, bool texCoords);

      std::vector<vec4> m_vertices; //!< The vertices.
      std::vector<Face> m_faces; //!< The faces.
      std::vector<vec4> m_normals; //!< The face normals.
      std::vector<Color> m_colors; //!< Per vertex colors.
      std::vector<vec2> m_texCoords; //!< Per vertex texture coordinates.
      Color m_color; //!< Single color for entire mesh.
  };

}

#endif
