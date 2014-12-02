#ifndef __ACTORPOINTER_H__
#define __ACTORPOINTER_H__

#include <dcx/managedmemorypool>

namespace Evidyon {

class Actor;
typedef dcx::dcxManagedMemoryPoolPointer<Actor> ActorPointer;

}

#endif