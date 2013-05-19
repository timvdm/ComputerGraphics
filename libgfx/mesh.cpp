#include "mesh.h"
#include "utility.h"

namespace GFX {
      
  void Mesh::setFaceColor(int index, const Color &color)
  {
    assert(index < m_faces.size());

    if (m_colors.size() < m_vertices.size())
      m_colors.resize(m_vertices.size());

    const Face &face = m_faces[index];
    for (std::size_t i = 0; i  < face.size(); ++i)
      m_colors[face[i]] = color;
  }
  
  void Mesh::setVertexColor(int index, const Color &color)
  {
    assert(index < m_vertices.size());

    if (m_colors.size() < m_vertices.size())
      m_colors.resize(m_vertices.size());

    m_colors[index] = color;
  }

  void Mesh::computeNormals(bool smooth)
  {
    m_normals.clear();

    for (std::size_t i = 0; i < m_faces.size(); ++i) {
      const Face &face = m_faces[i];
      if (face.size() < 3)
        continue;

      vec4 AB = m_vertices[face[1]] - m_vertices[face[0]];
      vec4 AC = m_vertices[face[2]] - m_vertices[face[0]];

      vec4 n(AB.y() * AC.z() - AB.z() * AC.y(),
             AB.z() * AC.x() - AB.x() * AC.z(),
             AB.x() * AC.y() - AB.y() * AC.x(), 0.0);

      n.normalize();

      m_normals.push_back(n);    
    }

    if (!smooth)
      return;

    std::vector<vec4> normals;
    for (std::size_t i = 0; i < m_vertices.size(); ++i) {
      vec4 normal(vec4::Zero());
      for (std::size_t j = 0; j < m_vertices.size(); ++j) {
        Real dist = (m_vertices[i] - m_vertices[j]).norm();
        if (dist < 0.00001)
          for (std::size_t k = 0; k < m_faces.size(); ++k)
            if (std::find(m_faces[k].begin(), m_faces[k].end(), j) != m_faces[k].end())
              normal += m_normals[k];
      }
      normal.normalize();
      normals.push_back(normal);
    }

    m_normals.swap(normals);
  }

  void Mesh::triangulate()
  {
    std::size_t numFaces = m_faces.size();
    for (std::size_t i = 0; i < numFaces; ++i) {
      Face face = m_faces[i];
      if (face.size() <= 3)
        continue;

      for (std::size_t j = 1; j < face.size() - 1; ++j)
        if (j == 1)
          m_faces[i] = make_face(face[0], face[j], face[j + 1]);
        else
          m_faces.push_back(make_face(face[0], face[j], face[j + 1]));
    }
  }

  std::vector<Real> Mesh::triangleAttributes(bool normals, bool colors, bool texCoords, std::size_t extra)
  {
    std::vector<Real> attr;

    if (normals && m_normals.size() != m_faces.size() && m_normals.size() != m_vertices.size())
      computeNormals();

    for (std::size_t i = 0; i < m_faces.size(); ++i) {
      const Face &face = m_faces[i];
      if (face.size() != 3)
        continue;

      addVertexAttributes(attr, i, 0, normals, colors, texCoords);
      addVertexAttributes(attr, i, 1, normals, colors, texCoords);
      addVertexAttributes(attr, i, 2, normals, colors, texCoords);
      attr.resize(attr.size() + extra);
    }

    return attr;
  }
  
  std::vector<Real> Mesh::quadAttributes(bool normals, bool colors, bool texCoords, std::size_t extra)
  {
    std::vector<Real> attr;

    if (normals && m_normals.size() != m_faces.size() && m_normals.size() != m_vertices.size())
      computeNormals();

    for (std::size_t i = 0; i < m_faces.size(); ++i) {
      const Face &face = m_faces[i];
      if (face.size() != 4)
        continue;

      addVertexAttributes(attr, i, 0, normals, colors, texCoords);
      addVertexAttributes(attr, i, 1, normals, colors, texCoords);
      addVertexAttributes(attr, i, 2, normals, colors, texCoords);
      addVertexAttributes(attr, i, 3, normals, colors, texCoords);
      attr.resize(attr.size() + extra);
    }

    return attr;
  }

