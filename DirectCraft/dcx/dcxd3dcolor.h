//------------------------------------------------------------------------------------------------
// File:    dcxd3dcolor.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXD3DCOLOR_H__
#define __DCXD3DCOLOR_H__

#include <dc/byte>
#include <d3dx9.h>

namespace dcx {

    // Use the DirectCraft basic namespace
    using namespace dc;

    /**
     * Stores a set of three variables of the templated type
     */
    class dcxD3DColor : public dcResource<dcxD3DColor>
    {
        public:

            /**
             * Initializes this resource
             */
            dcxD3DColor()
            {
                member( "R", &myR );
                member( "G", &myG );
                member( "B", &myB );
            }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            dcByte* getR() { return &myR; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            dcByte* getG() { return &myG; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            dcByte* getB() { return &myB; }

            /**
             * Obtains the X-member of this vector
             * @return Reference to the X-member
             */
            const dcByte* getR() const { return &myR; }

            /**
             * Obtains the Y-member of this vector
             * @return Reference to the Y-member
             */
            const dcByte* getG() const { return &myG; }

            /**
             * Obtains the Z-member of this vector
             * @return Reference to the Z-member
             */
            const dcByte* getB() const { return &myB; }

            D3DCOLOR getD3DColor() const {
              return D3DCOLOR_XRGB(myR.getValue(), myG.getValue(), myB.getValue());
            }

            void setD3DColor(D3DCOLOR color) {
              myR.setValue((unsigned char)(0xFF&(color>>16)));
              myG.setValue((unsigned char)(0xFF&(color>> 8)));
              myB.setValue((unsigned char)(0xFF&(color>> 0)));
            }

            COLORREF getRGBColor() const {
              return RGB(myR.getValue(), myG.getValue(), myB.getValue());
            }

            void setRGBColor(COLORREF color) {
              myR.setValue(GetRValue(color));
              myG.setValue(GetGValue(color));
              myB.setValue(GetBValue(color));
            }


        public:

            /**
             * Returns the name of this class type
             */
            static std::string staticTypeName()
            {
                return "D3DColor";
            }


        protected:

            /// Colors
            dcByte myR, myG, myB;
    };

}



#endif
