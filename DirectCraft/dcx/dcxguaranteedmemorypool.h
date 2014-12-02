//------------------------------------------------------------------------------------------------
// File:    dcxguaranteedmemorypool.h
//
// Desc:    A guaranteed memory pool will always provide a returned element, even if doing so
//          requires allocating from a non-contiguous block of memory.  This prevents pools of
//          this type from having iterators.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXGUARANTEEDMEMORYPOOL_H__
#define __DCXGUARANTEEDMEMORYPOOL_H__


namespace dcx {

/**
 * This class is used to allocate a block of memory of objects that are frequently allocated
 * and freed.
 */
template <typename O, const size_t PoolSize> class dcxGuaranteedMemoryPool {
  typedef dcx::dcxMemoryPool<O, PoolSize> CoreMemoryPool;

public:

  /**
   * Allocates memory for this memory pool
   */
  bool create() {
    return pool_.create();
  }

  /**
   * Frees the memory used for this class
   */
  void destroy() {
    pool_.destroy();
  }

  /**
   * Returns true if there are no outstanding allocations
   */
  bool empty() const {
    return pool_.empty();
  }

  /**
   * Gets a new memory segment from the pool
   */
  O* acquire() {
    O* object = pool_.acquire();
    if (!object) object = new O();
    return object;
  }

  /**
   * Gets rid of the given element from the memory pool
   */
  bool release( O* element ) {
    if (pool_.release(element)) return true;
    delete element;
    return true;
  }

  size_t numberOfAllocatedEntries() {
    return pool_.numberOfAllocatedEntries();
  }

  size_t numberOfFreeEntries() {
    return pool_.numberOfFreeEntries();
  }


protected:

  CoreMemoryPool pool_;
};


}



#endif