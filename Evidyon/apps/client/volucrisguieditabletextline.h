//------------------------------------------------------------------------------------------------
// File:    volucrisguieditabletextline.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIEDITABLETEXTLINE_H__
#define __VOLUCRISGUIEDITABLETEXTLINE_H__



#include "../common/gui/guieditabletextline.h"

/**
 * Implements a Volucris-specific editable text line
 */
class VolucrisGUIEditableTextLine : public GUIEditableTextLine
{
    public:

        /**
         * Initializes this class
         */
        VolucrisGUIEditableTextLine();

        /**
         * Calls the methods to set many text line parameters all at once
         */
        void configure(GUIFont* font,
                        bool isPassword, bool restrictToNumbers, D3DCOLOR textColor);

        /**
         * Calls the methods to set many text line parameters all at once
         */
        void configure(GUIFont* font,
                        bool isPassword, bool restrictToNumbers, D3DCOLOR textColor, int textLengthLimit);

        /**
         * Sets the number of characters this line can accept
         */
        void setTextLengthLimit(int characters);

        /**
         * Makes this text line accept any number of characters (default behavior)
         */
        void removeTextLengthLimit();

        /**
         * Changes the flag that restricts input so that only numbers are accepted
         */
        void numbersOnly(bool restrictToNumbers);

        /**
         * Registers this component's visible elements with the given layer.  This method calls
         * the superclass method.
         */
        void registerElements(GUILayer* layer);

        /**
         * Unregisters this component's visible elements.  This method calls the superclass method.
         */
        void unregisterElements(GUILayer* layer);

        void setBackgroundColor(D3DCOLOR color);


    protected:

        /**
         * Invoked when the user enters a valid character that should be added to the internal buffer
         */
        virtual void onTypedPrintableCharacter(char printableCharacter);


    protected:

        /// Whether or not this line only accepts numbers
        bool myNumbersOnlyFlag;

        /// The maximum number of characters (excluding the null-terminator) this text line accepts
        size_t myTextLengthLimit;

        /// The background for this text element.  Uses the superclass's canvas as the source.
        GUIBorderedFillElement myBackground;

        /// The area where this text element is drawn.  This is used to override the source canvas for
        /// the text in the superclass GUIEditableTextLine.  That canvas will instead be the background
        /// canvas for the whole element.
        GUICanvas myTextCanvas;
};


#endif