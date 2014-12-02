//------------------------------------------------------------------------------------------------
// File:    dcpair.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __PAIR_H__
#define __PAIR_H__


namespace dc {

    /**
     * Associates two values with each other
     */
    template <typename A, typename B> class dcPair : public dcResource<dcPair<A,B>>
    {
        public:

            /// Key resource type
            typedef A First;

            /// Value resource type
            typedef B Second;

            /// The basic resource type

        public:

            /**
             * Initializes this resource
                 */
            dcPair()
            {
                member( A::staticTypeName(), &myFirst );
                member( B::staticTypeName(), &mySecond );
            }

            /**
             * 
             */
            First* getFirst()
            {
                return &myFirst;
            }

            /**
             * 
             */
            Second* getSecond()
            {
                return &mySecond;
            }

            /** 
             * Compares this pair to another resource
             */
            int compareTo( const dcGenericResource* other ) const
            {
                // If the types don't match, use the default type comparator
                if( !other || getTypeID() != other->getTypeID() )
                    return dcGenericResource::compareTo( other );

                // Cast the parameter
                dcPair<A,B>* pair = (dcPair<A,B>*)other;

                // Check the first elements
                int firstCompare = myFirst.compareTo( &pair->myFirst );
                if( firstCompare != 0 ) return firstCompare;

                // The first elements were equal, so compare the second ones
                return mySecond.compareTo( &pair->mySecond );
            }


        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName()
            {
                return std::string("Pair of ") + A::staticTypeName() + std::string(" to ") + B::staticTypeName();
            }


        protected:

            /// The first value
            First myFirst;

            /// The second value
            Second mySecond;
    };
}


#endif