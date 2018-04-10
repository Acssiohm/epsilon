#include "parenthesis_left_layout.h"
#include <escher/metric.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

const uint8_t topLeftCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
};

const uint8_t bottomLeftCurve[ParenthesisLeftRightLayout::k_parenthesisCurveHeight][ParenthesisLeftRightLayout::k_parenthesisCurveWidth] = {
  {0x11, 0xEE, 0xFF, 0xFF, 0xFF},
  {0x45, 0xBE, 0xFF, 0xFF, 0xFF},
  {0xA9, 0x5A, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x49, 0xB6, 0xFF, 0xFF},
  {0xFF, 0xF2, 0x40, 0xBF, 0xFF},
  {0xFF, 0xFF, 0xEB, 0x40, 0x9A},
  {0xFF, 0xFF, 0xFF, 0xF9, 0x66},
};

ExpressionLayout * ParenthesisLeftLayout::clone() const {
  ParenthesisLeftLayout * layout = new ParenthesisLeftLayout();
  return layout;
}

bool ParenthesisLeftLayout::isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const {
  if (*numberOfOpenParenthesis == 0 && goingLeft) {
    return false;
  }
  *numberOfOpenParenthesis = goingLeft ? *numberOfOpenParenthesis - 1 : *numberOfOpenParenthesis + 1;
  return true;
}

void ParenthesisLeftLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDRect frame(p.x()+ParenthesisLeftRightLayout::k_externWidthMargin,
      p.y()+ParenthesisLeftRightLayout::k_externHeightMargin,
      ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)topLeftCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  frame = KDRect(p.x()+ParenthesisLeftRightLayout::k_externWidthMargin,
      p.y() + operandHeight() - ParenthesisLeftRightLayout::k_parenthesisCurveHeight - ParenthesisLeftRightLayout::k_externHeightMargin,
      ParenthesisLeftRightLayout::k_parenthesisCurveWidth,
      ParenthesisLeftRightLayout::k_parenthesisCurveHeight);

  ctx->blendRectWithMask(frame, expressionColor, (const uint8_t *)bottomLeftCurve, (KDColor *)(ParenthesisLeftRightLayout::s_parenthesisWorkingBuffer));

  ctx->fillRect(KDRect(p.x()+ParenthesisLeftRightLayout::k_externWidthMargin,
        p.y()+ParenthesisLeftRightLayout::k_parenthesisCurveHeight+ParenthesisLeftRightLayout::k_externHeightMargin,
        ParenthesisLeftRightLayout::k_lineThickness,
        operandHeight() - 2*(ParenthesisLeftRightLayout::k_parenthesisCurveHeight+ParenthesisLeftRightLayout::k_externHeightMargin)),
      expressionColor);
}

void ParenthesisLeftLayout::computeOperandHeight() {
  assert(m_parent != nullptr);
  m_operandHeight = Metric::MinimalBracketAndParenthesisHeight;
  KDCoordinate max_under_baseline = 0;
  KDCoordinate max_above_baseline = 0;
  int indexInParent = m_parent->indexOfChild(this);
  int currentNumberOfOpenParentheses = 1;
  int numberOfBrothers = m_parent->numberOfChildren();
  if (indexInParent < numberOfBrothers - 1
      && m_parent->child(indexInParent + 1)->mustHaveLeftBrother())
  {
    // If the parenthesis is the base of a superscript layout, it should have a
    // default height, else it creates an infinite loop because the parenthesis
    // needs the superscript height, which needs the parenthesis height.
    return;
  }
  for (int i = indexInParent + 1; i < numberOfBrothers; i++) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isRightParenthesis()) {
      currentNumberOfOpenParentheses--;
      if (currentNumberOfOpenParentheses == 0) {
        if (max_under_baseline + max_above_baseline > m_operandHeight) {
          m_operandHeight = max_under_baseline + max_above_baseline;
        }
        return;
      }
    } else if (brother->isLeftParenthesis()) {
      currentNumberOfOpenParentheses++;
    }
    KDCoordinate brotherHeight = brother->size().height();
    KDCoordinate brotherBaseline = brother->baseline();
    if (brotherHeight - brotherBaseline > max_under_baseline) {
      max_under_baseline = brotherHeight - brotherBaseline ;
    }
    if (brotherBaseline > max_above_baseline) {
      max_above_baseline = brotherBaseline;
    }
  }
  if (max_under_baseline + max_above_baseline > m_operandHeight) {
    m_operandHeight = max_under_baseline + max_above_baseline;
  }
}

void ParenthesisLeftLayout::computeBaseline() {
  assert(m_parent != nullptr);
  int currentNumberOfOpenParentheses = 1;
  int indexInParent = m_parent->indexOfChild(this);
  int numberOfBrothers = m_parent->numberOfChildren();
  if (indexInParent == numberOfBrothers - 1) {
    // The parenthesis is the rightmost child of its parent.
    m_baseline = operandHeight()/2;
    m_baselined = true;
    return;
  }
  if (m_parent->child(indexInParent + 1)->mustHaveLeftBrother()) {
    // If the parenthesis is the base of a superscript layout, it should have a
    // default baseline, else it creates an infinite loop because the
    // parenthesis needs the superscript height, which needs the parenthesis
    // baseline.
    m_baseline = operandHeight()/2;
    m_baselined = true;
    return;
  }

  m_baseline = 0;
  for (int i = indexInParent + 1; i < numberOfBrothers; i++) {
    ExpressionLayout * brother = m_parent->editableChild(i);
    if (brother->isRightParenthesis()) {
      if (i == indexInParent + 1) {
        // If the parenthesis is immediately closed, we set the baseline to half
        // the parenthesis height.
        m_baseline = operandHeight()/2;
        break;
      }
      currentNumberOfOpenParentheses--;
      if (currentNumberOfOpenParentheses == 0) {
        break;
      }
    } else if (brother->isLeftParenthesis()) {
      currentNumberOfOpenParentheses++;
    }
    if (brother->baseline() > m_baseline) {
      m_baseline = brother->baseline();
    }
  }
  m_baselined = true;
}

}
