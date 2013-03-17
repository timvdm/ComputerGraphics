#ifndef INC_LSYSTEM3D_H
#define INC_LSYSTEM3D_H

#include "../libgfx/mesh.h"
#include "../utils.h"

#include <memory>

namespace CG {

  class LSystem3D
  {
    public:
      static std::shared_ptr<GFX::Mesh> generateMesh(const std::string &filename);

    private:
      struct LSystemState
      {
        LSystemState() : pos(0, 0, 0, 1), H(1, 0, 0, 0), L(0, 1, 0, 0), U(0, 0, 1, 0)
        {
        }

        void push()
        {
          stack.push_back(std::make_tuple(H, L, U, pos));
        }

        void pop()
        {
          H = std::get<0>(stack.back());
          L = std::get<1>(stack.back());
          U = std::get<2>(stack.back());
          pos = std::get<3>(stack.back());
          stack.pop_back();
        }

        GFX::vec4 H;
        GFX::vec4 L;
        GFX::vec4 U;
        GFX::vec4 pos;
        std::vector<std::tuple<GFX::vec4, GFX::vec4, GFX::vec4, GFX::vec4> > stack;
      };

      static bool processCommand(char command, const LParser::LSystem3D &lSystem, LSystemState &state);

      static void drawLSystem(const LParser::LSystem3D &lSystem, const std::string &commands, GFX::Mesh &mesh, int iteration, LSystemState &state);
      
      static void drawLSystem(const LParser::LSystem3D &lSystem, GFX::Mesh &mesh);
  };

}

#endif
