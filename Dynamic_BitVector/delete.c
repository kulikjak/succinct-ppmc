#include "dbv.h"
#include "memory.h"

void DBV_Delete(DBVStructRef DBV__, uint32_t pos__) {
  uint32_t temp, mask, vector;
  int32_t val;

  if (pos__ >= DBV_MEMORY_GET_ANY(DBV__->mem_, DBV__->root_)->p_)
    FATAL("Index out of range");

  DBV_STACK_CLEAN();

  /* traverse the tree and enter correct leaf */
  DBVMemObj mem = DBV__->mem_;
  DBVMemPtr current = DBV__->root_;

  DBVNodeRef node_ref;
  DBVLeafRef leaf_ref;

  while (!DBV_IS_LEAF(current)) {
    DBV_STACK_PUSH(current);

    /* update p counter as we are traversing the structure */
    node_ref = DBV_MEMORY_GET_NODE(mem, current);
    node_ref->p_ -= 1;

    temp = DBV_MEMORY_GET_ANY(mem, node_ref->left_)->p_;

    if (temp > pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= temp;
      current = node_ref->right_;
    }
  }

  /* delete bit inside the node */
  leaf_ref = DBV_MEMORY_GET_LEAF(mem, current);
  assert(leaf_ref->p_ > 0);

  mask = ((pos__) == 0) ? 0 : (uint32_t)(~(0)) << (32 - (pos__));
  vector = 0;

  /* contains any nonzero value for set bit (not exactly 1) */
  val = leaf_ref->vector_ & (0x1 << (31 - pos__));

  vector |= leaf_ref->vector_ & mask;
  vector |= (leaf_ref->vector_ & (~(mask) >> 1)) << 1;

  leaf_ref->vector_ = vector;
  leaf_ref->p_ -= 1;

  /* handle r counters all the way to root in case that deleted value was 1 */
  if (val) {
    do {
      node_ref = DBV_MEMORY_GET_ANY(mem, current);
      node_ref->r_ -= 1;

      current = DBV_STACK_POP();
    } while (current != -1);
  }
}
