#ifndef POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILED_LAYOUT_NODE_H

#include "layout_node.h"
#include "layout_reference.h"
#include "layout_cursor.h"

namespace Poincare {

class AllocationFailedLayoutNode : public LayoutNode {
public:
  // LayoutNode
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    assert(false);
    return 0;
  }
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override { return LayoutCursor(); }
  void deleteBeforeCursor(LayoutCursor * cursor) override { }

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedLayoutNode); }
  const char * description() const override { return "Allocation Failed";  }
  bool isAllocationFailure() const override { return true; }

protected:
  // LayoutNode
  void computeSize() override { m_sized = true; }
  void computeBaseline() override { m_baselined = true; }
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }

private:
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override { return false; }
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override { return false; }
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override { return false; }

  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
};

class AllocationFailedLayoutRef : public LayoutReference<AllocationFailedLayoutNode> {
public:
  AllocationFailedLayoutRef() : LayoutReference<AllocationFailedLayoutNode>() {}
  AllocationFailedLayoutRef(TreeNode * aNode) : LayoutReference<AllocationFailedLayoutNode>(aNode) {}
};

}

#endif
