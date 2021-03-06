//------------------------------------------------------------------------------------------------
// File:    dcenum.h
//
// Desc:    Represents an enumeration
//
// Copyright (c) 2007 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCENUM_H__
#define __DCENUM_H__




namespace dc {

    /**
     * This class is used to represent enumeration data.  This is preferable to simply using an
     * integer class because it provides a layer of abstraction, associating the string value of
     * the class with an actual name instead of just a number (even though the data stored is,
     * in reality, just a number).
     * 
     *   E - The name of a C++ "enum" declaration around which to construct this resource type
     */
    template <typename E> class dcEnum : public dcObject<dcEnum<E>,E>
    {
        public:

            /// The C++ enum to model
            typedef E BasicEnumType;


        public:

            /**
             * Gets a string representation of this class type
             * @return String value indicating what's in this class
             */
            std::string toString() const
            {
                const char* stringValue = getString( myValue );
                return (stringValue == 0) ? "???" : stringValue;
            }

            /**
             * Saves this resourcekwg88ca
             * @param stream The stream into which to save
             * @return Whether or not the operation succeeded
             */
            virtual bool save( dcStreamOut* stream )
            {
                //return stream->write( &myValue, sizeof(myValue) );
                const char* string = getString(myValue);
                return stream->writeString((string == 0) ? "" : string);
            }

            /**
             * Loads this resource
             * @param stream The stream from which to load
             * @return Whether or not the operation succeeded
             */
            virtual bool load( dcStreamIn* stream )
            {
                //return stream->read( &myValue, sizeof(myValue) );
                std::string value;
                if (!stream->readString(&value)) return false;
                myValue = getEnumValueFromString(value);
                return true;
            }

            // Takes a string and sets the value of this object
            virtual void interpret(const char* string) {
              myValue = getEnumValueFromString(string);
            }

        public:

            /**
             * This method is not implemented by DirectCraft.  The user must resolve the method
             * each time a new type of enum is generated.
             * Gets the string representation of a value in the enum.  If the value is illegal
             * for this enum, return null.
             */
            static const char* getString( BasicEnumType value );

            /**
             * Returns the value of BasicEnumType 'e' for which
             * e == getEnumValueFromString(getString(e))
             */
            static BasicEnumType getEnumValueFromString(const std::string& string_to_match) {
              const char* value_str;
              BasicEnumType e = BasicEnumType(0);
              for (e = BasicEnumType(e); (value_str = getString(e)) != 0; e = BasicEnumType(int(e) + 1)) {
                if (string_to_match.compare(value_str) == 0) return e;
              }
              return e; // this is guaranteed to be invalid since getString(e) == NULL
            }

            /**
             * This method is not implemented by DirectCraft.  The user must resolve the method
             * each time a new type of enum is generated.
             * Obtains the name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName();
    };

}




#endif