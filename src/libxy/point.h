#ifndef CG_XY_POINT_H
#define CG_XY_POINT_H

namespace CG {

  namespace XY {

    template<typename Scalar>
    struct Point
    {
      typedef Scalar scalar_type;

      Point(Scalar xx = Scalar(), Scalar yy = Scalar()) : x(xx), y(yy)
      {
      }

      Scalar x;
      Scalar y;
    };

    typedef Point<int> Pointi;
    typedef Point<double Pointd;

  }

}

#endif
