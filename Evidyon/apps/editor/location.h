//------------------------------------------------------------------------------------------------
// File:    location.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __LOCATION_H__
#define __LOCATION_H__

/*
template <typename T> dc::dcObjectWrapper : public dc::dcResource<dcGenericResource,dcObjectWrapper<T>>
{
    public:

    protected:

        T* myReference;
};*/


/**
 * This class references a preexisting raw value (int) somewhere in its parent class and allows
 * that value to be edited as if it were a DirectCraft object.
 */
class dcIntegerWrapper : public dc::dcResource<dcIntegerWrapper>
{
    public:

        typedef int BasicType;

    public:

        dcIntegerWrapper();

        dcIntegerWrapper(int* reference);

        void setValue(int value);

        int& getValue() const;

        bool save(dc::dcStreamOut* stream);
        bool load(dc::dcStreamIn* stream);


    public:

        /**
         * Gets the string representation of this class
         */
        std::string toString() const;


    protected:

        int* myReference;
};


#include <dc/integer>


//
///**
// * Holds the integral location of something in the game; this corresponds to a square division on
// * the map.
// */
//class Location : public dc::dcObject<Location,std::pair<int,int>>
//{
//    public:
//
//        /// The coordinate type as a resource
//      typedef dc::dcInteger CoordinateType;
//
//        /// The coordinate type as a built-in C++ type
//        typedef CoordinateType::BasicType BasicCoordinateType;
//
//    public:
//
//        /**
//         * Initializes the class
//         */
//        Location();
//
//        /**
//         * X-member accessor
//         */
//        inline BasicCoordinateType getX() const { return myValue.first; }
//
//        /**
//         * X-member set
//         */
//        inline void setX(BasicCoordinateType x) { myValue.first = x; }
//
//        /**
//         * Y-member accessor
//         */
//        inline BasicCoordinateType getY() const { return myValue.second; }
//
//        /**
//         * Y-member set
//         */
//        inline void setY(BasicCoordinateType y) { myValue.second = y; }
//
//
//    public:
//
//        bool save(dc::dcStreamOut* stream);
//        bool load(dc::dcStreamIn* stream);
//
//        /**
//         * Gets the string representation of this class
//         */
//        std::string toString() const;
//
//
//    protected:
//
//        /**
//         * Method implemented by the derived class to compare the class
//         * to another resource that is guaranteed to be valid and of
//         * the same type.
//         * @see compareTo
//         */
//        int compareValue(const BasicType& otherValue) const;
//
//
//    public:
//
//        /**
//         * The name of this class type
//         */
//        static std::string staticTypeName();
//
//
//    protected:
//
//        dc::dcIntegerWrapper myX;
//        dc::dcIntegerWrapper myY;
//};



/**
 * Holds the integral location of something in the game; this corresponds to a square division on
 * the map.
 */
class Location : public dc::dcResource<Location>
{
    public:

        /// The coordinate type as a resource
      typedef dc::dcInteger CoordinateType;

        /// The coordinate type as a built-in C++ type
        typedef CoordinateType::BasicType BasicCoordinateType;

        typedef std::pair<BasicCoordinateType, BasicCoordinateType> BasicType;

    public:

        /**
         * Initializes the class
         */
        Location();

        inline BasicType getValue() const {
          return BasicType(getX(), getY());
        }

        inline void setValue(const BasicType& value) {
          setX(value.first);
          setY(value.second);
        }

        /**
         * X-member accessor
         */
        inline BasicCoordinateType getX() const { return myX.getValue(); }

        /**
         * X-member set
         */
        inline void setX(BasicCoordinateType x) { myX.setValue(x); }

        /**
         * Y-member accessor
         */
        inline BasicCoordinateType getY() const { return myY.getValue(); }

        /**
         * Y-member set
         */
        inline void setY(BasicCoordinateType y) { myY.setValue(y); }


    public:


        /**
         * Gets the string representation of this class
         */
        std::string toString() const;

      virtual int compareTo( const dc::dcGenericResource* other ) const;


    protected:


    public:

        /**
         * The name of this class type
         */
        static std::string staticTypeName();


    protected:

      dc::dcInteger myX;
      dc::dcInteger myY;
};


#endif