  void Mesh::addVertexAttributes(std::vector<Real> &attr, int f, int v, bool normals, bool colors, bool texCoords)
  {
    const Face &face = m_faces[f];

    attr.push_back(m_vertices[face[v]].x());
    attr.push_back(m_vertices[face[v]].y());
    attr.push_back(m_vertices[face[v]].z());

    if (normals) {
      if (m_normals.size() == m_faces.size()) {
        attr.push_back(m_normals[f].x());
        attr.push_back(m_normals[f].y());
        attr.push_back(m_normals[f].z());
      } else if (m_normals.size() == m_vertices.size()) {
        attr.push_back(m_normals[face[v]].x());
        attr.push_back(m_normals[face[v]].y());
        attr.push_back(m_normals[face[v]].z());
      }
    }

    if (colors) {
      if (m_colors.size() != m_vertices.size()) {
        // single color
        attr.push_back(m_color.r);
        attr.push_back(m_color.g);
        attr.push_back(m_color.b);
        attr.push_back(m_color.a);
      } else {
        // per vertex colors
        attr.push_back(m_colors[face[v]].r);
        attr.push_back(m_colors[face[v]].g);
        attr.push_back(m_colors[face[v]].b);
        attr.push_back(m_colors[face[v]].a);
      }
    }
  }

  Mesh::Face Mesh::make_face(int i, int j, int k, int l, int m)
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

  std::shared_ptr<Mesh> Mesh::cube()
  {
    std::shared_ptr<Mesh> mesh(new Mesh);

    mesh->addVertex( 1, -1, -1);
    mesh->addVertex(-1,  1, -1);
    mesh->addVertex( 1,  1,  1);
    mesh->addVertex(-1, -1,  1);
    mesh->addVertex( 1,  1, -1);
    mesh->addVertex(-1, -1, -1);
    mesh->addVertex( 1, -1,  1);
    mesh->addVertex(-1,  1,  1);

    mesh->addFace(0, 4, 2, 6);
    mesh->addFace(4, 1, 7, 2);
    mesh->addFace(1, 5, 3, 7);
    mesh->addFace(5, 0, 6, 3);
    mesh->addFace(6, 2, 7, 3);
    mesh->addFace(0, 5, 1, 4);

    return mesh;
  }

  std::shared_ptr<Mesh> Mesh::tetrahedron()
  {
    std::shared_ptr<Mesh> mesh(new Mesh);
    
    mesh->addVertex( 1, -1, -1);
    mesh->addVertex(-1,  1, -1);
    mesh->addVertex( 1,  1,  1);
    mesh->addVertex(-1, -1,  1);

    mesh->addFace(0, 1, 2);
    mesh->addFace(1, 3, 2);
    mesh->addFace(0, 1, 3);
    mesh->addFace(0, 2, 3);

    return mesh;
  }
  
  std::shared_ptr<Mesh> Mesh::octahedron()
  {
    std::shared_ptr<Mesh> mesh(new Mesh);

    mesh->addVertex( 1,  0,  0);
    mesh->addVertex( 0,  1,  0);
    mesh->addVertex(-1,  0,  0);
    mesh->addVertex( 0, -1,  0);
    mesh->addVertex( 0,  0, -1);
    mesh->addVertex( 0,  0,  1);

    mesh->addFace(0, 1, 5);
    mesh->addFace(1, 2, 5);
    mesh->addFace(2, 3, 5);
    mesh->addFace(3, 0, 5);
    mesh->addFace(1, 0, 4);
    mesh->addFace(2, 1, 4);
    mesh->addFace(3, 2, 4);
    mesh->addFace(0, 3, 4);

    return mesh;
  }

