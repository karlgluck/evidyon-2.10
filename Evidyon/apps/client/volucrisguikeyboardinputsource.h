//------------------------------------------------------------------------------------------------
// File:    volucrisguikeyboardinputsource.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __VOLUCRISGUIKEYBOARDINPUTSOURCE_H__
#define __VOLUCRISGUIKEYBOARDINPUTSOURCE_H__


/**
 * Gets Windows keyboard input and passes it into the GUI
 */
class VolucrisGUIKeyboardInputSource : public GUIKeyboardInputSource
{
    public:

        /**
         * Processes Windows messages
         */
        LRESULT handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult);

        /**
         * Turns a Windows VK-code into a GUIKey
         */
        GUIKey translateVKToGUIKey(int vkCode);
};



#endif