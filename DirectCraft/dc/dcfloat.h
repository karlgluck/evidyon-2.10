//------------------------------------------------------------------------------------------------
// File:    dcfloat.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCFLOAT_H__
#define __DCFLOAT_H__

namespace dc {

    /**
     * Stores a floating-point value
     */
    class dcFloat : public dc::dcObject<dcFloat,float>
    {
        public:

            /**
             * Initializes this resource
             */
            dcFloat();

            /**
             * Gets a string representation of this class type
             * @return String value indicating what's in this class
             */
            std::string toString() const;

            // Takes a string and sets the value of this object
            virtual void serialize(std::string* string);
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