  std::shared_ptr<Mesh> Mesh::icosahedron()
  {
    std::shared_ptr<Mesh> mesh(new Mesh);

    Real sqrt_5_2 = std::sqrt(5.0) / 2.0;
    Real pi_2_5 = 2.0 * M_PI / 5.0;
    Real pi_5 = M_PI / 5.0;

    mesh->addVertex(0.0, 0.0, sqrt_5_2);

    for (int i = 0; i < 5; ++i)
      mesh->addVertex(std::cos(i * pi_2_5), std::sin(i * pi_2_5), 0.5);

    for (int i = 0; i < 5; ++i)
      mesh->addVertex(std::cos(pi_5 + i * pi_2_5), std::sin(pi_5 + i * pi_2_5), -0.5);

    mesh->addVertex(0.0, 0.0, -sqrt_5_2);

    mesh->addFace(0, 1, 2);
    mesh->addFace(0, 2, 3);
    mesh->addFace(0, 3, 4);
    mesh->addFace(0, 4, 5);

    mesh->addFace(0, 5, 1);
    mesh->addFace(1, 6, 2);
    mesh->addFace(2, 6, 7);
    mesh->addFace(2, 7, 3);

    mesh->addFace(3, 7, 8);
    mesh->addFace(3, 8, 4);
    mesh->addFace(4, 8, 9);
    mesh->addFace(4, 9, 5);

    mesh->addFace(5, 9, 10);
    mesh->addFace(5, 10, 1);
    mesh->addFace(1, 10, 6);
    mesh->addFace(11, 7, 6);

    mesh->addFace(11, 8, 7);
    mesh->addFace(11, 9, 8);
    mesh->addFace(11, 10, 9);
    mesh->addFace(11, 6, 10);

    return mesh;
  }

  std::shared_ptr<Mesh> Mesh::dodecahedron()
  {
    std::shared_ptr<Mesh> mesh(new Mesh);

    Real sqrt_5_2 = std::sqrt(5.0) / 2.0;
    Real pi_2_5 = 2.0 * M_PI / 5.0;
    Real pi_5 = M_PI / 5.0;

    Real xyz[] = {
      0.0, 0.0, sqrt_5_2,
      std::cos(0.0 * pi_2_5), std::sin(0.0 * pi_2_5), 0.5,
      std::cos(1.0 * pi_2_5), std::sin(1.0 * pi_2_5), 0.5,
      std::cos(2.0 * pi_2_5), std::sin(2.0 * pi_2_5), 0.5,
      std::cos(3.0 * pi_2_5), std::sin(3.0 * pi_2_5), 0.5,
      std::cos(4.0 * pi_2_5), std::sin(4.0 * pi_2_5), 0.5,
      std::cos(pi_5 + 0.0 * pi_2_5), std::sin(pi_5 + 0.0 * pi_2_5), -0.5,
      std::cos(pi_5 + 1.0 * pi_2_5), std::sin(pi_5 + 1.0 * pi_2_5), -0.5,
      std::cos(pi_5 + 2.0 * pi_2_5), std::sin(pi_5 + 2.0 * pi_2_5), -0.5,
      std::cos(pi_5 + 3.0 * pi_2_5), std::sin(pi_5 + 3.0 * pi_2_5), -0.5,
      std::cos(pi_5 + 4.0 * pi_2_5), std::sin(pi_5 + 4.0 * pi_2_5), -0.5,
      0.0, 0.0, -sqrt_5_2
    };

    GFX::vec3 *verts = reinterpret_cast<GFX::vec3*>(xyz);

    int faces[] = {
      0, 1, 2,
      0, 2, 3,
      0, 3, 4,
      0, 4, 5,
      0, 5, 1,
      1, 6, 2,
      2, 6, 7,
      2, 7, 3,
      3, 7, 8,
      3, 8, 4,
      4, 8, 9,
      4, 9, 5,
      5, 9, 10,
      5, 10, 1,
      1, 10, 6,
      11, 7, 6,
      11, 8, 7,
      11, 9, 8,
      11, 10, 9,
      11, 6, 10
    };

    for (int i = 0; i < 20; ++i) {
      int *face = faces + 3 * i;
      GFX::vec3 point((verts[face[0]] + verts[face[1]] + verts[face[2]]) / 3.0);
      mesh->addVertex(point);
    }

    mesh->addFace(0, 1, 2, 3, 4);
    mesh->addFace(0, 5, 6, 7, 1);
    mesh->addFace(1, 7, 8, 9, 2);
    mesh->addFace(2, 9, 10, 11, 3);

    mesh->addFace(3, 11, 12, 13, 4);
    mesh->addFace(4, 13, 14, 5, 0);
    mesh->addFace(19, 18, 17, 16, 15);
    mesh->addFace(19, 14, 13, 12, 18);

    mesh->addFace(18, 12, 11, 10, 17);
    mesh->addFace(17, 10, 9, 8, 16);
    mesh->addFace(16, 8, 7, 6, 15);
    mesh->addFace(15, 6, 5, 14, 19);

    return mesh;
  }
  
