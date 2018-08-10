#ifndef POINCARE_CHAR_LAYOUT_NODE_H
#define POINCARE_CHAR_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class CharLayoutNode : public LayoutNode {
public:
  CharLayoutNode(char c = 'a', KDText::FontSize fontSize = KDText::FontSize::Large) :
    LayoutNode(),
    m_char(c),
    m_fontSize(fontSize)
  {}

  // CharLayout
  void setChar(char c) { m_char = c; }
  KDText::FontSize fontSize() const { return m_fontSize; }
  void setFontSize(KDText::FontSize fontSize) { m_fontSize = fontSize; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;

  // AllocationFailure
  static CharLayoutNode * FailedAllocationStaticNode();
  CharLayoutNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // TreeNode
  size_t size() const override { return sizeof(CharLayoutNode); }
  int numberOfChildren() const override { return 0; }
#if TREE_LOG
  const char * description() const override {
    static char Description[] = "Char a";
    Description[5] = m_char;
    return Description;
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  char m_char;
  KDText::FontSize m_fontSize;
};

class CharLayoutRef : public LayoutReference {
public:
  CharLayoutRef(char c, KDText::FontSize fontSize = KDText::FontSize::Large) :
    LayoutReference(TreePool::sharedPool()->createTreeNode<CharLayoutNode>())
  {
    node()->setChar(c);
    node()->setFontSize(fontSize);
  }

  KDText::FontSize fontSize() const { return const_cast<CharLayoutRef *>(this)->node()->fontSize(); }
private:
  CharLayoutNode * node() { return static_cast<CharLayoutNode *>(LayoutReference::node());}
};

}

#endif
