#ifndef POINCARE_TREE_REFERENCE_H
#define POINCARE_TREE_REFERENCE_H

#include "tree_pool.h"
#include <stdio.h>

namespace Poincare {

template <typename T>
class TreeReference {
  friend class TreeNode;
  friend class AdditionNode;
  friend class ExpressionNode;
  template <typename U>
  friend class TreeReference;
  template <typename U>
  friend class ExpressionReference;
  template <typename U>
  friend class LayoutReference;
public:
  TreeReference(const TreeReference & tr) : m_identifier(TreePool::NoNodeIdentifier) { setTo(tr); }
  TreeReference(TreeReference&& tr) : m_identifier(TreePool::NoNodeIdentifier) { setTo(tr); }
  TreeReference& operator=(const TreeReference& tr) {
    setTo(tr);
    return *this;
  }
  TreeReference& operator=(TreeReference&& tr) {
    setTo(tr);
    return *this;
  }

  inline bool operator==(TreeReference<TreeNode> t) { return m_identifier == t.identifier(); }

  TreeReference<T> clone() const {
    if (!isDefined()){
      return TreeReference<T>(nullptr);
    }
    TreeNode * myNode = node();
    if (myNode->isAllocationFailure()) {
      int allocationFailureNodeId = myNode->allocationFailureNodeIdentifier();
      return TreeReference<T>(TreePool::sharedPool()->node(allocationFailureNodeId));
    }
    TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(myNode);
    nodeCopy->deepResetReferenceCounter();
    return TreeReference<T>(nodeCopy);
  }

  ~TreeReference() {
    if (isDefined()) {
      assert(node());
      assert(node()->identifier() == m_identifier);
      node()->release();
    }
  }
  operator TreeReference<TreeNode>() const { return TreeReference<TreeNode>(this->node()); }

  int identifier() const { return m_identifier; }
  TreeNode * node() const { return TreePool::sharedPool()->node(m_identifier); }
  T * typedNode() const {
    // TODO: Here, assert that the node type is indeed T
    // ?? Might be allocation failure, not T
    return static_cast<T*>(node());
  }

  bool isDefined() const { return m_identifier != TreePool::NoNodeIdentifier && node() != nullptr; }
  bool isAllocationFailure() const { return isDefined() && node()->isAllocationFailure(); }

  int nodeRetainCount() const {
    assert(isDefined());
    return node()->retainCount();
  }
  void incrementNumberOfChildren(int increment = 1) {
    assert(isDefined());
    node()->incrementNumberOfChildren(increment);
  }
  void decrementNumberOfChildren(int decrement = 1) {
    assert(isDefined());
    node()->decrementNumberOfChildren(decrement);
  }
  int numberOfDescendants(bool includeSelf) const {
    assert(isDefined());
    return node()->numberOfDescendants(includeSelf);
  }

  // Hierarchy
  bool hasChild(TreeReference<TreeNode> t) const {
    assert(isDefined());
    return node()->hasChild(t.node());
  }
  bool hasSibling(TreeReference<TreeNode> t) const {
    assert(isDefined());
    return node()->hasSibling(t.node());
  }
  int numberOfChildren() const {
    assert(isDefined());
    return node()->numberOfChildren();
  }
  TreeReference<T> parent() const {
    assert(isDefined());
    return TreeReference(node()->parentTree());
  }
  TreeReference<T> treeChildAtIndex(int i) const {
    assert(isDefined());
    return TreeReference(node()->childTreeAtIndex(i));
  }
  int indexOfChild(TreeReference<TreeNode> t) const {
    assert(isDefined());
    return node()->indexOfChild(t.node());
  }


  // Hierarchy operations

  void addChildTreeAtIndex(TreeReference<TreeNode> t, int index) {
    assert(isDefined());
    if (node()->isAllocationFailure()) {
      return;
    }
    if (t.isAllocationFailure()) {
      replaceWithAllocationFailure();
      return;
    }
    assert(index >= 0 && index <= numberOfChildren());

    // Retain t before detaching it, else it might get destroyed
    t.node()->retain();

    // Detach t from its parent
    TreeReference<TreeNode> tParent = t.parent();
    if (tParent.isDefined()) {
      tParent.removeTreeChild(t);
    }

    // Move t
    TreeNode * newChildPosition = node()->next();
    for (int i = 0; i < index; i++) {
      newChildPosition = newChildPosition->nextSibling();
    }
    TreePool::sharedPool()->move(newChildPosition, t.node());
    node()->incrementNumberOfChildren();
  }
  void removeTreeChildAtIndex(int i) {
    assert(isDefined());
    assert(i >= 0 && i < numberOfChildren());
    TreeReference<TreeNode> t = treeChildAtIndex(i);
    removeTreeChild(t);
  }

  void removeTreeChild(TreeReference<TreeNode> t) {
    assert(isDefined());
    TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node());
    t.node()->release();
    node()->decrementNumberOfChildren();
  }

  void removeChildren() {
    assert(isDefined());
    node()->releaseChildren();
    TreePool::sharedPool()->moveChildren(TreePool::sharedPool()->last(), node());
    node()->eraseNumberOfChildren();
  }

