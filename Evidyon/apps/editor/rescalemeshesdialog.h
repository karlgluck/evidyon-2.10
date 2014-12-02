//------------------------------------------------------------------------------------------------
// File:    rescalemeshesdialog.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __RESCALEMESHESDIALOG_H__
#define __RESCALEMESHESDIALOG_H__

#include <dcx/d3d>

/**
 * 
 */
class RescaleMeshesDialog : public dcx::dcxD3DDialogController
{
    public:

        /**
         * Initializes the class
         */
        RescaleMeshesDialog();

        /**
         * Runs the rescale-meshes dialog
         */
        int execute(const dc::dcGenericResource::Array* display);

        /**
         * Forces this dialog (and any of its children) to close
         */
        void end(INT_PTR result);


    protected:

        /**
         * If a menu item is selected, this method is invoked
         *   @param hDlg The dialog containing the menu
         *   @param hMenu The menu containing the selected item
         *   @param ItemID The ID of the item that was selected
         *   @param ItemIndex The index of the selected item
         */
        INT_PTR dialogMenuNotification(HWND hDlg, HMENU hMenu, WORD ItemID, WORD ItemIndex);

        /**
         * When something on the dialog is activated, this function is invoked
         *   @param hWnd Dialog handle
         *   @param hControl Control that spawned the notification
         *   @param ControlID ID of the control that created the notification
         *   @param Notification The event that occurred
         *   @return Return code
         */
        INT_PTR dialogNotification(HWND hDlg, HWND hControl, WORD ControlID, WORD Notification);

        /**
         * All messages received by the dialog are passed through here.  The default
         * implementation of this method will call EndDialog with a value of "0"
         * when the window is closed.
         *   @param hWnd Dialog handle
         *   @param uMsg Message
         *   @param wParam Parameter
         *   @param lParam Parameter
         *   @return Return code
         */
        INT_PTR dialogEvent(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:

        dcxWin32Mouse myMouse;
        dcxWin32Keyboard myKeyboard;
};



#endif