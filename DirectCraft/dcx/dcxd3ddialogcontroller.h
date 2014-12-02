//------------------------------------------------------------------------------------------------
// File:    dcxd3ddialogcontroller.h
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef __DCXD3DDIALOGCONTROLLER_H__
#define __DCXD3DDIALOGCONTROLLER_H__


namespace dcx {

    /**
     * A class that derives from this one is passed information from a standard dialog-
     * processing routine.  As an extension to dcxWin32DialogController, there are additional
     * built-in methods to help manage a Direct3D application window.
     */
    class dcxD3DDialogController : public dcxWin32DialogController
    {
        public:

            /**
             * Initializes the class
             */
            dcxD3DDialogController();


        protected:

            /**
             * Creates a new dialog that uses a new swap-chain of a preexisting Direct3D device
             */
            bool createD3DDialog( HINSTANCE hInstance, HWND hParent, WORD menuResourceID, const char* caption,
                                  int width, int height, LPDIRECT3DDEVICE9 sourceD3DDevice );

            /**
             * Erases allocations made by this class
             */
            void releaseObjects();

            /**
             * Runs the Windows message pump.  This class returns 'true' as long as the dialog
             * exists; it returns 'false' when the dialog closes.
             */
            bool messagePump();

            /**
             * Puts whatever rendering has occurred onto the main screen
             */
            bool present();


        protected:

            /// The window that the dialog uses
            HWND myMainWindow;

            /// The device that is associated with rendering the window
            LPDIRECT3DDEVICE9 myD3DDevice;

            /// The swap chain created specifically for this window
            LPDIRECT3DSWAPCHAIN9 myD3DSwapChain;
    };

}



#endif