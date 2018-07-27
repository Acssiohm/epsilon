#ifndef POINCARE_EMPTY_LAYOUT_NODE_H
#define POINCARE_EMPTY_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class EmptyLayoutNode : public LayoutNode {
public:
  enum class Color {
    Yellow,
    Grey
  };

  EmptyLayoutNode(Color color = Color::Yellow, bool visible = true, KDText::FontSize fontSize = KDText::FontSize::Large, bool margins = true) :
    LayoutNode(),
    m_isVisible(visible),
    m_color(color),
    m_fontSize(fontSize),
    m_margins(margins)
  {}

  // EmptyLayout
  Color color() const { return m_color; }
  void setColor(Color color) { m_color = color; }
  bool isVisible() const { return m_isVisible; }
  void setVisible(bool visible) { m_isVisible = visible; }
  void setMargins(bool margins) { m_margins = margins; }
  void setFontSize(KDText::FontSize fontSize) { m_fontSize = fontSize; }

  // LayoutNode
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool isEmpty() const override { return true; }

  // TreeNode
  size_t size() const override { return sizeof(EmptyLayoutNode); }
#if TREE_LOG
  const char * description() const override { return "Empty"; }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }
private:
  constexpr static KDCoordinate k_marginWidth = 1;
  constexpr static KDCoordinate k_marginHeight = 3;
  constexpr static KDCoordinate k_lineThickness = 1;
  KDCoordinate height() const { return KDText::charSize(m_fontSize).height() - 2*k_marginHeight; }
  KDCoordinate width() const { return KDText::charSize(m_fontSize).width() - 2*k_marginWidth; }

  // LayoutNode
  void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) override;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;

  bool m_isVisible;
  Color m_color;
  KDText::FontSize m_fontSize;
  bool m_margins;
};

class EmptyLayoutRef : public LayoutReference {
public:
  EmptyLayoutRef(TreeNode * n) : LayoutReference(n) {}
  EmptyLayoutRef(EmptyLayoutNode::Color color = EmptyLayoutNode::Color::Yellow, bool visible = true, KDText::FontSize fontSize = KDText::FontSize::Large, bool margins = true) :
    LayoutReference()
  {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<EmptyLayoutNode>();
    m_identifier = node->identifier();
    if (!(node->isAllocationFailure())) {
      EmptyLayoutNode * castedNode = static_cast<EmptyLayoutNode *>(node);
      castedNode->setColor(color);
      castedNode->setVisible(visible);
      castedNode->setFontSize(fontSize);
      castedNode->setMargins(margins);
    }
  }
  void setVisible(bool visible) {
    if (!(node()->isAllocationFailure())) {
      static_cast<EmptyLayoutNode *>(node())->setVisible(visible);
    }
  }

  void setColor(EmptyLayoutNode::Color color) {
    if (!(node()->isAllocationFailure())) {
      static_cast<EmptyLayoutNode *>(node())->setColor(color);
    }
  }
};

}

#endif
