//------------------------------------------------------------------------------------------------
// File:    dcrange.h
//
// Desc:    TODO: this class kinda sucks...its naming is WAY too long and complicated.  Very
//          consistant, but too hard to use
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCRANGE_H__
#define __DCRANGE_H__

namespace dc {


    /**
     * An alias for an std::pair containing two entries (coordinates) of the same type that
     * specifies a place in 2D.
     */
    template <typename T> struct BasicLocation : public std::pair<T,T> {};


    /**
     *
     * T - An object type (a resource that is a derivative of dcObject<>)
     */
    template <typename T> class dcRange : public dcResource<dcRange<T>>
    {
        public:

            /// What kind of object the coordinates are stored as
            typedef T CoordinateType;
        
            /// What kind of coordinates this range stores
            typedef typename T::BasicType BasicCoordinateType;

            /// A non-resource location that can be checked against this range
            typedef BasicLocation<BasicCoordinateType> BasicLocationType;
            
        public:


            /**
             * Initializes this resource
                 */
            dcRange()
            {
                member( "Left", &myLeft );
                member( "Top", &myTop );
                member( "Right", &myRight );
                member( "Bottom", &myBottom );
                // Reset this range.  Hopefully, this is unnecesary.
                //set( 0, 0, 0, 0 );
            }

            /**
             * Determines whether or not this range includes the given location
             */
            bool contains( const BasicLocationType& location )
            {
                // Make sure that the range is valid
                ensureRealRange();

                // Check to see whether or not this location is inside the range
                return (myLeft.getValue() <= location.first)  &&
                       (myRight.getValue() >= location.first) &&
                       (myTop.getValue() <= location.second)  &&
                       (myBottom.getValue() >= location.second);
            }

            /**
             * Obtains the location of the center of this range using the given type.  This is
             * templated so that floating-point centers can be obtained from integral ranges.
             * @param center The destination variable for the center of this range
             * @return The parameter passed as "center"
             */
            template <typename C> BasicLocation<C>* getCenter( BasicLocation<C>* center ) const
            {
                // Make sure the parameter is valid
                if( !center ) return 0;
            
                // Average the range
                center->first  = (C)((myLeft.getValue() + myRight.getValue()) / ((C)2));
                center->second = (C)((myTop.getValue() + myBottom.getValue()) / ((C)2));
                
                // Return the result
                return center;
            }

            /**
             * Gets a reference to the top coordinate object of this range
             */
            inline CoordinateType* top() const { return &myTop; }

            /**
             * Gets a reference to the left coordinate object of this range
             */
            inline CoordinateType* left() const { return &myLeft; }

            /**
             * Gets a reference to the bottom coordinate object of this range
             */
            inline CoordinateType* bottom() const { return &myBottom; }

            /**
             * Gets a reference to the right coordinate object of this range
             */
            inline CoordinateType* right() const { return &myRight; }

            /**
             * Gets the top coordinate of this range
             */
            inline BasicCoordinateType topCoordinate() const { return myTop.getValue(); }

            /**
             * Gets the left coordinate of this range
             */
            inline BasicCoordinateType leftCoordinate() const { return myLeft.getValue(); }

            /**
             * Gets the bottom coordinate of this range
             */
            inline BasicCoordinateType bottomCoordinate() const { return myBottom.getValue(); }

            /**
             * Gets the right coordinate of this range
             */
            inline BasicCoordinateType rightCoordinate() const { return myRight.getValue(); }

            /**
             * Changes this range's values
             */
            void set( BasicCoordinateType left, BasicCoordinateType top,
                      BasicCoordinateType right, BasicCoordinateType bottom )
            {
                myLeft.setValue( left );
                myTop.setValue( top );
                myRight.setValue( right );
                myBottom.setValue( bottom );
                ensureRealRange();
            }

            /**
             * Changes this range's values
             */
            void set( const BasicLocationType& topLeft, const BasicLocationType& bottomRight )
            {
                myLeft.setValue( topLeft.first );
                myTop.setValue( topLeft.second );
                myRight.setValue( bottomRight.first );
                myBottom.setValue( bottomRight.second );
                ensureRealRange();
            }


        protected:

            /**
             * Casts a generic resource interface into this resource's type.
             * This method will throw an exception if the variable is of a
             * different type.
             */
            //dcRange<T>* typecast( dcGenericResource* variable ) const
            //{
            //    // Null references can be all types
            //    if( variable == 0 ) return 0;

            //    // Make sure the types match
            //    if( getTypeID() != variable->getTypeID() )
            //        throw "invalid";

            //    // Re-cast the variable and return it
            //    return (dcRange<T>*)variable;
            //}

            /**
             * Makes sure that this range's left-right and top-bottom coordinates are in the
             * correct order.
             */
            void ensureRealRange()
            {
                // Correct the left-right coordinates
                if( myLeft.getValue() > myRight.getValue() )
                {
                    BasicCoordinateType swap = myLeft.getValue();
                    myLeft.setValue( myRight.getValue() );
                    myRight.setValue( swap );
                }

                // Correct the top-bottom coordinates
                if( myTop.getValue() > myBottom.getValue() )
                {
                    BasicCoordinateType swap = myTop.getValue();
                    myTop.setValue( myBottom.getValue() );
                    myBottom.setValue( swap );
                }
            }


        public:

            /**
             * The name of this class type
             * @return dcClass name string
             */
            static std::string staticTypeName() { return std::string("Range of ") + T::staticTypeName(); }


        protected:

            /// The range of this coordinate
            CoordinateType myLeft, myTop, myRight, myBottom;

    };


}

#endif