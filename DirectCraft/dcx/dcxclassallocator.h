//------------------------------------------------------------------------------------------------
// File:    dcxclassallocator.h
//
// Desc:    Provides a simple class which, when instantiated, allocates a new instance of another
//          class and, when destructed, frees the class it created.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXCLASSALLOCATOR_H__
#define __DCXCLASSALLOCATOR_H__

namespace dcx {

    /**
     * Allocates a class when constructed, deallocates it when destructed.  This common pattern
     * is used in functions that get global objects.  The object of type T can be defined as
     * a static entity using the dcxClassAllocator, and it is guaranteed to be freed.  This uses
     * a much smaller executable memory footprint then would a solid static class.
     *
     * Template parameters:
     *      T - class type to allocate
     *
     * Example:
     *
     *      class Foo { ... };
     *
     *      Foo* getGlobalFoo()
     *      {
     *        static dcxClassAllocator<Foo> allocator;
     *        return allocator.getClass();
     *      }
     *
     */
    template <typename T> class dcxClassAllocator
    {
        public:

            /**
             * Initializes the class allocator by creating a new instance of the templated type
             */
            dcxClassAllocator()
            {
                myClass = new T();
            }

            /**
             * Frees the memory that is managed by this class
             */
            ~dcxClassAllocator()
            {
                delete myClass;
            }

            /**
             * Obtains the class this allocator handles
             */
            T* getClass()
            {
                return myClass;
            }

            /**
             * Calls a method of the allocated class
             */
            T& operator -> ()
            {
                return *myClass;
            }

        protected:

            /// The memory for the class this allocator manages
            T* myClass;
    };

}



#endif