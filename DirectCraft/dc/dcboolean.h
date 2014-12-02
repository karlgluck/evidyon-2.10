//------------------------------------------------------------------------------------------------
// File:    dcboolean.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCBOOLEAN_H__
#define __DCBOOLEAN_H__

namespace dc {

    /**
     * Stores a boolean flag
     */
    class dcBoolean : public dc::dcObject<dcBoolean,bool>
    {
        public:

            /**
             * Initializes this resource
             */
            dcBoolean();

            /**
             * Swaps the value of this flag and returns the new value
             */
            bool toggle();

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

}


#endif