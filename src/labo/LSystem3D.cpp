#include "LSystem3D.h"

#include <libgfx/mesh.h>
#include <libgfx/utility.h>

#include <fstream>
#include <cmath>
#include <memory>

namespace CG {

  std::shared_ptr<GFX::Mesh> LSystem3D::generateMesh(const std::string &filename)
  {
    // parse L3D file
    LParser::LSystem3D lSystem;
    std::ifstream ifs(filename.c_str());
    assert(ifs);
    try {
      ifs >> lSystem;
    } catch (const std::exception &e) {
      std::cout << 1 << std::endl;
      std::cerr << e.what() << std::endl;
      return 0;
    }
    ifs.close();

    // convert LSystem3D to a mesh
    std::shared_ptr<GFX::Mesh> mesh(new GFX::Mesh);
    drawLSystem(lSystem, *mesh);

    return mesh;
  }

  bool LSystem3D::processCommand(char command, const LParser::LSystem3D &lSystem, LSystemState &state)
  {
    GFX::Real delta = GFX::deg2rad(lSystem.get_angle());

    switch (command) {
      case '+':
        {
          // roll left around U axis
          GFX::vec4 newH =  state.H * std::cos(delta) + state.L * std::sin(delta);
          state.L        = -state.H * std::sin(delta) + state.L * std::cos(delta);
          state.H = newH;
        }
        return true;
      case '-':
        {
          // roll right around U axis
          GFX::vec4 newH =  state.H * std::cos(-delta) + state.L * std::sin(-delta);
          state.L        = -state.H * std::sin(-delta) + state.L * std::cos(-delta);
          state.H = newH;
        }
        return true;
      case '^':
        {
          // roll up around L axis
          GFX::vec4 newH =  state.H * std::cos(delta) + state.U * std::sin(delta);
          state.U        = -state.H * std::sin(delta) + state.U * std::cos(delta);
          state.H = newH;
        }
        return true;
      case '&':
        {
          // roll down around L axis
          GFX::vec4 newH =  state.H * std::cos(-delta) + state.U * std::sin(-delta);
          state.U        = -state.H * std::sin(-delta) + state.U * std::cos(-delta);
          state.H = newH;
        }
        return true;
      case '\\':
        {
          // roll left around H axis
          GFX::vec4 newL =  state.L * std::cos(delta) - state.U * std::sin(delta);
          state.U        =  state.L * std::sin(delta) + state.U * std::cos(delta);
          state.L = newL;
        }
        return true;
      case '/':
        {
          // roll right around H axis
          GFX::vec4 newL =  state.L * std::cos(-delta) - state.U * std::sin(-delta);
          state.U        =  state.L * std::sin(-delta) + state.U * std::cos(-delta);
          state.L = newL;
        }
        return true;
      case '|':
        {
          // turn around, leave H unchanged
          state.H = -state.H;
          state.L = -state.L;
        }
        return true;
      case '(':
        state.push();
        return true;
      case ')':
        state.pop();
        return true;
    }

    return false;
  }

  void LSystem3D::drawLSystem(const LParser::LSystem3D &lSystem, const std::string &commands, GFX::Mesh &mesh, int iteration, LSystemState &state)
  {
    if (iteration > 0) {
      // recursive calls...
      for (std::size_t i = 0; i < commands.size(); ++i)
        if (!processCommand(commands[i], lSystem, state))
          drawLSystem(lSystem, lSystem.get_replacement(commands[i]), mesh, iteration - 1, state);
    } else {
      // reached deepest recursive level -> draw the lines
      for (std::size_t i = 0; i < commands.size(); ++i) {
        if (!processCommand(commands[i], lSystem, state)) {
          // compute new position
          GFX::vec4 newPos = state.pos + state.H;
          newPos.w() = 1.0;
          // draw a line?
          if (lSystem.draw(commands[i])) {
            int v = mesh.vertices().size();
            mesh.addFace(v, v + 1);
            mesh.addVertex(state.pos);
            mesh.addVertex(newPos);
          }
          // move to new position
          state.pos = newPos;
        }
      }
    }
  }

  void LSystem3D::drawLSystem(const LParser::LSystem3D &lSystem, GFX::Mesh &mesh)
  {
    LSystemState state;
    drawLSystem(lSystem, lSystem.get_initiator(), mesh, lSystem.get_nr_iterations(), state);
  }

}
