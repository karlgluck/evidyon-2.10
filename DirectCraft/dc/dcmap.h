//------------------------------------------------------------------------------------------------
// File:    dcmap.h
//
// Desc:    Stores a map of key-to-value resources
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCMAP_H__
#define __DCMAP_H__


namespace dc {

    /**
     * K - an dcObject to use as a key
     * V - any dcGenericResource to which unique keys will be associated
     */
    template <typename K, typename V> class dcMap : public dcCollection<dcMap<K,V>>
    {
        public:

            /// The kind of resource that can be searched
            typedef K Key;

            /// The kind of resource to which keys map
            typedef V Value;

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
                        member( Key::staticTypeName(), &myKey );
                        member( Value::staticTypeName(), &myValue );
                    }

                    /**
                     * 
                     */
                    Key* getKey()
                    {
                        return &myKey;
                    }

                    /**
                     * 
                     */
                    Value* getValue()
                    {
                        return &myValue;
                    }

                    /**
                     * 
                     */
                    const Key* getKey() const
                    {
                        return &myKey;
                    }

                    /**
                     * 
                     */
                    const Value* getValue() const
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
                            // compare directly if both are keys in the same map
                            // or this is a key and its raw type.  otherwise, do
                            // a full element comparison (otherwise compareTo will
                            // indicate that two maps with the same keys but different
                            // mapped values are equivalent!)
                            if( getTypeID() == other->getTypeID() ) {
                              if (getOwner() == other->getOwner())
                                return myKey.compareTo( &((Element*)other)->myKey );
                            }
                            else if( myKey.getTypeID() == other->getTypeID() )
                                return myKey.compareTo( other );
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
                        return std::string("Element of ") + dcMap<K,V>::staticTypeName();
                    }

                protected:

                    /// The key in this pair
                    Key myKey;

                    /// The value to which the key maps
                    Value myValue;
            };


        public:

            /**
             * Initializes this resource
             */
            dcMap()
            {
            }


            /**
             * Searches for the given key in this map as a non-resource type.  This means
             * that only Objects can act as keys.
             * @param key The key to search for
             * @return The elemnt that matches to this key, or null
             */
            Element* find( const typename Key::BasicType key )
            {
                // Initialize a resource-key
                Key resourceKey;
                resourceKey.setValue( key );

                // Make sure that the map is sorted
                //sort();

                // Look for the key in the map
                //return (Element*)binarySearch( (dcGenericResource*)&resourceKey );
                return (Element*)dcGenericCollection::find( (dcGenericResource*)&resourceKey, 0, NULL );
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
            static std::string staticTypeName() { return std::string("Map of ") + K::staticTypeName() + std::string(" to ") + V::staticTypeName(); }
    };
}



#endif