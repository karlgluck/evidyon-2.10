//------------------------------------------------------------------------------------------------
// File:    dcxvector3.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXVECTOR3_H__
#define __DCXVECTOR3_H__



namespace dcx {

    // Use the DirectCraft basic namespace
    using namespace dc;

    /**
     * Stores a set of three variables of the templated type
     */
    template <typename T> class dcxVector3 : public dcResource<dcxVector3<T>>
    {
        public:

            /**
             * Initializes this resource
             */
            dcxVector3()
            {
                member( "x", &myX );
                member( "y", &myY );
                member( "z", &myZ );
            }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            T* getX() { return &myX; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            T* getY() { return &myY; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            T* getZ() { return &myZ; }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            const T* getX() const { return &myX; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            const T* getY() const { return &myY; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            const T* getZ() const { return &myZ; }

            /**
             * Obtains the X, Y, or Z member of the vector by its index (0, 1 or 2)
             * @return The specified member, or null (0) if the index is out of range.
             */
            T* getByIndex( size_t index )
            {
                switch( index )
                {
                    case 0:  return &myX;
                    case 1:  return &myY;
                    case 2:  return &myZ;
                    default: return 0;
                }
            }

        public:

            /**
             * Returns the name of this class type
             */
            static std::string staticTypeName()
            {
                return "Vector3<" + T::staticTypeName() + ">";
            }


        protected:

            /// First component of the vector
            T myX;

            /// Second component of the vector
            T myY;

            /// Third component of the vector
            T myZ;
    };

}



#endif

//------------------------------------------------------------------------------------------------
// File:    .h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef ___H__
#define ___H__

#endif