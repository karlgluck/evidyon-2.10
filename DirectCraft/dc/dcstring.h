//------------------------------------------------------------------------------------------------
// File:    dcstring.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCSTRING_H__
#define __DCSTRING_H__

namespace dc {

    /**
     * Stores a string of characters
     */
    class dcString : public dc::dcObject<dcString,std::string>
    {
        public:

            /**
             * Initializes this resource
             */
            dcString();

        public:

            /**
             * Method implemented by the derived class to save this object
             * to the destination stream.  The default implementation simply
             * saves the reference
             */
            bool save( dcStreamOut* stream );

            /**
             * Method implemented by the derived class to load this object
             * from the stream
             */
            bool load( dcStreamIn* stream );

            /**
             * Gets a string representation of this class type
             * @return String value indicating what's in this class
             */
            std::string toString() const;

            // Takes a string and sets the value of this object
            virtual void interpret(const char* string);


        protected:

            /**
             * Method implemented by the derived class to compare the class
             * to another resource that is guaranteed to be valid and of
             * the same type.
             * @see compareTo
             */
            int compareValue( const std::string& otherValue ) const;


        public:

            /**
             * Obtains the name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName();
    };
};


#endif