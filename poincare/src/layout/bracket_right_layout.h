#ifndef POINCARE_BRACKET_RIGHT_LAYOUT_H
#define POINCARE_BRACKET_RIGHT_LAYOUT_H

#include <poincare/src/layout/bracket_left_right_layout.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class BracketRightLayout : public BracketLeftRightLayout {
  friend class MatrixLayout;
public:
  using BracketLeftRightLayout::BracketLeftRightLayout;
  ExpressionLayout * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, ']');
  }
  bool isRightBracket() const override { return true; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  void computeOperandHeight() override;
  void computeBaseline() override;
};

}

#endif
