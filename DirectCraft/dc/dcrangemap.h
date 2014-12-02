//------------------------------------------------------------------------------------------------
// File:    dcrangemap.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCRANGEMAP_H__
#define __DCRANGEMAP_H__

namespace dc {

    /**
     * Maps a coordinate range to a certain resource type.  This provides the ability to map a 2d
     * region to a resource in a map, such that searching for any Location within that region will
     * return a reference to the mapped resource.  Additionally, resource regions can overlap and
     * be duplicated.
     * C - The dcObject to use as coordinates in the map
     * T - The dcGenericResource type to which a dcRange of coordinates will correspond
     */
    template <typename C, typename T> class dcRangeMap : public dcCollection<dcRangeMap<C,T>>
    {
        public:

            /// This is the range resource type used in this map to index resources
            typedef typename dcRange<C> RangeType;

            /// The resource type to which the coordinates map
            typedef T ValueType;

            /**
             * Associates two values with each other
             */
            class Element : public dcResource<Element>
            {
                public:


                    /**
                     * Initializes this resource
                     */
                    Element()
                    {
                        member( "Range", &myRange );
                        member( "Value", &myValue );
                    }

                    /**
                     * TODO: this should be getRange...
                     */
                    RangeType* getKey()
                    {
                        return &myRange;
                    }

                    /**
                     * 
                     */
                    ValueType* getValue()
                    {
                        return &myValue;
                    }

                    /**
                     * Compares this element with another resource.  Only keys are compared.
                     */
                    int compareTo( const dcGenericResource* other ) const
                    {
                        if( other )
                        {
                            if( getTypeID() == other->getTypeID() )
                                return myRange.compareTo( &((Element*)other)->myRange );
                            else if( myRange.getTypeID() == other->getTypeID() )
                                return myRange.compareTo( other );
                        }

                        // Use the generic implementation
                        return dcGenericResource::compareTo( other );
                    }


                public:

                    /**
                     * The name of this class type
                     * @return dcClass name string
                     */
                    static std::string staticTypeName()
                    {
                        return std::string("Element of") + dcRangeMap<C,T>::staticTypeName();
                    }

                protected:

                    /// The key in this pair
                    RangeType myRange;

                    /// The value to which the key maps
                    ValueType myValue;
            };

            /// A pair of coordinates that can be looked up in the range map to find resources
            /// whose ranges overlap those coordinates.
            typedef typename RangeType::BasicLocationType BasicLocationType;

            /// A set of coordinates
            typedef std::set<BasicLocationType> BasicLocationSetType;

            /// The type of the list that is returned when searching for locations
            typedef std::list<Element*> RangeResourceListType;



        public:

            /**
             * Initializes this resource
             */
            dcRangeMap()
            {
            }


            /**
             * Finds all the elements 'e' of type BasicLocationType for which the expression
             * e->getFirst()->contains( any of the locations ) is true, and appends 'e' to the
             * output list.  The list is not cleared before this method is evaluated.
             * @param locations The set of coordinates to search
             * @param list The output list for the intersecting elements
             * @return How many elements were added to the list
             */
            size_t search( const BasicLocationSetType* locations, RangeResourceListType* list )
            {
                // How many entries this list contains
                size_t listStartSize = list->size();
                
                // Go through all of the elements in this collection
                for( Array::iterator element = myMembers.begin(); element != myMembers.end(); ++element )
                {
                    // Get the range-resource type from the list
                    Element* rangeResourcePair = (Element*)*element;

                    // The range of the current element
                    RangeType* range = rangeResourcePair->getKey();

                    // Compare each element to the list of locations
                    for( BasicLocationSetType::const_iterator location  = locations->begin();
                                                              location != locations->end(); ++location )
                    {
                        // Check to see if the element contains the 
                        if( range->contains( *location ) )
                        {
                            // Add this element to the output list
                            list->push_back( rangeResourcePair );
                            
                            // Move on to the next element
                            break;
                        }
                    }
                }

                // Pass back how many elements we found
                return list->size() - listStartSize;
            }

            /**
             * Determines whether or not this collection allows duplicate entries to
             * be held internally.
             * @return Flag value
             */
            bool allowDuplicates() const
            {
                return false;
            }

            /**
             * Determines whether or not the derived collection type requires that its
             * entries be kept sorted.
             */
            bool keepSorted() const
            {
                return true;
            }

        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName() { return std::string("Range Map of ") + C::staticTypeName() + std::string(" coordinates to ") + T::staticTypeName(); }
    };

}

#endif
