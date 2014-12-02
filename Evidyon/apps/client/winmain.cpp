//------------------------------------------------------------------------------------------------
// File:    winmain.cpp
//
// Desc:    Entry point to the client application
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "../shared/evidyon_version.h"

/// Application reference
VolucrisClient* g_Client = NULL;

#define IF(cond) if (cond) {}



//------------------------------------------------------------------------------------------------
// Name: MainWindowMsgProc()
// Desc: The window's message handler
//------------------------------------------------------------------------------------------------
LRESULT WINAPI MainWindowMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    // Pass messages to the
    g_Client->handleWindowsMessage(hWnd, uMsg, wParam, lParam);

    switch(uMsg)
    {/*
        case WM_MOUSEMOVE:
            {
                SetCursor(NULL);
                ShowCursor(FALSE);
            } break;

        case WM_NCMOUSEMOVE:
            {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                ShowCursor(TRUE);
            } break;*/

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}





//------------------------------------------------------------------------------------------------
// Name: WinMain
// Desc: 
//------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
    // Run the application
    {
        g_Client = new VolucrisClient();
        g_Client->execute();
        SAFE_DELETE(g_Client);
    }

    // Display any errors
    DEBUG_ATEXIT();

    // Exit
    return 0;
}