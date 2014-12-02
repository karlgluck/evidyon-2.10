//------------------------------------------------------------------------------------------------
// File:    dcobject.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCOBJECT_H__
#define __DCOBJECT_H__



namespace dc {

    /** 
     * Specific (non-templated, one basic-type variable) resource types derive from this class.
     *  D - the derived type
     *  S - the simple data type this object stores (int, bool, std::string, ...).  The derived
     *      class should implement save() and load() if the raw value of the data type is not
     *      what should be saved to the disk (as in std::string).  The simple data type must
     *      implement operator =, operator ==, operator <, operator >, and operator !=
     *  P - the parent class for this type
     */
    template <typename D, typename S, typename P = dcGenericResource> class dcObject : public dcResource<D,P>
    {
        public:

            // Allow the input parameters to be referenced
            typedef D ResourceType;
            typedef S BasicType;

        public:


            /**
             * Initializes this resource
             */
            dcObject()
            {
            }

            /**
             * Obtains the value stored in the object
             * @return The current value saved in this resource
             */
            const BasicType& getValue() const { return myValue; }

            /**
             * Sets the value saved in this resource
             * @value The new value to save in this object
             */
            void setValue( BasicType value ) { myValue = value; }

            /**
             * Saves this resource
             * @param stream The stream into which to save
             * @return Whether or not the operation succeeded
             */
            virtual bool save( dcStreamOut* stream )
            {
                return stream->write( &myValue, sizeof(myValue) );
            }

            /**
             * Loads this resource
             * @param stream The stream from which to load
             * @return Whether or not the operation succeeded
             */
            virtual bool load( dcStreamIn* stream )
            {
                return stream->read( &myValue, sizeof(myValue) );
            }


            /**
             * Compares this resource with another resource
             */
            int compareTo( const dcGenericResource* other ) const
            {
                if( other == 0 || getTypeID() != other->getTypeID() )
                    return dcGenericResource::compareTo( other );
                else
                    return compareValue( ((ResourceType*)(other))->myValue );
            }


            //----[  serialize  ]----------------------------------------------------------
            void serialize(std::string* string) {
              string->assign(toString());
            }


        protected:

            /**
             * Method implemented by the derived class to compare the class
             * to another resource that is guaranteed to be valid and of
             * the same type.
             * @see compareTo
             */
            virtual int compareValue( const S& otherValue ) const
            {
                if( myValue < otherValue ) return -1;
                if( myValue > otherValue ) return +1;
                else return 0;
            }


        protected:

            /**
             * Casts a generic resource interface into this resource's type.
             * This method will throw an exception if the variable is of a
             * different type.
             */
            ResourceType* typecast( dcGenericResource* variable ) const
            {
                // Null references can be all types
                if( variable == 0 ) return 0;

                // Make sure the types match
                if( getTypeID() != variable->getTypeID() )
                    throw "invalid";

                // Re-cast the variable and return it
                return (ResourceType*)variable;
            }


        protected:

            /// The value saved in this object
            BasicType myValue;
    };

}


#endif