  std::shared_ptr<Mesh> Mesh::buckyball()
  {
    std::shared_ptr<Mesh> icosa = icosahedron();
    std::shared_ptr<Mesh> mesh(new Mesh);

    std::size_t offset = 0;

    // add hexagons
    for (std::size_t i = 0; i < icosa->faces().size(); ++i) {
      const Face &face = icosa->faces()[i];

      const vec4 p1 = icosa->vertices()[face[0]];
      const vec4 p2 = icosa->vertices()[face[1]];
      const vec4 p3 = icosa->vertices()[face[2]];

      const vec4 dir_p1p2 = (p2 - p1) / 3.0;
      const vec4 dir_p2p3 = (p3 - p2) / 3.0;
      const vec4 dir_p3p1 = (p1 - p3) / 3.0;

      const vec4 q1 = p1 +       dir_p1p2;
      const vec4 q2 = p1 + 2.0 * dir_p1p2;
      const vec4 q3 = p2 +       dir_p2p3;
      const vec4 q4 = p2 + 2.0 * dir_p2p3;
      const vec4 q5 = p3 +       dir_p3p1;
      const vec4 q6 = p3 + 2.0 * dir_p3p1;

      std::size_t offset = mesh->vertices().size();

      mesh->addVertex(q1.x(), q1.y(), q1.z());
      mesh->addVertex(q2.x(), q2.y(), q2.z());
      mesh->addVertex(q3.x(), q3.y(), q3.z());
      mesh->addVertex(q4.x(), q4.y(), q4.z());
      mesh->addVertex(q5.x(), q5.y(), q5.z());
      mesh->addVertex(q6.x(), q6.y(), q6.z());

      Face newFace;

      newFace.push_back(offset    );
      newFace.push_back(offset + 1);
      newFace.push_back(offset + 2);
      newFace.push_back(offset + 3);
      newFace.push_back(offset + 4);
      newFace.push_back(offset + 5);

      mesh->addFace(newFace);

      //std::cout << "V: " << mesh->vertices().size() << std::endl;
      //std::cout << "F: " << mesh->faces().size() * mesh->faces()[0].size() << std::endl;
    }

    return mesh;
  }

  std::shared_ptr<Mesh> Mesh::cone(int n, Real h)
  {
    std::shared_ptr<Mesh> mesh(new Mesh);

    Real pi_2_n = 2.0 * M_PI / n;

    for (int i = 0; i < n; ++i)
      mesh->addVertex(std::cos(i * pi_2_n), std::sin(i * pi_2_n), 0.0);

    mesh->addVertex(0.0, 0.0, h);

    for (int i = 0; i < n; ++i)
      mesh->addFace(i, (i + 1) % n, n);

    Face face;
    for (int i = 0; i < n; ++i)
      face.push_back(n - i - 1);
    mesh->addFace(face);

    return mesh;
  }

