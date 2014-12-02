//------------------------------------------------------------------------------------------------
// File:    dckeyvaluepair.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __KEYVALUEPAIR_H__
#define __KEYVALUEPAIR_H__


// todo: add comments

namespace dc {


    /**
     * Associates two values with each other
     */
    template <typename K, typename V> class dcKeyValuePair : public dcResource<dcGenericResource,dcKeyValuePair<K,V>>
    {
        public:

            /// Key resource type
            typedef K Key;

            /// Value resource type
            typedef V Value;

        public:


            /**
             * Initializes this resource
                 */
            dcKeyValuePair()
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
             * Compares this pair with the given key
             */
            int compareKey( const Key* key ) const
            {
                return myKey.compareTo( key );
            }


        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName()
            {
                return std::string("Key-Value Pair of ") + Key::staticTypeName() + std::string(" to ") + Value::staticTypeName();
            }

        protected:

            /// The key in this pair
            Key myKey;

            /// The value to which the key maps
            Value myValue;
    };

}


#endif