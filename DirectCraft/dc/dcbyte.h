//------------------------------------------------------------------------------------------------
// File:    dcbyte.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCBYTE_H__
#define __DCBYTE_H__


namespace dc {

    /**
     * Stores an 8-bit unsigned integer value
     */
    class dcByte : public dc::dcObject<dcByte,unsigned char>
    {
        public:

            /**
             * Initializes this resource
             */
            dcByte();

            /**
             * Gets a string representation of this class type
             * @return String value indicating what's in this class
             */
            std::string toString() const;

            // Takes a string and sets the value of this object
            virtual void interpret(const char* string);


        public:

            /**
             * Obtains the name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName();
    };
};


#endif