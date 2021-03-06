//------------------------------------------------------------------------------------------------
// File:    dcxd3ddialogcontroller.cpp
//
// Desc:    
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include <dcx/d3d>
#include "../dc/debug"
#include "../dcx/win32"


using namespace dcx;


//------------------------------------------------------------------------------------------------
// Name:  dcxD3DDialogController
// Desc:  Initializes the class
//------------------------------------------------------------------------------------------------
dcxD3DDialogController::dcxD3DDialogController()
{
    myMainWindow = NULL;
    myD3DDevice = NULL;
    myD3DSwapChain = NULL;
}





//------------------------------------------------------------------------------------------------
// Name:  createD3DDialog
// Desc:  Creates a new dialog that uses a new swap-chain of a preexisting Direct3D device
//------------------------------------------------------------------------------------------------
bool dcxD3DDialogController::createD3DDialog( HINSTANCE hInstance, HWND hParent, WORD menuResourceID,
        const char* caption, int width, int height, LPDIRECT3DDEVICE9 sourceD3DDevice )
{
    // Make sure no old objects currently exist
    releaseObjects();

    // Generate a dialog template
    dcxWin32DialogTemplate d3dDialog;
    d3dDialog.setStyleAsResizeableAppWindow();
    d3dDialog.setTitle( caption );
    if( menuResourceID ) d3dDialog.setMenu( menuResourceID );
    BYTE buffer[256];
    LPCDLGTEMPLATE dlgTemplate = d3dDialog.compile( buffer, sizeof(buffer) );

    // Make sure we created the template successfully
    if( APP_ERROR( !dlgTemplate )( "Window couldn't be compiled for D3D dialog" ) )
        return false;

    // Create the dialog
    HWND hDlg = createDialog( hInstance, dlgTemplate, hParent );

    // Confirm dialog creation
    if( APP_ERROR( !hDlg )( "Couldn't create D3D dialog window" ) )
        return false;

    // Make sure the window is the right size
    dcxWin32SetWindowClientSize( hDlg, width, height );

    // Generate the Direct3D device for this window and save it
    if( APP_ERROR( !dcxD3DDuplicateDeviceForWindow( sourceD3DDevice, hDlg, &myD3DSwapChain, NULL ) )
            ( "Unable to create Direct3D device for dialog window" ) )
    {
        // Get rid of the window we made
        DestroyWindow( hDlg );

        // Failure
        return false;
    }

    // Copy the device pointer and window
    myMainWindow = hDlg;
    (myD3DDevice = sourceD3DDevice)->AddRef();

    // Activate the swap chain for this dialog (just use default Z-units)
    dcxD3DSetTargetSwapChain( myD3DSwapChain, 1.0f, 1000.0f );

    // Success
    return true;
}




//------------------------------------------------------------------------------------------------
// Name:  releaseObjects
// Desc:  Erases Direct3D allocations made by this class
//------------------------------------------------------------------------------------------------
void dcxD3DDialogController::releaseObjects()
{
    if( IsWindow( myMainWindow ) )
    {
        DestroyWindow( myMainWindow );
        myMainWindow = NULL;
    }

    if( myD3DSwapChain )
    {
        myD3DSwapChain->Release();
        myD3DSwapChain = NULL;
    }

    if( myD3DDevice )
    {
        myD3DDevice->Release();
        myD3DDevice = NULL;
    }
}




//------------------------------------------------------------------------------------------------
// Name:  messagePump
// Desc:  Runs the Windows message pump.  This class returns 'true' as long as the dialog
//------------------------------------------------------------------------------------------------
bool dcxD3DDialogController::messagePump()
{
    // Handle all of the messages in the queue
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        // Clear out the message structure
        ZeroMemory( &msg, sizeof(msg) );
    }

    // As long as the main window exists, keep going
    return IsWindow( myMainWindow ) != FALSE;
}




//------------------------------------------------------------------------------------------------
// Name:  present
// Desc:  Puts whatever rendering has occurred onto the main screen
//------------------------------------------------------------------------------------------------
bool dcxD3DDialogController::present()
{
    // Make sure the swap chain exists, then try to present it to the front buffer
    if( APP_ERROR( !myD3DSwapChain )( "Can't present Direct3D dialog without swap chain!" ) )
        return false;
    else
        return !APP_ERROR( FAILED( myD3DSwapChain->Present( NULL, NULL, NULL, NULL, 0 ) ) )( "Couldn't draw swap chain" );
}

