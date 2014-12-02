//------------------------------------------------------------------------------------------------
// File:    winmain.cpp
//
// Desc:    Entry point to the Project Volucris resource editor, which is a project built using
//          the DirectCraft (dc) and DirectCraft Extensions (dcx) libraries.
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"
#include <dc/dcresourcetrader.h>

#include <dcx/inifilereader>
#include "commctrl.h"




//------------------------------------------------------------------------------------------------
// Name:  WinMain
// Desc:  Entry point for the application
//------------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    { // initialize the path
      char buffer[MAX_PATH];
      GetCurrentDirectory(MAX_PATH, buffer);
      dc::dcFileName::setCurrentPath(buffer);
    }

    INITCOMMONCONTROLSEX InitCtrlEx;
    InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCtrlEx.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&InitCtrlEx);

    if (true)
    {
        std::string s;
        dc::dcResourceTrader::printAllResourceTypes(&s);
        OutputDebugString(s.c_str());
    }

    // Execute the editor application
    return globalEditor()->Run();
}