  std::shared_ptr<Mesh> Mesh::cylinder(int n, Real h)
  {
    std::shared_ptr<Mesh> mesh(new Mesh);
    
    Real pi_2_n = 2.0 * M_PI / n;

    for (int i = 0; i < n; ++i) {
      Real x = std::cos(i * pi_2_n);
      Real y = std::sin(i * pi_2_n);
      mesh->addVertex(x, y, 0.0);
    }
    for (int i = 0; i < n; ++i) {
      Real x = mesh->vertices()[i].x();
      Real y = mesh->vertices()[i].y();
      mesh->addVertex(x, y, h);
    }

    for (int i = 0; i < n; ++i)
      mesh->addFace(i, (i + 1) % n, n + (i + 1) % n, n + i);

    std::vector<int> bottom, top;
    for (int i = 0; i < n; ++i) {
      bottom.push_back(n - i - 1);
      top.push_back(2 * n - i - 1);
    }
    mesh->addFace(bottom);
    mesh->addFace(top);

    return mesh;
  }

  std::shared_ptr<Mesh> Mesh::sphere(int n)
  {
    std::shared_ptr<Mesh> mesh(new Mesh);

    Real sqrt_5_2 = std::sqrt(5.0) / 2.0;
    Real pi_2_5 = 2.0 * M_PI / 5.0;
    Real pi_5 = M_PI / 5.0;

    std::vector<GFX::vec3> verts0;
    verts0.push_back(GFX::vec3(0.0, 0.0, sqrt_5_2));
    verts0.push_back(GFX::vec3(std::cos(0.0 * pi_2_5), std::sin(0.0 * pi_2_5), 0.5));
    verts0.push_back(GFX::vec3(std::cos(1.0 * pi_2_5), std::sin(1.0 * pi_2_5), 0.5));
    verts0.push_back(GFX::vec3(std::cos(2.0 * pi_2_5), std::sin(2.0 * pi_2_5), 0.5));
    verts0.push_back(GFX::vec3(std::cos(3.0 * pi_2_5), std::sin(3.0 * pi_2_5), 0.5));
    verts0.push_back(GFX::vec3(std::cos(4.0 * pi_2_5), std::sin(4.0 * pi_2_5), 0.5));
    verts0.push_back(GFX::vec3(std::cos(pi_5 + 0.0 * pi_2_5), std::sin(pi_5 + 0.0 * pi_2_5), -0.5));
    verts0.push_back(GFX::vec3(std::cos(pi_5 + 1.0 * pi_2_5), std::sin(pi_5 + 1.0 * pi_2_5), -0.5));
    verts0.push_back(GFX::vec3(std::cos(pi_5 + 2.0 * pi_2_5), std::sin(pi_5 + 2.0 * pi_2_5), -0.5));
    verts0.push_back(GFX::vec3(std::cos(pi_5 + 3.0 * pi_2_5), std::sin(pi_5 + 3.0 * pi_2_5), -0.5));
    verts0.push_back(GFX::vec3(std::cos(pi_5 + 4.0 * pi_2_5), std::sin(pi_5 + 4.0 * pi_2_5), -0.5));
    verts0.push_back(GFX::vec3(0.0, 0.0, -sqrt_5_2));

    std::vector<GFX::Mesh::Face> faces0;
    faces0.push_back(GFX::Mesh::make_face(0, 1, 2));
    faces0.push_back(GFX::Mesh::make_face(0, 2, 3));
    faces0.push_back(GFX::Mesh::make_face(0, 3, 4));
    faces0.push_back(GFX::Mesh::make_face(0, 4, 5));
    faces0.push_back(GFX::Mesh::make_face(0, 5, 1));
    faces0.push_back(GFX::Mesh::make_face(1, 6, 2));
    faces0.push_back(GFX::Mesh::make_face(2, 6, 7));
    faces0.push_back(GFX::Mesh::make_face(2, 7, 3));
    faces0.push_back(GFX::Mesh::make_face(3, 7, 8));
    faces0.push_back(GFX::Mesh::make_face(3, 8, 4));
    faces0.push_back(GFX::Mesh::make_face(4, 8, 9));
    faces0.push_back(GFX::Mesh::make_face(4, 9, 5));
    faces0.push_back(GFX::Mesh::make_face(5, 9, 10));
    faces0.push_back(GFX::Mesh::make_face(5, 10, 1));
    faces0.push_back(GFX::Mesh::make_face(1, 10, 6));
    faces0.push_back(GFX::Mesh::make_face(11, 7, 6));
    faces0.push_back(GFX::Mesh::make_face(11, 8, 7));
    faces0.push_back(GFX::Mesh::make_face(11, 9, 8));
    faces0.push_back(GFX::Mesh::make_face(11, 10, 9));
    faces0.push_back(GFX::Mesh::make_face(11, 6, 10));

    std::vector<GFX::vec3> verts1;
    std::vector<GFX::Mesh::Face> faces1;

    for (int i = 0; i < n; ++i) {
      verts1.clear();
      faces1.clear();

      for (std::size_t j = 0; j < faces0.size(); ++j) {
        const GFX::Mesh::Face &face = faces0[j];

        GFX::vec3 A(verts0[face[0]]);
        GFX::vec3 B(verts0[face[1]]);
        GFX::vec3 C(verts0[face[2]]);

        GFX::vec3 D((A + B) / 2.0);
        GFX::vec3 E((A + C) / 2.0);
        GFX::vec3 F((B + C) / 2.0);

        int v = verts1.size();

        verts1.push_back(A);
        verts1.push_back(B);
        verts1.push_back(C);
        verts1.push_back(D);
        verts1.push_back(E);
        verts1.push_back(F);

        faces1.push_back(GFX::Mesh::make_face(v    , v + 3, v + 4));
        faces1.push_back(GFX::Mesh::make_face(v + 1, v + 5, v + 3));
        faces1.push_back(GFX::Mesh::make_face(v + 2, v + 4, v + 5));
        faces1.push_back(GFX::Mesh::make_face(v + 3, v + 5, v + 4));
      }

      verts1.swap(verts0);
      faces1.swap(faces0);
    }

    for (std::size_t i = 0; i < verts0.size(); ++i)
      verts0[i].normalize();

    for (std::size_t i = 0; i < verts0.size(); ++i)
      mesh->addVertex(verts0[i]);
    for (std::size_t i = 0; i < faces0.size(); ++i)
      mesh->addFace(faces0[i]);

    return mesh;
  }

  std::shared_ptr<Mesh> Mesh::torus(int n, int m, Real R, Real r)
  {
    std::shared_ptr<Mesh> mesh(new Mesh);

    for (int i = 0; i < n; ++i)
      for (int j = 0; j < m; ++j) {
        Real u = 2.0 * i * M_PI / n;
        Real v = 2.0 * j * M_PI / m;
        Real x = (R + r * std::cos(v)) * std::cos(u);
        Real y = (R + r * std::cos(v)) * std::sin(u);
        Real z = r * std::sin(v);
        mesh->addVertex(x, y, z);

        int p0 = columnMajorIndex(i          , j          , m);
        int p1 = columnMajorIndex((i + 1) % n, j          , m);
        int p2 = columnMajorIndex((i + 1) % n, (j + 1) % m, m);
        int p3 = columnMajorIndex(i          , (j + 1) % m, m);
        mesh->addFace(p0, p1, p2, p3);
      }

    return mesh;
  }

}