  void replaceWith(TreeReference<TreeNode> t) {
    assert(isDefined());
    TreeReference<TreeNode> p = parent();
    if (p.isDefined()) {
      p.replaceTreeChildAtIndex(p.node()->indexOfChildByIdentifier(identifier()), t);
    }
  }

  void replaceTreeChild(TreeReference<TreeNode> oldChild, TreeReference<TreeNode> newChild) {
    replaceTreeChildAtIndex(indexOfChild(oldChild), newChild);
  }

  void replaceTreeChildAtIndex(int oldChildIndex, TreeReference<TreeNode> newChild) {
    assert(isDefined());
    if (newChild.isAllocationFailure()) {
      replaceWithAllocationFailure();
      return;
    }
    TreeReference<TreeNode> p = newChild.parent();
    if (p.isDefined()) {
      p.decrementNumberOfChildren();
    }
    assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
    TreeReference<T> oldChild = treeChildAtIndex(oldChildIndex);
    TreePool::sharedPool()->move(oldChild.node()->nextSibling(), newChild.node());
    if (!p.isDefined()) {
      newChild.node()->retain();
    }
    TreePool::sharedPool()->move(TreePool::sharedPool()->last(), oldChild.node());
    oldChild.node()->release();
  }

  void replaceWithAllocationFailure() {
    assert(isDefined());
    TreeReference<TreeNode> p = parent();
    bool hasParent = p.isDefined();
    int indexInParentNode = hasParent ? node()->indexInParent() : -1;
    int currentRetainCount = node()->retainCount();
    TreeNode * staticAllocFailNode = typedNode()->failedAllocationStaticNode();

    // Move the node to the end of the pool and decrease children count of parent
    TreePool::sharedPool()->move(TreePool::sharedPool()->last(), node());
    if (p.isDefined()) {
      p.decrementNumberOfChildren();
    }

    // Release all children and delete the node in the pool
    node()->releaseChildrenAndDestroy();

    /* Create an allocation failure node with the previous node id. We know
     * there is room in the pool as we deleted the previous node and an
     * AllocationFailure nodes size is smaller or equal to any other node size.*/
    //TODO static assert that the size is smaller
    TreeNode * newAllocationFailureNode = TreePool::sharedPool()->deepCopy(staticAllocFailNode);
    newAllocationFailureNode->rename(m_identifier);
    if (p.isDefined()) {
      assert(indexInParentNode >= 0);
      /* Set the refCount to previousRefCount-1 because the previous parent is
       * no longer retaining the node. When we add this node to the parent, it
       * will retain it and increment the retain count. */
      newAllocationFailureNode->setReferenceCounter(currentRetainCount - 1);
      p.addChildTreeAtIndex(newAllocationFailureNode, indexInParentNode);
    } else {
      newAllocationFailureNode->setReferenceCounter(currentRetainCount);
    }
  }

  void swapChildren(int i, int j) {
    assert(isDefined());
    assert(i >= 0 && i < numberOfChildren());
    assert(j >= 0 && j < numberOfChildren());
    if (i == j) {
      return;
    }
    int firstChildIndex = i < j ? i : j;
    int secondChildIndex = i > j ? i : j;
    TreeReference<T> firstChild = treeChildAtIndex(firstChildIndex);
    TreeReference<T> secondChild = treeChildAtIndex(secondChildIndex);
    TreePool::sharedPool()->move(firstChild.node()->nextSibling(), secondChild.node());
    TreePool::sharedPool()->move(treeChildAtIndex(secondChildIndex).nextSibling(), firstChild.node());
  }

  void mergeTreeChildrenAtIndex(TreeReference<T> t, int i) {
    assert(i >= 0 && i <= numberOfChildren());
    // Steal operands
    int numberOfNewChildren = t.numberOfChildren();
    if (i < numberOfChildren()) {
      TreePool::sharedPool()->moveChildren(node()->childTreeAtIndex(i), t.node());
    } else {
      TreePool::sharedPool()->moveChildren(node()->lastDescendant()->next(), t.node());
    }
    t.node()->eraseNumberOfChildren();
    // If t is a child, remove it
    if (node()->hasChild(t.node())) {
      removeTreeChild(t);
    }
    node()->incrementNumberOfChildren(numberOfNewChildren);
  }

  TreeReference(TreeNode * node) { // TODO Make this protected
    if (node == nullptr) {
      m_identifier = TreePool::NoNodeIdentifier;
    } else {
      setIdentifierAndRetain(node->identifier());
    }
  }

protected:
  TreeReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<T>();
    m_identifier = node->identifier();
  }

  void setIdentifierAndRetain(int newId) {
    m_identifier = newId;
    node()->retain();
  }
private:
  void setTo(const TreeReference & tr) {
    /* We cannot use (*this)==tr because tr would need to be casted to
     * TreeReference<TreeNode>, which calls setTo and triggers an infinite loop */
    if (identifier() == tr.identifier()) {
      return;
    }
    TreeNode * currentNode = node();
    bool releaseNode = isDefined();
    if (tr.isDefined()) {
      setIdentifierAndRetain(tr.identifier());
    } else {
      m_identifier = TreePool::NoNodeIdentifier;
    }
    if (releaseNode) {
      currentNode->release();
    }
  }
  int m_identifier;
};

typedef TreeReference<TreeNode> TreeRef;

}

#endif
