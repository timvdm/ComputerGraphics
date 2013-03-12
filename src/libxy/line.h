#ifndef CG_XY_POINT_H
#define CG_XY_POINT_H

#include "point.h"

namespace CG {

  namespace XY {

    template<typename Scalar>
    struct Line
    {
      Line(const Point<Scalar> &p1_, const Point<Scalar> &p2_, Scalar width_ = 1)
        : p1(p1_), p2(p2_), width(width_)
      {
      }

      Point<Scalar> p1;
      Point<Scalar> p2;
      Scalar width;
    };

  }

}

#endif
