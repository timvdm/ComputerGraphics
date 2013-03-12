#ifndef CG_XY_SVGSTYLE_H
#define CG_XY_SVGSTYLE_H

namespace CG {

  namespace XY {

    class SvgStyle
    {
      public:
        enum StrokeLinecapType
        {
          ButtLinecap,
          RoundLinecap,
          SquareLinecap
        };

        enum StrokeLinejoinType
        {
          MiterLinejoin,
          RoundLinejoin,
          BevelLinejoin
        };

        enum FillRuleType
        {
          NonZeroFillRule,
          EvenOddFillRule
        };

        struct Stroke
        {
          Stroke() : ignore(true)
          {
          }

          Stroke(const Color &value_) : value(value_), ignore(false)
          {
          }

          Color value;
        };

        struct StrokeWidth
        {
          StrokeWidth(double value_ = 1.0) : value(value_)
          {
          }

          double value;
        };

        struct StrokeLinecap
        {
          StrokeLinecap(StrokeLinecapType value_ = ButtLinecap) : value(value_)
          {
          }

          StrokeLinecapType value;
        };

        struct StrokeLinejoin
        {
          StrokeLinejoin(StrokeLinejoinType value_ = MiterLinejoinType) : value(value_)
          {
          }

          StrokeLinejoinType value;
        };

        struct StrokeMiterlimit
        {
          StrokeMiterlimit(double value_ = 4.0) : value(value_)
          {
          }

          double value;
        };

        struct Fill
        {
          Fill(const Color &value_ = Color::black()) : value(value_)
          {
          }

          Color value;
        };

        struct FillRule
        {
          FillRule(FillRuleType &value_ = NonZero) : value(value_)
          {
          }

          FillRuleType value;
        };

        SvgStyle(const Stroke &stroke = Stroke(),
                 const StrokeWidth &strokeWidth = StrokeWidth(),
                 const StrokeLinecap &strokeLinecap = StrokeLinecap(),
                 const StrokeLinejoin &strokeLinejoin = StrokeLinejoin(),
                 const StrokeMiterlimit &strokeMiterlimit = StrokeMiterlimit(),
                 const Fill &fill = Fill(),
                 const FillRule &fillRule = FillRule())
          : m_stroke(stroke), m_strokeWidth(strokeWidth), m_stroke_linecap(strokeLinecap),
            m_strokeLinejoin(strokeLinejoin), m_strokeMiterlimit(strokeMiterlimit),
            m_fill(fill), m_fillRule(fillRule)
        {
        }

        const Stroke& stroke() const
        {
          return m_stroke;
        }

        void setStroke(const Stroke &stroke)
        {
          m_stroke = stroke;
        }

        const StrokeWidth& strokeWidth() const
        {
          return m_strokeWidth;
        }

        void setStrokeWidth(const StrokeWidth &strokeWidth)
        {
          m_strokeWidth = strokeWidth;
        }

        const StrokeLinecap& strokeLinecap() const
        {
          return m_strokeLinecap;
        }

        void setStrokeLinecap(const Stroke &strokeLinecap)
        {
          m_strokeLinecap = strokeLinecap;
        }

        const StrokeLinejoin& strokeLinejoin() const
        {
          return m_strokeLinejoin;
        }

        void setStrokeLinejoin(const StrokeLinejoin &strokeLinejoin)
        {
          m_strokeLinejoin = strokeLinejoin;
        }

        const StrokeMiterlimit& strokeMiterlimit() const
        {
          return m_strokeMiterlimit;
        }

        void setStrokeMiterlimit(const StrokeMiterlimit &strokeMiterlimit)
        {
          m_strokeMiterlimit = strokeMiterlimit;
        }

        const Fill& fill() const
        {
          return m_fill;
        }

        void setFill(const Fill &fill)
        {
          m_fill = fill;
        }

        const FillRule& fillRule() const
        {
          return m_fillRule;
        }

        void setFillRule(const FillRule &fillRule)
        {
          m_fillRule = fillRule;
        }

      private:
        Stroke m_stroke;
        StrokeWidth m_strokeWidth;
        StrokeLinecap m_strokeLinecap;
        StrokeLinejoin m_strokeLinejoin;
        StrokeMiterlimit m_strokeMiterlimit;
        Fill m_fill;
        FillRule m_fillRule;
    };

  }

}

#endif
