//------------------------------------------------------------------------------------------------
// File:    dcinteger.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCINTEGER_H__
#define __DCINTEGER_H__

namespace dc {

    /**
     * Stores a signed 32-bit integer
     */
    class dcInteger : public dc::dcObject<dcInteger,int>
    {
        public:

            /**
             * Initializes this resource
             */
            dcInteger();

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