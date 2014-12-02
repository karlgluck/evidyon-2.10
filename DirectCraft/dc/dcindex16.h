//------------------------------------------------------------------------------------------------
// File:    dcindex16.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCINDEX16_H__
#define __DCINDEX16_H__

namespace dc {

    /**
     * Stores a 16-bit unsigned integer
     */
    class dcIndex16 : public dc::dcObject<dcIndex16,unsigned short>
    {
        public:

            /**
             * Initializes this resource
             */
            dcIndex16();

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