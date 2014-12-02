//------------------------------------------------------------------------------------------------
// File:    volucrisguimouseinputsource.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "stdafx.h"


#define WM_MOUSEWHEEL       0x020A
#define WHEEL_DELTA         120



VolucrisGUIMouseInputSource::VolucrisGUIMouseInputSource() {
  lock_mouse_to_window_ = false;
}


void VolucrisGUIMouseInputSource::setMouseLock(bool enable) {
  lock_mouse_to_window_ = false;
}

//
//void VolucrisGUIMouseInputSource::jumpTo(GUIPoint point) {
//  WINDOWINFO window_info;
//  //GetWindowRect(main_window, &rc);
//  GetWindowInfo(main_window, &window_info);
//  lock_mouse_to_window_ = true;
//  SetCursorPos(window_info.rcClient.left + point.x, window_info.rcClient.top + point.y);
//  lock_mouse_to_window_ = false;
//}




//------------------------------------------------------------------------------------------------
// Name: handleWindowsMessage
// Desc: Processes Windows messages
//------------------------------------------------------------------------------------------------
LRESULT VolucrisGUIMouseInputSource::handleWindowsMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* returnResult)
{
    // By default, we don't need to return a result
    *returnResult = false;

    // Handle the message
    switch(uMsg)
    {
        case WM_MOUSEMOVE:
        {
            if (lock_mouse_to_window_) break;
            main_window = hWnd;

            // Get the point structure
            POINTS pt = MAKEPOINTS(lParam);
            GUIPoint guiPoint = { pt.x, pt.y };

            // Update the state
            mouseMovedTo(guiPoint);
/*
            if (lock_mouse_to_window_) {
              //SetCursorPos(CENTER_COORDINATE, CENTER_COORDINATE);
              RECT rc;
              GetWindowRect(hWnd, &rc);
              //AdjustWindowRect(&rc, WS_POPUPWINDOW|WS_CAPTION, FALSE);
              ClipCursor(&rc);
            } else {
              ClipCursor(NULL);
            }*/

        } break;

        case WM_LBUTTONDOWN:   mouseButtonAction(GUIMOUSE_DOWN); SetCapture(hWnd); break;
        case WM_LBUTTONUP:     mouseButtonAction(GUIMOUSE_CLICK_SINGLE); 
                               mouseButtonAction(GUIMOUSE_UP); ReleaseCapture(); break;
        case WM_LBUTTONDBLCLK: mouseButtonAction(GUIMOUSE_CLICK_DOUBLE); break;

        case WM_RBUTTONDOWN:   mouseButtonAction(GUIMOUSE_ALT_DOWN); SetCapture(hWnd); break;
        case WM_RBUTTONUP:     mouseButtonAction(GUIMOUSE_CLICK_ALT_SINGLE); 
                               mouseButtonAction(GUIMOUSE_ALT_UP); ReleaseCapture(); break;
        case WM_RBUTTONDBLCLK: mouseButtonAction(GUIMOUSE_CLICK_ALT_DOUBLE); break;

        case WM_MOUSEWHEEL:    mouseScroll(-(float)(((int)wParam)) / (float)(1000.0f * WHEEL_DELTA)); break;
    }

    // Success
    return S_OK;
}

