//------------------------------------------------------------------------------------------------
// File:  winmain.h
//
// Desc:  Declares symbols that are defined in winmain.cpp but needed elsewhere (such as the
//      global message processing method)
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __WINMAIN_H__
#define __WINMAIN_H__


/**
 * Message-processing method for the client application's main window
 */
LRESULT WINAPI MainWindowMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);




#endif