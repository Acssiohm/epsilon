#include <poincare/tree_pool.h>
#include <string.h>
#include <stdint.h>

namespace Poincare {

TreePool * TreePool::sharedPool() {
  static TreePool pool;
  return &pool;
}

TreeNode * TreePool::node(int identifier) const {
  if (identifier == NoNodeIdentifier) {
    return nullptr;
  }
  if (identifier < 0) {
    int index = indexOfStaticNode(identifier);
    assert(index >= 0 && index < MaxNumberOfStaticNodes);
    return m_staticNodes[index];
  }
  assert(identifier >= 0 && identifier <= MaxNumberOfNodes);
  return m_nodeForIdentifier[identifier];
}

static void memmove32(uint32_t * dst, uint32_t * src, size_t len) {
  if (src < dst && dst < src + len) {
    /* Copy backwards to avoid overwrites */
    src += len;
    dst += len;
    while (len--) {
      *--dst = *--src;
    }
  } else {
    while (len--) {
      *dst++ = *src++;
    }
  }
}

void TreePool::logNodeForIdentifierArray() {
#if TREE_LOG
  printf("\n\n");
  for (int i = 0; i < MaxNumberOfNodes; i++) {
    if (m_nodeForIdentifier[i] != nullptr) {
      printf("Identifier %d, node %p\n", i, m_nodeForIdentifier[i]);
    }
  }
  printf("\n\n");
#endif
}

void TreePool::registerStaticNodeIfRequired(TreeNode * node) {
  if (node->identifier() == -1) {
    int newIdentifier = registerStaticNode(node);
    node->rename(newIdentifier, false);
  }
}

void TreePool::move(TreeNode * destination, TreeNode * source) {
  size_t moveSize = source->deepSize();
  moveNodes(destination, source, moveSize);
}

void TreePool::moveChildren(TreeNode * destination, TreeNode * sourceParent) {
  size_t moveSize = sourceParent->deepSize() - sourceParent->size();
  moveNodes(destination, sourceParent->next(), moveSize);
}

void TreePool::moveNodes(TreeNode * destination, TreeNode * source, size_t moveSize) {
  if (source == destination || moveSize == 0) {
    return;
  }

  char * destinationAddress = reinterpret_cast<char *>(destination);
  char * sourceAddress = reinterpret_cast<char *>(source);
  if (insert(destinationAddress, sourceAddress, moveSize)) {
    // Update the nodeForIdentifier array
    for (int i = 0; i < MaxNumberOfNodes; i++) {
      char * nodeAddress = reinterpret_cast<char *>(m_nodeForIdentifier[i]);
      if (nodeAddress == nullptr) {
        continue;
      } else if (nodeAddress >= sourceAddress && nodeAddress < sourceAddress + moveSize) {
        if (destinationAddress < sourceAddress) {
          m_nodeForIdentifier[i] = reinterpret_cast<TreeNode *>(nodeAddress - (sourceAddress - destinationAddress));
        } else {
          m_nodeForIdentifier[i] = reinterpret_cast<TreeNode *>(nodeAddress + (destinationAddress - (sourceAddress + moveSize)));
        }
      } else if (nodeAddress > sourceAddress && nodeAddress < destinationAddress) {
        m_nodeForIdentifier[i] = reinterpret_cast<TreeNode *>(nodeAddress - moveSize);
      } else if (nodeAddress < sourceAddress && nodeAddress >= destinationAddress) {
        m_nodeForIdentifier[i] = reinterpret_cast<TreeNode *>(nodeAddress + moveSize);
      }
    }
  }
}



#include <stdio.h>

void TreePool::log() {
#if TREE_LOG
  printf("POOL:");
  for (TreeNode * node : *this) {
    printf("|(%03d|%s|%03d|%p)", node->m_identifier, node->description(), node->retainCount(), node);
  }
  printf("|\n");

  //logNodeForIdentifierArray();
#endif
}

void * TreePool::alloc(size_t size) {
  if (m_cursor >= m_buffer + BufferSize || m_cursor + size > m_buffer + BufferSize) {
    return nullptr;
  }
  void * result = m_cursor;
  m_cursor += size;
  return result;
}

void TreePool::dealloc(TreeNode * node) {
  char * ptr = reinterpret_cast<char *>(node);
  assert(ptr >= m_buffer && ptr < m_cursor);
  size_t size = node->size();

  // Step 1 - Compact the pool
  memmove(
    ptr,
    ptr + size,
    m_cursor - (ptr + size)
  );
  m_cursor -= size;

  // Step 2 - Update m_nodeForIdentifier
  for (int i = 0; i < MaxNumberOfNodes; i++) {
    if (m_nodeForIdentifier[i] != nullptr && m_nodeForIdentifier[i] > node) {
      m_nodeForIdentifier[i] = reinterpret_cast<TreeNode *>(reinterpret_cast<char *>(m_nodeForIdentifier[i]) - size);
    }
  }
}

bool TreePool::insert(char * destination, char * source, size_t length) {
  if (source == destination || (destination > source && destination < source + length)) {
    return false;
  }

  assert(length % 4 == 0);
  assert((long)source % 4 == 0);
  assert((long)destination % 4 == 0);

  uint32_t * src = reinterpret_cast<uint32_t *>(source);
  uint32_t * dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = length/4;
  char tempBuffer[BufferSize];
  uint32_t * tmp = reinterpret_cast<uint32_t *>(tempBuffer);
  memmove32(reinterpret_cast<uint32_t *>(tmp), src, len);
  if (dst < src) {
    memmove32(dst + len, dst, src - dst);
    memmove32(dst, tmp, len);
  } else {
    memmove32(src, src + len, dst - (src + len));
    memmove32(dst - len, tmp, len);
  }
  return true;
}

}
